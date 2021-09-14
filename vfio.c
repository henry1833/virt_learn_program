#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include <string.h>
#include<sys/eventfd.h>
#include<sys/ioctl.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<libgen.h>

#include <glib.h>
#include <glib/gprintf.h>

#include<linux/ioctl.h>
#include<linux/vfio.h>

#define MAP_SIZE (1UL * 1024 * 1024 * 1024)
#define MAP_CHUNK (4 * 1024)
#define REALLOC_INTERVAL 30

uint8_t conf_data[512];

struct pci_config{
	uint16_t Vendor_id;
	uint16_t Device_id;
	uint16_t command;
	uint16_t status;
	uint32_t Revision_id:8;
	uint32_t Class_Code:24;
}__attribute__((packed));

struct pdev_pci{
	char *sysfsdev;
	char *name;
	int dev_fd;
	int group_fd;
	int group_id;
};

void print_pci(void *data)
{
	struct pci_config *ptr = (struct pci_config*)data;
	
	printf("Vendor id: %04x  Device id:%04x\n",ptr->Vendor_id,ptr->Device_id);
	printf("command:   %04x  statue:   %04x\n",ptr->command,ptr->status);
	printf("class code:%04x   Rev_id:   %4x\n",ptr->Class_Code,ptr->Revision_id);
}

int init_pci_data(struct pdev_pci *dev)
{
	int len;
    int domain=0,bus=0,slot=0x16,function=0;
	struct stat st;
    char *tmp,group_path[512], *group_name;
	
	dev->sysfsdev=g_strdup_printf("/sys/bus/pci/devices/%04x:%02x:%02x.%01x",domain,bus,slot,function);	
	
	
	if(stat(dev->sysfsdev,&st) < 0){
		perror("no such host device");
		return -1;
	}
	
	dev->name = g_path_get_basename(dev->sysfsdev);
	
	tmp = g_strdup_printf("%s/iommu_group",dev->sysfsdev);
    len = readlink(tmp,group_path,sizeof(group_path));
	g_free(tmp);
	
	group_path[len] = 0;
	printf("after readlink, get group path result:%s\n",group_path);
	
	group_name = basename(group_path);
	
	if(sscanf(group_name,"%d",&dev->group_id)!=1){
		printf("failed to read %s\n",group_path);
		return -1;
	}
     
	 return 0;
}


int main()
{
	int ret;
	int container,i;
	unsigned long count;
	struct pdev_pci *pdev;
	void **maps;
	struct vfio_group_status group_status = {.argsz = sizeof(group_status) };
	struct vfio_iommu_type1_info iommu_info = { .argsz = sizeof(iommu_info) };
	struct vfio_iommu_type1_dma_map dma_map = { .argsz = sizeof(dma_map) };
	struct vfio_iommu_type1_dma_unmap dma_unmap = { .argsz = sizeof(dma_unmap) };
	struct vfio_device_info device_info = { .argsz = sizeof(device_info) };

	struct vfio_region_info region_info = { .argsz =  sizeof(region_info) };
	struct vfio_irq_info irq = { .argsz = sizeof(irq)};
    char path[512];
	
    pdev = malloc(sizeof(struct pdev_pci));

	if(init_pci_data(pdev)){
		perror("initilizal data failed!");
		return -1;
	}
	
	container = open("/dev/vfio/vfio",O_RDWR);

	ret = ioctl(container, VFIO_GET_API_VERSION);
	
	if(ioctl(container, VFIO_GET_API_VERSION) != VFIO_API_VERSION)
    {
		printf("error vfio version\n");
		return 0;
	}	

    if(!ioctl(container, VFIO_CHECK_EXTENSION,VFIO_TYPE1_IOMMU))
	{
		printf("not support type1 iommmu\n");
		return 0;
	}
	
	snprintf(path,sizeof(path),"/dev/vfio/%d",pdev->group_id);
	
	printf("path:%s\n",path);
	
	pdev->group_fd = open(path,O_RDWR);
	if(pdev->group_fd<0){
		perror("failed to open group");
		goto err;
	}
	
	ret = ioctl(pdev->group_fd, VFIO_GROUP_GET_STATUS,&group_status);
    
	if(!(group_status.flags & VFIO_GROUP_FLAGS_VIABLE))
    {
		printf("not suport VIABLE\n");
		goto err0;
	}	

	ret = ioctl(pdev->group_fd,VFIO_GROUP_SET_CONTAINER,&container);
	if (ret) {
		printf("Failed to set group container\n");
		goto err0;
	}

	ret = ioctl(container, VFIO_SET_IOMMU, VFIO_TYPE1_IOMMU);
	if (ret) {
		perror("VFIO set iommu failed");
		goto err1;
	}

    printf("pdev->fd:%d,pdev->name:%s\n",pdev->group_fd,pdev->name);
    pdev->dev_fd = ioctl(pdev->group_fd,VFIO_GROUP_GET_DEVICE_FD,pdev->name);
    if(pdev->dev_fd < 0){
		perror("Failed to get device fd");
		goto err0;
	}

    ret = ioctl(pdev->dev_fd,VFIO_DEVICE_GET_INFO,&device_info);
	if(ret)
	{
		perror("error getting device_info");
		goto err1;
	}
	
	printf("device num regions:%d   num_irqs:%d\n",device_info.num_regions,device_info.num_irqs);

	region_info.index = VFIO_PCI_CONFIG_REGION_INDEX;
	printf("region_info argsz:%d index:%x\n",region_info.argsz,region_info.index);
    ret = ioctl(pdev->dev_fd,VFIO_DEVICE_GET_REGION_INFO,&region_info);
    if(ret)
	{
		perror("get device  region info failed!");
	}		
	printf("region info flags:%x argsz:%d offset:%lx\n",region_info.flags,region_info.argsz,region_info.offset);
	
	ret = pread(pdev->dev_fd,conf_data,64,region_info.offset);  
    if(ret<0)
    {
		perror("read config data failed!");
	}		

    print_pci(conf_data);
	
	
err1:
    close(pdev->dev_fd);
err0:
    close(pdev->group_fd);
err:
	close(container);
	g_free(pdev->sysfsdev);
    free(pdev);    
	return 0;
}
