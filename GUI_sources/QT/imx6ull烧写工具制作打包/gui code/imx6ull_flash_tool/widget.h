#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>


// linux中usb检测方法，暂时没用，windows里面也可以用，但是不支持热插拔
#ifdef Q_OS_Linux
#include "CDetachDeviceThread.h"
#endif

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    #ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    #endif
private slots:
    void doProcessFinished();
    void CmdOutput();
    void doSelectBootFile();
    void doSelectTreeFile();
    void doSelectKernelFile();
    void doSelectRootfsFile();
    void doWriteUbootFile();
    void doWriteTreeFile();
    void doWriteKernelFile();
    void doWriteRootfsFile();
    void doWriteAllFile();
    void doWriteSingleFile();
    void doSelectSingleFile();
    void doWriteUserSpaceFile();
    void doSelectUserSpaceFile();
private:
    Ui::Widget *ui;

    QString strWindowTitle;

    QProcess *m_Process;
    void doDeleteTempFile();
    int uuu_for_each_devices();


    #ifdef Q_OS_WIN
    void registerDevice();
    #endif
};

#endif // WIDGET_H
