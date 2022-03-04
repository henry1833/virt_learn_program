#include<linux/module.h> 
#include<linux/init.h>
#include<linux/virtio.h>
#include<linux/virtio_ring.h>
#include<linux/virtio_config.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/proc_fs.h>

#ifndef VIRTIO_ID_CUSTDEV
#define VIRTIO_ID_CUSTDEV 30
#endif


#define VIRTIO_CUSTDEV_BUFFERS 1024
#define VIRTIO_CUSTDEV_STRING "virtio_custdev"

#define VIRTIO_CUSTDEV_VRING_NUM 256
#define VIRTIO_CUSTDEV_VRING_ALIGN PAGE_SIZE

struct virtio_custom_device{
    struct virtio_device vdev;
    struct virtqueue *vq_rx;
    struct virtqueue *vq_tx;
    struct proc_dir_entry *pde;
    unsigned int buffers;
    unsigned buf_lens[VIRTIO_CUSTDEV_BUFFERS];
    void *read_data;
    struct completion data_ready;
    u8 status;
};

#define to_virtio_custom_device(_dev) \
	container_of(_dev,struct virtio_custom_device,vdev)

static int virtio_custdev_open(struct inode *inode, struct file *filp)
{
    struct virtio_custom_device *vcust = PDE_DATA(inode);
    filp->private_data = vcust;

    return 0;
}

static ssize_t virtio_custdev_read(struct file *filp, char __user *buffer,size_t count, loff_t *offset){
    struct virtio_custom_device *vcust = filp->private_data;
    struct scatterlist sg;
    unsigned long res;
    char *rcv_buf;
    
    if(vcust->buffers < 1){
        printk("all buffers read!\n");
	return 0;
    }

    rcv_buf = kzalloc(vcust->buf_lens[vcust->buffers - 1],GFP_KERNEL);
    if(!rcv_buf){
        return -ENOMEM;
    }

    sg_init_one(&sg,rcv_buf,vcust->buf_lens[vcust->buffers-1]);
    virtqueue_add_inbuf(vcust->vq_rx,&sg,1,rcv_buf,GFP_KERNEL);
    virtqueue_kick(vcust->vq_rx);

    wait_for_completion(&vcust->data_ready);

    res = copy_to_user(buffer,vcust->read_data,vcust->buf_lens[vcust->buffers]);
    if(res != 0){
        printk("Could not read %lu bytes!",res);
	vcust->buf_lens[vcust->buffers] = vcust->buf_lens[vcust->buffers] -res;
    }

    kfree(rcv_buf);
    return vcust->buf_lens[vcust->buffers];
}

static ssize_t virtio_custdev_write(struct file *filp, const char __user *buffer, size_t count, loff_t *offset){
    struct virtio_custom_device *vcust = filp->private_data;
    void *to_send;
    unsigned long res;
    struct scatterlist sg;

    if(vcust->buffers >= VIRTIO_CUSTDEV_BUFFERS){
        printk("all buffers used!\n");
	return ENOSPC;
    }
    
    to_send = kmalloc(count,GFP_KERNEL);
    if(!to_send){
        return 1;
    }

    res = copy_from_user(to_send,buffer,count);
    if(res != 0){
        printk("Could not write %lu bytes!\n",res);
	count = count - res;
    }

    sg_init_one(&sg,to_send,count);
    vcust->buf_lens[vcust->buffers++] = count;

    virtqueue_add_outbuf(vcust->vq_tx,&sg,1,to_send,GFP_KERNEL);
    virtqueue_kick(vcust->vq_tx);
    return count;
}

void virtio_custdev_outbuf_cb(struct virtqueue *vq){
    int len;
    void *buf = virtqueue_get_buf(vq,&len);

    if(buf){
        kfree(buf);
    }
}

void virtio_custdev_inbuf_cb(struct virtqueue *vq){
    int len;
    struct virtio_custom_device *vcust = vq->vdev->priv;
    vcust->read_data = virtqueue_get_buf(vq,&len);
    vcust->buffers--;
    complete(&vcust->data_ready);
    printk("Received %i bytes\n",len);
}

int virtio_vcust_assign_virtqueue(struct virtio_custom_device *vcust){
    const char *names[] = {"virtio-vview-tx","virtio-vview-rx"};
    vq_callback_t *callbacks[] = {virtio_custdev_outbuf_cb, virtio_custdev_inbuf_cb};
    struct virtqueue *vqs[2];
    int err;

    err = virtio_find_vqs(&vcust->vdev,2,vqs,callbacks,names,NULL);
    if(err){
       return err;
    }

    vcust->vq_tx = vqs[0];
    vcust->vq_rx = vqs[1];
    return 0;
}

static struct file_operations vcust_fops ={
    .owner = THIS_MODULE,
    .open = virtio_custdev_open,
    .read = virtio_custdev_read,
    .write = virtio_custdev_write,
};

static unsigned int features_table[] = {
};

static struct virtio_device_id id_table[] = {
    {VIRTIO_ID_CUSTDEV,VIRTIO_DEV_ANY_ID},
    {0},
};

static int virtio_custdev_probe(struct virtio_device *vdev){
    int err;
    char proc_name[20];
    struct virtio_custom_device *vcust = to_virtio_custom_device(vdev);
    
    printk("register virtio device\n");
    if(NULL == vcust){
        printk("alloc device struct failed\n");
	return -ENOMEM;
    }

    vcust->vdev.priv = vcust;

    err = virtio_vcust_assign_virtqueue(vcust);
    if(err){
        printk("Error adding virtqueue\n");
	goto err;
    }

    vcust->buffers = 0;
    init_completion(&vcust->data_ready);
    snprintf(proc_name,sizeof(proc_name),"%s-%i",VIRTIO_CUSTDEV_STRING,vdev->index);

    vcust->pde = proc_create_data(proc_name,0644,NULL,&vcust_fops,vcust);
    if(!vcust->pde){
        printk("Create proc file interface failed\n");
	err = -ENOMEM;
	goto err;
    }

    return 0;
err:
    kfree(vcust);
    return err;
}

static void virtio_custdev_remove(struct virtio_device *vdev){
    struct virtio_custom_device *vcust = vdev->priv;
    proc_remove(vcust->pde);
    complete(&vcust->data_ready);
    kfree(vdev->priv);
    printk("virtio view device removed\n");
}

static struct virtio_driver virtio_custdev_driver = {
    .driver.name = KBUILD_MODNAME,
    .driver.owner = THIS_MODULE,
    .id_table = id_table,
    .feature_table = features_table,
    .feature_table_size = ARRAY_SIZE(features_table),
    .probe = virtio_custdev_probe,
    .remove = virtio_custdev_remove
};

//module_virtio_driver(virtio_custdev_driver);

static u8 custdev_get_status(struct virtio_device *vdev) {
    struct virtio_custom_device *vcdev = to_virtio_custom_device(vdev);

    return vcdev->status;
}

static void custdev_set_status(struct virtio_device *vdev,u8 status) {
    struct virtio_custom_device *vcdev = to_virtio_custom_device(vdev);

    vcdev->status = status;
}

static void custdev_reset(struct virtio_device *vdev) {
    struct virtio_custom_device *vcdev = to_virtio_custom_device(vdev);

    vcdev->status = 0;
}

void custdev_del_vqs(struct virtio_device *vdev)
{
    struct virtqueue *vq,*n;

    list_for_each_entry_safe(vq,n,&vdev->vqs,list){
       vring_del_virtqueue(vq);	
    }
}

bool custdev_notify(struct virtqueue *vq){
    struct virtio_custom_device *vcdev = to_virtio_custom_device(vq->vdev);

//    writel();
    return true;
}

struct virtqueue *cd_setup_vq(struct virtio_device *vdev,unsigned index,
		              void (*callback)(struct virtqueue *vq),
			      const char *name,bool ctx)
{
    int err;
    struct virtqueue *vq;

    vq = vring_create_virtqueue(index,VIRTIO_CUSTDEV_VRING_NUM,VIRTIO_CUSTDEV_VRING_ALIGN,vdev,true,true,
		                ctx,custdev_notify,callback,name);
    
    if(!vq){
        err = -ENOMEM;
    }

    return vq;
}

int custdev_find_vqs(struct virtio_device *vdev, unsigned nvqs,
		     struct virtqueue *vqs[],vq_callback_t *callbacks[],
		     const char * const names[], const bool *ctx,
		     struct irq_affinity *desc)
{
    int i;

    for(i=0; i<nvqs;i++){
        vqs[i] = cd_setup_vq(vdev,i,callbacks[i],names[i],ctx ? ctx[i]:false);

	if(IS_ERR(vqs[i])){
	    custdev_del_vqs(vdev);
	    return PTR_ERR(vqs[i]);    
	}
    }

    return 0;
}

static const struct virtio_config_ops virtio_custdev_config_ops = {
    .get_status = custdev_get_status,
    .set_status = custdev_set_status,
    .reset = custdev_reset,
    .find_vqs = custdev_find_vqs,
    .del_vqs = custdev_del_vqs,
};


struct virtio_custom_device *vcdev = NULL;

static int __init virtio_custom_init(void){
    int error;
    
    vcdev = kzalloc(sizeof(struct virtio_custom_device),GFP_KERNEL);
    if(!vcdev)
        return -ENOMEM;

    vcdev->vdev.dev.parent = NULL;
    vcdev->vdev.id.device = VIRTIO_ID_CUSTDEV;
    vcdev->vdev.id.vendor = VIRTIO_DEV_ANY_ID;
    vcdev->vdev.config = &virtio_custdev_config_ops;

    error = register_virtio_device(&vcdev->vdev);
    if(error){
        printk("register virtio device failed\n");
        goto nodev;
    }

    error = register_virtio_driver(&virtio_custdev_driver);
    if(error){
        printk("register virtio driver failed\n");
	goto err;
    }

    return 0;
err:
    unregister_virtio_device(&vcdev->vdev);
nodev:
    kfree(vcdev);
    return error;
}

static void __exit virtio_custom_exit(void){
    unregister_virtio_driver(&virtio_custdev_driver);
    unregister_virtio_device(&vcdev->vdev);
    kfree(vcdev);
}

module_init(virtio_custom_init);
module_exit(virtio_custom_exit);

MODULE_DEVICE_TABLE(virtio,id_table);
MODULE_DESCRIPTION("virtio example front-end driver");
MODULE_LICENSE("GPL");
