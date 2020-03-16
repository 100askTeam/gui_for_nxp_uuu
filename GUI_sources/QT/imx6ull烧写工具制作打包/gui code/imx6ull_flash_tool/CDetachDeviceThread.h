#ifndef CDETACHDEVICETHREAD_H
#define CDETACHDEVICETHREAD_H

#include <QThread>
#include "libusb.h"


class CDetachDeviceThread : public QThread
{
    Q_OBJECT

public:
    explicit CDetachDeviceThread(QObject *parent = nullptr);
    ~CDetachDeviceThread();
protected:
    void run();

private:
    static int LIBUSB_CALL hotplug_callback(struct libusb_context *ctx,  struct libusb_device *dev, libusb_hotplug_event event, void *user_data);
    static int LIBUSB_CALL hotplug_callback_detach(struct libusb_context *ctx, struct libusb_device *dev, libusb_hotplug_event event, void *user_data);
    int DetchDeviceInit();

    int uuu_for_each_devices();
signals:

public slots:
};

#endif // CDETACHDEVICETHREAD_H
