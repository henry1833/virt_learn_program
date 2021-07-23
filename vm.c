#include<err.h>
#include<fcntl.h>
#include<linux/kvm.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<getopt.h>
#include<sys/ioctl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>

#define KVM_API_VERSION 12

static int cpu_nums=1;
static size_t vm_memory_size = 0x200000;
static char *filename = NULL;

struct KVMState{
	int fd;
	int vmfd;
};

int kvm_init(struct KVMState *s)
{
	int ret;
	s->fd = open("/dev/kvm",O_RDWR);
	if(s->fd == -1)
	{
		fprintf(stderr,"Could not access KVM kernel module:%m\n");
		ret = -errno;
		goto err;
	}
	
	ret = ioctl(s->fd,KVM_GET_API_VERSION,0);
	if(ret == KVM_API_VERSION)
	{
		fprintf(stderr,"kvm version not supported\n");
		ret = -EINVAL;
		goto err;
	}
	ret = 0;
err:
    return ret;	
}


void show_usage()
{
    printf("Usages:\n"
           "-smp create cpu numbers\n"
	   "-mem vm memory size\n"
	   "-f To run binary filename\n"
	   "-h display this help and exit\n"
    );
}

static struct option long_options[]=
{
        {"smp",required_argument,0,'n'},
        {"mem",required_argument,0,'m'},
        {"file",required_argument,0,'f'},
        {"help",no_argument,0,'H'},
        {0,0,0,0}
};


int parse_argv(int argc,char **argv)
{
    int c = 0;
    int option_index =0;
    
    while((c = getopt_long(argc,argv,"f:m:hH",long_options,&option_index)) != -1){
        switch(c){
            case 'h':
            case 'H':
                show_usage();
                break;
            case 'm':
                vm_memory_size = atoi(optarg); 
                break;
            case 'n':
                cpu_nums = atoi(optarg);
                break;
            case 'f':
                filename = strdup(optarg);
                break;
            default:
                printf("invalid parameter\n");
        }
    }

    printf("cpu nums:%d  memory size:0x%lx filename:%s\n",cpu_nums,vm_memory_size,filename);
    return 0;    
}

int main(int argc,char **argv)
{
    int fd,kvm,vmfd,vcpufd,ret;
    uint8_t buff[255];
    uint8_t *mem;
    struct kvm_sregs sregs;
    size_t mmap_size;
    struct kvm_run *run;
    struct KVMState *ks;
	
    ret = parse_argv(argc,argv); 
    if(ret)
        exit(-1);

    ks = malloc(sizeof(struct KVMState));
    
	ret = kvm_init(ks);
	if(ret)
	{
		fprintf(stderr,"kvm init failed\n");
		exit(-1);
	}
    
    fd = open(argv[1],O_RDWR);

    if(fd == -1)
	err(1,"Open binary fail");

    vmfd = ioctl(kvm,KVM_CREATE_VM,(unsigned long)0);
    if(vmfd == -1)
	err(1,"KVM_CREATE_VM");

    mem = mmap(NULL,0x1000,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    if(!mem)
        err(1,"allocating guest memory");

     while((ret=read(fd,buff,255))>0)
     {
        memcpy(mem,buff,ret);
     }

//    memcpy(mem,code,sizeof(code));
    close(fd);
    struct kvm_userspace_memory_region region = {
        .slot = 0,
	.guest_phys_addr =0x1000,
	.memory_size = 0x1000,
	.userspace_addr = (uint64_t)mem,
    };

    ret = ioctl(vmfd,KVM_SET_USER_MEMORY_REGION,&region);
    if(ret == -1)
        err(1,"KVM_SET_USER_MEMORY_REGION");

    vcpufd = ioctl(vmfd,KVM_CREATE_VCPU,(unsigned long)0);
    if(vcpufd == -1)
    {
        err(1,"KVM_CREATE_VCPU");
    }

    ret = ioctl(kvm,KVM_GET_VCPU_MMAP_SIZE,NULL);
    if(ret ==-1)
    {
        err(1,"KVM_GET_VCPU_MMAP_SIZE");
    }	    

    mmap_size = ret;
    if(mmap_size < sizeof(*run))
	err(1,"KVM_GET_VCPU_MMAP_SIZE unexpectedly small");

    run = mmap(NULL,mmap_size,PROT_READ|PROT_WRITE,MAP_SHARED,vcpufd,0);
    if(!run)
	err(1,"mmap vcpu");

    ret = ioctl(vcpufd,KVM_GET_SREGS,&sregs);
    if(ret == -1)
    {
	err(1,"KVM_GET_SREGS");
    }

    sregs.cs.base = 0;
    sregs.cs.selector =0;

    ret = ioctl(vcpufd,KVM_SET_SREGS,&sregs);
    if(ret == -1)
	err(1,"KVM_SET_SREGS");

    struct kvm_regs regs = {
        .rip = 0x1000,
	.rax = 2,
	.rbx = 2,
	.rflags = 0x2,
    };

    ret = ioctl(vcpufd,KVM_SET_REGS,&regs);
    if(ret == -1)
	err(1,"KVM_SET_REGS");

    while(1)
    {
        ret = ioctl(vcpufd,KVM_RUN,NULL);
	if(ret == -1)
	    err(1,"KVM_RUN");
	switch(run->exit_reason){
		case KVM_EXIT_IO:
		    if(run->io.direction == KVM_EXIT_IO_OUT &&
				    run->io.size == 1 && run->io.port == 0x3f8
				    && run->io.count ==1)
			    putchar(*(((char *)run)+run->io.data_offset));
		    else
			    errx(1,"unhandled KVM_EXIT_IO");
		    break;
	    case KVM_EXIT_MMIO:
		    break;
		case KVM_EXIT_SHUTDOWN:
		    break;
		case KVM_EXIT_UNKNOWN:
		    fprintf(stderr,"KVM: unknown exit,hardware reason %lx\n",
			(uint64_t)run->hw.hardware_exit_reason);
		    break;
		case KVM_EXIT_SYSTEM_EVENT:
		     break;
		case KVM_EXIT_FAIL_ENTRY:
		    errx(1,"KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason =0x%llx",
(unsigned long long)run->fail_entry.hardware_entry_failure_reason);

		case KVM_EXIT_INTERNAL_ERROR:
		    errx(1,"KVM_EXIT_INTERNAL_ERROR: suberror =0x%llx",
(unsigned long long)run->internal.suberror);

        case KVM_EXIT_HLT:
	            puts("KVM_EXIT_HLT");
		    return 0;
		default:
		    errx(1,"exit_reason =0x%x",run->exit_reason);
	}
    }
}
