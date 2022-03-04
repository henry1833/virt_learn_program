#include "qemu/osdep.h"
#include "qemu/iov.h"
#include "hw/hw.h"
#include "hw/virtio/virtio.h"
#include "qemu/error-report.h"
#include "sysemu/runstate.h"
#include "standard-headers/linux/virtio_ids.h"

#define TYPE_VIRTIO_CUSTDEV "virtio-custom-device"
#define VIRTIO_CUSTDEV(obj) OBJECT_CHECK(VirtIOCustdev,(obj),TYPE_VIRTIO_CUSTDEV)

#define VIRTIO_CUSTDEV_BUFFERS 1024

void virtio_custdev_print(const char *fmt,...) GCC_FMT_ATTR(1,2);

typedef struct VirtIOCustdev {
  VirtIODevice parent_obj;
  VirtQueue *vq_tx;
  VirtQueue *vq_rx;
  void *rcv_bufs[VIRTIO_CUSTDEV_BUFFERS];
  unsigned int rcv_count;
} VirtIOCustdev;

void GCC_FMT_ATTR(1,2) virtio_custdev_print(const char *fmt, ...){
    va_list ap;

    va_start(ap,fmt);
    error_vreport(fmt,ap);
    va_end(ap);
}

static uint64_t virtio_custdev_get_features(VirtIODevice *vdev, uint64_t f, Error **errp){
    return f;
}


static void virtio_custdev_set_status(VirtIODevice *vdev, uint8_t status){
    if(!vdev->vm_running){
        return;
    }

    vdev->status = status;
}


static void virtio_custdev_handle_outbuf(VirtIODevice *vdev, VirtQueue *vq){
    VirtIOCustdev *vcust = VIRTIO_CUSTDEV(vdev);
    VirtQueueElement *vqe;

    while(!virtio_queue_ready(vq)){
        virtio_custdev_print("not ready");
        return;
    }

    if(!runstate_check(RUN_STATE_RUNNING)){
        virtio_custdev_print("not synced ");
	return;
    }

    vqe = virtqueue_pop(vq,sizeof(VirtQueueElement));
    vcust->rcv_bufs[vcust->rcv_count] = malloc(vqe->out_sg->iov_len);
    iov_to_buf(vqe->out_sg,vqe->out_num,0,vcust->rcv_bufs[vcust->rcv_count],vqe->out_sg->iov_len);
    virtio_custdev_print("received: %li bytes",vqe->out_sg->iov_len);
    vcust->rcv_count++;
    virtqueue_push(vq,vqe,0);
    virtio_notify(vdev,vq);
    g_free(vqe);
    return;
}

static void virtio_custdev_handle_inbuf(VirtIODevice *vdev, VirtQueue *vq){
    VirtIOCustdev *vcust = VIRTIO_CUSTDEV(vdev);
    VirtQueueElement *vqe;
    unsigned int last_buf = vcust->rcv_count - 1;

    while(!virtio_queue_ready(vq)){
        virtio_custdev_print("not ready");
	return;
    }

    if(!runstate_check(RUN_STATE_RUNNING)){
        virtio_custdev_print("not synced");
	return;
    }    

    vqe = virtqueue_pop(vq,sizeof(VirtQueueElement));

    iov_from_buf(vqe->in_sg,vqe->in_num,0,vcust->rcv_bufs[last_buf],vqe->in_sg->iov_len);
    vcust->rcv_count--;
    virtqueue_push(vq,vqe,vqe->in_sg->iov_len);
    virtio_notify(vdev,vq);
    g_free(vcust->rcv_bufs[last_buf]);
    g_free(vqe);
    return;
}

static void virtio_custdev_device_realize(DeviceState *dev, Error **errp){
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);
    VirtIOCustdev *vcust = VIRTIO_CUSTDEV(dev);
    virtio_init(vdev,"virtio-custdev",VIRTIO_ID_CUSTDEV,0);
    vcust->vq_rx = virtio_add_queue(vdev,64,virtio_custdev_handle_outbuf);
    vcust->vq_tx = virtio_add_queue(vdev,64,virtio_custdev_handle_inbuf);
    vcust->rcv_count  = 0;
}

static void virtio_custdev_device_unrealize(DeviceState *dev){
    int i;
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);
    VirtIOCustdev *vcust = VIRTIO_CUSTDEV(dev);
    for(i=0; i < vcust->rcv_count; i++){
        g_free(vcust->rcv_bufs[i]);
    }

    virtio_cleanup(vdev);
}

/*
static Property virtio_custdev_properties[] = {
    DEFINE_PROP_STRING(),
    DEFINE_PROP_END_OF_LIST(),
};
*/

static void virtio_custdev_class_init(ObjectClass *klass,void *data){
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioDeviceClass *vdc = VIRTIO_DEVICE_CLASS(klass);

//    device_class_set_props(dc,virtio_custdev_properties);
    set_bit(DEVICE_CATEGORY_MISC,dc->categories);
    vdc->realize = virtio_custdev_device_realize;
    vdc->unrealize = virtio_custdev_device_unrealize;
    vdc->get_features = virtio_custdev_get_features;
    vdc->set_status = virtio_custdev_set_status;
}

static const TypeInfo virtio_custdev_info = {
    .name = TYPE_VIRTIO_CUSTDEV,
    .parent = TYPE_VIRTIO_DEVICE,
    .instance_size = sizeof(VirtIOCustdev),
    .class_init = virtio_custdev_class_init,
};

static void virtio_custdev_register_types(void) {
    type_register_static(&virtio_custdev_info);
}

type_init(virtio_custdev_register_types)
