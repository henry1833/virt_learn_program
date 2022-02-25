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
#include<sys/eventfd.h>
#include<errno.h>
#include<ctype.h>
#include<pthread.h>
#include<cpuid.h>

#define KVM_API_VERSION 12

static int cpu_nums=1;
static size_t vm_memory_size = 0x200000;
static char *filename = NULL;

struct vm_parameter{
    int cpu_nums;
	size_t vm_memory_size;	
	char *filename;
};

struct VCPUState{
    int  vcpufd;
    struct kvm_run *kvm_run;
};

struct KVMState{
	int fd;
	int vmfd;
	int ioefd;
	int irqefd;
	struct VCPUState *cpus;
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
	if(ret != KVM_API_VERSION)
	{
		fprintf(stderr,"kvm version not supported ret:%d\n",ret);
		ret = -EINVAL;
		goto err;
	}
	ret = eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
	if(ret > 0){
		s->ioefd = ret;
		ret = 0;
	}
	
	ret = eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
	if(ret > 0){
		s->irqefd = ret;
		ret = 0;
	}
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
    int i,len,base,val;

    while((c = getopt_long(argc,argv,"f:m:hH",long_options,&option_index)) != -1){
        switch(c){
            case 'h':
            case 'H':
                show_usage();
                break;
            case 'm':
		len = strlen(optarg);

		if(optarg[0]=='0' && optarg[1] =='x'){
		    base = 0x10;
		    i = 2;
		}
		else{
		    base = 10;
		    i = 0;
		}
                
		vm_memory_size = 0;
		while(i< len){
		    if(isxdigit(optarg[i]))
		    {
			if(optarg[i] >= '0' && optarg[i] <= '9')
			{
		            val = optarg[i] - '0';
			}
			else if(optarg[i] >= 'A' && optarg[i] < 'F'){
			    val = optarg[i] - 'A' + 10;
			}
			else if(optarg[i] >= 'a' && optarg[i] < 'f'){
			    val = optarg[i] - 'a' + 10;
			}
		        vm_memory_size *= base;
		        vm_memory_size += val; 	
		    }else
		    {
		        break; 
		    }
		    i++;
		}
                
//		printf("vm_memory_size:%d\n",vm_memory_size);
		if(!isalpha(optarg[i]) || isalpha(optarg[i+1])){
		    printf("Invalid parameter %d\n",i);
		    return -1;
		}

		switch(optarg[i]){
		    case 'K':
	            case 'k':
			vm_memory_size *= 0x400;
			break;
		    case 'M':
	            case 'm':
			vm_memory_size *= 0x100000;
	                break;
		    case 'g':
		    case 'G':
			vm_memory_size *= 0x40000000;
			break;
		    default:
			printf("Invalid parameter\n");
			return -1;
		}
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


#define KVM_MAX_CPUID_ENTRIES 100

int vm_init_cpuid(struct KVMState *ks)
{
    struct {
        struct kvm_cpuid2 cpuid;
        struct kvm_cpuid_entry2 entries[KVM_MAX_CPUID_ENTRIES];
    } cpuid_data;    
    uint32_t limit,i,j,cpuid_i;
    uint32_t unused;
    struct kvm_cpuid_entry2 *c;
    
    int ret =0;

    cpuid_i = 0;
    memset(&cpuid_data,0,sizeof(cpuid_data));

    __get_cpuid(0x0,&limit,&unused,&unused,&unused);
    
    for(i=0; i <= limit; i++){
	if (cpuid_i == KVM_MAX_CPUID_ENTRIES) {
            fprintf(stderr, "unsupported level value: 0x%x\n", limit);
	    return -1;
        }
	
	c = &cpuid_data.entries[cpuid_i++];
	switch(i) {
        case 4:
        case 0xb:
        case 0xd:
            for (j = 0; ; j++) {
                if (i == 0xd && j == 64) {
                    break;
                }

                if (i == 0x1f && j == 64) {
                    break;
                }

                c->function = i;
                c->flags = KVM_CPUID_FLAG_SIGNIFCANT_INDEX;
                c->index = j;
                __get_cpuid_count(i, j, &c->eax, &c->ebx, &c->ecx, &c->edx);

                if (i == 4 && c->eax == 0) {
                    break;
                }
                if (i == 0xb && !(c->ecx & 0xff00)) {
                    break;
                }
                if (i == 0x1f && !(c->ecx & 0xff00)) {
                    break;
                }
                if (i == 0xd && c->eax == 0) {
                    continue;
                }
                if (cpuid_i == KVM_MAX_CPUID_ENTRIES) {
                    fprintf(stderr, "cpuid_data is full, no space for "
                            "cpuid(eax:0x%x,ecx:0x%x)\n", i, j);
		    return -1;
                }
                c = &cpuid_data.entries[cpuid_i++];
            }
            break;
	case 0x7:
	case 0x14: {
            uint32_t times;
            c->function = i;
            c->index = 0;
            c->flags = KVM_CPUID_FLAG_SIGNIFCANT_INDEX;
	    __get_cpuid(i,&c->eax,&c->ebx,&c->ecx,&c->edx);
            times = c->eax;	    
	   
	    for (j = 1; j <= times; ++j) {
                if (cpuid_i == KVM_MAX_CPUID_ENTRIES) {
                    fprintf(stderr, "cpuid_data is full, no space for "
                                "cpuid(eax:0x%x,ecx:0x%x)\n", i, j);
                    return -1;
                }
                c = &cpuid_data.entries[cpuid_i++];
                c->function = i;
                c->index = j;
                c->flags = KVM_CPUID_FLAG_SIGNIFCANT_INDEX;
	        __get_cpuid_count(i,j,&c->eax,&c->ebx,&c->ecx,&c->edx);
            }
            break;
	}
	default:
	    c->function = i;
	    c->flags = 0;
	    __get_cpuid(i,&c->eax,&c->ebx,&c->ecx,&c->edx);
	    if(!c->eax && !c->ebx && !c->ecx && !c->edx){
	        cpuid_i--;
	    }
	    break;
	}
    }

    __get_cpuid(0x80000000,&limit,&unused,&unused,&unused);

    for(i=0x80000000; i <= limit; i++){
        if (cpuid_i == KVM_MAX_CPUID_ENTRIES) {
            fprintf(stderr, "unsupported level value: 0x%x\n", limit);
            return -1;
        }
	c = &cpuid_data.entries[cpuid_i++];

	c->function = i;
	c->flags = 0;
	__get_cpuid(i,&c->eax,&c->ebx,&c->ecx,&c->edx);
	if(!c->eax && !c->ebx && !c->ecx && !c->edx){
	        cpuid_i--;
	}
	
    }

    cpuid_data.cpuid.nent = cpuid_i;
    cpuid_data.cpuid.padding = 0;
    ret = ioctl(ks->cpus->vcpufd,KVM_SET_CPUID2,&cpuid_data);

    return ret;
}

#define virq 0x48


int set_ioeventfd_mmio(struct KVMState *ks,uint64_t addr,uint32_t size)
{
	int ret;
	struct kvm_ioeventfd iofd = {
		.datamatch = 0x5d7d,
		.addr = addr,
		.len = size,
		.flags = KVM_IOEVENTFD_FLAG_DATAMATCH,
		.fd = ks->ioefd,
	};
	
	struct kvm_irqfd irqfd = {
        .fd = ks->irqefd,
		.gsi = virq,
		.flags = 0,
	};
/*	
	ret = ioctl(ks->vmfd,KVM_IOEVENTFD,&iofd);
	if(ret < 0){
		return -errno;
	}
*/
	
	ret = ioctl(ks->vmfd,KVM_IRQFD,&irqfd);
	if(ret < 0){
		printf("register IRQFD failed\n");
		return -errno;
	}
	
	
	return 0;
}


static void* wait_eventfd(void *arg)
{
	struct KVMState *ks = (struct KVMState*)arg;
	int retval;
	int s;
	uint64_t eftd_ctr;
	fd_set rfds;
//	fd_set wfds;
	
	printf("thread worker start running...\n");
	
	FD_ZERO(&rfds);
//	FD_ZERO(&wfds);
	
	while(1){
	    FD_SET(ks->ioefd,&rfds);
//		FD_SET(ks->efd,&wfds);
    
	    retval = select(ks->ioefd+1,&rfds,NULL,NULL,NULL);
	
	    if(retval == -1){
		    printf("\n select() error, Exiting......\n");
	    }else if(retval > 0){
			if(FD_ISSET(ks->ioefd,&rfds)){
		        s = read(ks->ioefd,&eftd_ctr,sizeof(uint64_t));
		        if(s != sizeof(uint64_t)){
			        printf("\neventfd read error. Exiting...\n");
		        }else{
			        printf("\neventfd from read(), value read = 0x%llx\n",eftd_ctr);
		        }

			}
/*			
			if(FD_ISSET(ks->efd,&wfds)){
                printf("printf eventfd write\n");

				eftd_ctr = 0x9d9d;
				s = write(ks->efd,&eftd_ctr,sizeof(uint64_t));
		        if(s != sizeof(uint64_t)){
			        printf("\neventfd write error. Exiting...\n");
		        }else{
			        printf("\neventfd write(), write size = 0x%x\n",s);
		        }

             
			}
*/
//           sleep(2);
	   }else if(retval == 0){
		        printf("\nSelect() says that no data was available\n");
	   }
	}
}



int main(int argc,char **argv)
{
    int fd,kvm,vcpufd,ret;
    uint8_t buff[256];
    uint8_t *mem1,*mem2;
    struct kvm_sregs sregs;
    struct kvm_run *run;
    size_t mmap_size;
    struct KVMState *ks;
    struct stat stat;
	
    ret = parse_argv(argc,argv); 
    if(ret)
        exit(-1);

    ks = malloc(sizeof(struct KVMState));
    ks->cpus = malloc(sizeof(struct VCPUState));
	
    ret = kvm_init(ks);
    if(ret)
    {
        fprintf(stderr,"kvm init failed\n");
        exit(-1);
    }
    
    fd = open(filename,O_RDWR);

    if(fd == -1)
	err(1,"Open binary fail\n");
    
    ret = fstat(fd,&stat);
    if(ret)
    {
        fprintf(stderr,"Acquire binary file information failed\n");
	    exit(-1);
    }

    ks->vmfd = ioctl(ks->fd,KVM_CREATE_VM,(unsigned long)0);
    if(ks->vmfd == -1)
	err(1,"KVM_CREATE_VM");

    mem1 = mmap(NULL,0x80000,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    if(!mem1)
        err(1,"allocating guest memory");


    ret = read(fd,(mem1+0xc00),stat.st_size+1);
    if(ret < 0)
    {
        fprintf(stderr,"Read binary failed\n");
        close(fd);
        exit(-1);
    }

    close(fd);
    struct kvm_userspace_memory_region region1 = {
        .slot = 0,
        .guest_phys_addr =0x7000,
        .memory_size = 0x80000,
        .userspace_addr = (uint64_t)mem1,
    };

    ret = ioctl(ks->vmfd,KVM_SET_USER_MEMORY_REGION,&region1);
    if(ret == -1)
        err(1,"KVM_SET_USER_MEMORY_REGION region 1");

    mem2 = mmap(NULL,0x4000000,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    if(!mem2)
        err(1,"allocating guest memory");

    struct kvm_userspace_memory_region region2 = {
        .slot = 2,
	    .guest_phys_addr =0x100000,
	    .memory_size = 0x4000000,
	    .userspace_addr = (uint64_t)mem2,
    };

    ret = ioctl(ks->vmfd,KVM_SET_USER_MEMORY_REGION,&region2);
    if(ret == -1)
        err(1,"KVM_SET_USER_MEMORY_REGION region 2");


    struct kvm_enable_cap cap = {
        .cap = KVM_CAP_SPLIT_IRQCHIP,
	    .flags = 0,
	    .args[0] = 24,
    };

    ret = ioctl(ks->vmfd,KVM_ENABLE_CAP,&cap);
    if(ret){
        fprintf(stderr,"Could not enable split irqchip mode\n");
    }

	
	struct kvm_irq_routing *irq_routes = malloc(sizeof(struct kvm_irq_routing) + sizeof(struct kvm_irq_routing_entry));
	
    irq_routes->nr = 1;
    irq_routes->flags = 0;
	
    irq_routes->entries[0].gsi = virq;
    irq_routes->entries[0].type = KVM_IRQ_ROUTING_MSI;
    irq_routes->entries[0].flags = 0;
    irq_routes->entries[0].u.msi.address_lo = (0xfee00000|0xff<<12);
    irq_routes->entries[0].u.msi.address_hi = 0;
    irq_routes->entries[0].u.msi.data = (0x30);
 
	
	ret = ioctl(ks->vmfd,KVM_SET_GSI_ROUTING,irq_routes);
	if(ret){
		fprintf(stderr,"Create IRQCHIP FAILED! %s\n",strerror(errno));
	}


    ks->cpus->vcpufd = vcpufd = ioctl(ks->vmfd,KVM_CREATE_VCPU,(unsigned long)0);
    if(ks->cpus->vcpufd == -1)
    {
        err(1,"KVM_CRE  ATE_VCPU");
    }

    ret = vm_init_cpuid(ks);
    mmap_size = ioctl(ks->fd,KVM_GET_VCPU_MMAP_SIZE,NULL);
    if(mmap_size < 0)
    {
        err(1,"KVM_GET_VCPU_MMAP_SIZE");
    }else if(mmap_size < sizeof(struct kvm_run)){	    
	    err(1,"KVM_GET_VCPU_MMAP_SIZE unexpectedly small");
    }

    ret = set_ioeventfd_mmio(ks,0x8000000,0x8);
	if(ret < 0){
		err(1,"Set ioeventd mmio");
	}

    ks->cpus->kvm_run = run = mmap(NULL,mmap_size,PROT_READ|PROT_WRITE,MAP_SHARED,vcpufd,0);
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
        .rip = 0x7c00,
        .rax = 2,
        .rbx = 2,
        .rflags = 0x82,
    };

    ret = ioctl(vcpufd,KVM_SET_REGS,&regs);
    if(ret == -1){
	   err(1,"KVM_SET_REGS");
    }

    pthread_t pthid;
/*	
	ret = pthread_create(&pthid,NULL,wait_eventfd,ks);
	if(ret!=0){
		err(1,"Create pthread failed\n");
	}
*/	
    uint64_t phys_addr; 

    while(1)
    {
        ret = ioctl(vcpufd,KVM_RUN,NULL);
        if(ret == -1){
            err(1,"KVM_RUN");
        }
        switch(run->exit_reason){
        case KVM_EXIT_IO:
            if(run->io.direction == KVM_EXIT_IO_OUT &&
	           run->io.size == 1 && run->io.port == 0x3f8
	           && run->io.count ==1){
	           putchar(*(((char *)run)+run->io.data_offset));
	        }
	        else{
	           printf("port:0x%x size:%d count:%d\n",run->io.port,run->io.size,run->io.count);
                errx(1,"unhandled KVM_EXIT_IO");
	        }
	        break;
        case KVM_EXIT_MMIO:
	        phys_addr = run->mmio.phys_addr;
            if(phys_addr >= 0xb8000 && phys_addr <= 0xba000){
		        if((phys_addr - 0xb8000) % (2 *80) == 0)
		        {
		           printf("\n");
		        }	
		        printf("%c",run->mmio.data[0]);
	        }else if(phys_addr == 0x8000000)
			{
				int res;
				uint64_t ctrval=0x1;
				printf("\nPage fault mmio address:0x%llx value:0x%llx\n",phys_addr,*((uint64_t*)run->mmio.data));
				getchar();
				res = write(ks->irqefd,&ctrval,sizeof(uint64_t));
				if(res != sizeof(ctrval)){
					printf("write data failed:%s\n",strerror(errno));
				}
				
			}else{	   
		        printf("KVM: mmoi exit\nphys_addr:0x%lx len:0x%lx is_write:%d\n",
			       run->mmio.phys_addr,run->mmio.len,run->mmio.is_write);
                ret = ioctl(vcpufd,KVM_GET_REGS,&regs);
                printf("rip:0x%lx rflags:%lx\n",regs.rip,regs.rflags);
				exit(-1);
            }
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
            errx(1,"exit_reason =0x%x\n",run->exit_reason);
	    }
//	printf("exit_reason =0x%x\n",run->exit_reason);
//        break;
    }
    return 0;
}
