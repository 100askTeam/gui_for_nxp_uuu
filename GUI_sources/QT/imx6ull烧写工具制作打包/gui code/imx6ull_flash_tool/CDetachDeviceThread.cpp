#include "CDetachDeviceThread.h"
#include <QDebug>

CDetachDeviceThread::CDetachDeviceThread(QObject *parent) : QThread(parent)
{

}

CDetachDeviceThread::~CDetachDeviceThread()
{
    qDebug()<< "~CDetachDeviceThread";
}


void CDetachDeviceThread::run()
{
    DetchDeviceInit();
}



int CDetachDeviceThread::DetchDeviceInit()
{
    libusb_hotplug_callback_handle hp[2];
    //句柄，具体啥作用目前未知
    libusb_hotplug_callback_handle handle_Arrive,handle_Left;
    int rc;

    //注册hotplug
     //需要监控的VID，设置为LIBUSB_HOTPLUG_MATCH_ANY，则不判断VID
     int vendor_id = LIBUSB_HOTPLUG_MATCH_ANY;
     //需要监控的PID 设置为LIBUSB_HOTPLUG_MATCH_ANY，则不判断PID
     int product_id = LIBUSB_HOTPLUG_MATCH_ANY;
     //需要监控的设备的class,设置为LIBUSB_HOTPLUG_MATCH_ANY，则不判断，与libusb_device_descriptor的class匹配
     //这个参数不是很懂
     int class_id = LIBUSB_HOTPLUG_MATCH_ANY;
     //设备需要监听的事件，插入和拔出
     libusb_hotplug_event event_ARRIVED = static_cast<libusb_hotplug_event> (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED);
     libusb_hotplug_event event_LEFT = static_cast<libusb_hotplug_event> (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT);

     //typedef enum {
       //    LIBUSB_HOTPLUG_NO_FLAGS = 0,//只有在发生拔插的时候才会调用注册的回调函数
       //    LIBUSB_HOTPLUG_ENUMERATE = 1<<0,//在初始化前设备已经插入，也会调用注册的回调函数
       //} libusb_hotplug_flag;
       libusb_hotplug_flag flag = static_cast<libusb_hotplug_flag>(1);

       //设备回调过来的数据
        void *user_data;


    // usb初始化
    rc = libusb_init (NULL);
        if (rc < 0)
        {
            qDebug("failed to initialise libusb: %s\n", libusb_error_name(rc));
            return EXIT_FAILURE;
        }
        qDebug() << "JZJ";
        //判断当前的的库，是否支持热插拔
        if (!libusb_has_capability (LIBUSB_CAP_HAS_HOTPLUG)) {
            printf ("Hotplug capabilites are not supported on this platform\n");
            libusb_exit (NULL);
            return EXIT_FAILURE;
        }



        rc =  libusb_hotplug_register_callback(NULL, event_ARRIVED, flag, vendor_id, product_id, class_id, hotplug_callback, &user_data, &handle_Arrive);

        if (LIBUSB_SUCCESS != rc) {
            //fprintf (stderr, "Error registering callback 0\n");
            qDebug() << "Error registering callback 0\n";
            libusb_exit (NULL);
            return EXIT_FAILURE;
        }

        rc = libusb_hotplug_register_callback(NULL, event_LEFT, flag, vendor_id,product_id,class_id, hotplug_callback_detach, &user_data, &handle_Left);

        if (LIBUSB_SUCCESS != rc) {
            //fprintf (stderr, "Error registering callback 1\n");
            qDebug() << "Error registering callback 1\n";
            libusb_exit (NULL);
            return EXIT_FAILURE;
        }


        while(1)
        {
           libusb_handle_events (NULL);
        }

        //注销回调事件
        libusb_hotplug_deregister_callback(NULL, hp[0]);
        libusb_hotplug_deregister_callback(NULL, hp[1]);
        libusb_exit(NULL);

}




int LIBUSB_CALL CDetachDeviceThread::hotplug_callback(struct libusb_context *ctx,struct libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
    struct libusb_device_descriptor desc;
    int rc;

    (void)ctx;
    (void)dev;
    (void)event;
    (void)user_data;

    rc = libusb_get_device_descriptor(dev, &desc);
    if (LIBUSB_SUCCESS != rc) {
        //fprintf (stderr, "Error getting device descriptor\n");
        qDebug() << "Error getting device descriptor\n";
    }

    qDebug("Device attached: %04x:%04x\n", desc.idVendor, desc.idProduct);



    return 0;
}

int LIBUSB_CALL CDetachDeviceThread::hotplug_callback_detach(struct libusb_context *ctx,struct libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
    struct libusb_device_descriptor desc;
    (void)ctx;
    (void)dev;
    (void)event;
    (void)user_data;


    libusb_get_device_descriptor(dev, &desc);

    printf ("Device detached: %04x:%04x\n", desc.idVendor, desc.idProduct);


    return 0;
}




int CDetachDeviceThread::uuu_for_each_devices()
{
    if (libusb_init(NULL) < 0)
    {
        //set_last_err_string("Call libusb_init failure");
        qDebug() << "Call libusb_init failure";
        return -1;
    }

    libusb_device **newlist = NULL;
    libusb_get_device_list(NULL, &newlist);
    size_t i = 0;
    libusb_device *dev;


    while ((dev = newlist[i++]) != NULL)
    {
        struct libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) {
            //set_last_err_string("failure get device descrior");
            qDebug() << "failure get device descrior";
            return -1;
        }
        //qDebug() << desc.idVendor << desc.idProduct << desc.bcdDevice;
        qDebug("VID=0x%x,PID=0x%x,bcdDevice=%d",desc.idVendor,desc.idProduct,desc.bcdDevice);
        //string str = get_device_path(dev);

   }

    libusb_free_device_list(newlist, 1);
    libusb_exit(NULL);

    return 0;
}
