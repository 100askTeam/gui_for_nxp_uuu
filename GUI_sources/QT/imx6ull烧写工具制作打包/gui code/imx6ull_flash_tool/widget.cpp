#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "libusb.h"
#include <Windows.h>
#include <dbt.h>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    strWindowTitle = "IMX6ULL Flash Tool V1.0---based 100ask edit by jiangzhijie";

    setWindowTitle(tr("%1---[USB:%2]").arg(strWindowTitle).arg("未连接"));

    ui->cBoxDevice->addItem(tr("EMMC"));
    ui->cBoxDevice->addItem(tr("SD/TF"));

    m_Process = new QProcess(this);
    connect(m_Process,SIGNAL(finished(int)),this,SLOT(doProcessFinished()));
    connect(m_Process,SIGNAL(readyReadStandardOutput()),this,SLOT(CmdOutput()));

    connect(ui->tbtnSelectUboot,SIGNAL(clicked(bool)),this,SLOT(doSelectBootFile()));
    connect(ui->tBtnSelectTree,SIGNAL(clicked(bool)),this,SLOT(doSelectTreeFile()));
    connect(ui->tBtnSelectKernel,SIGNAL(clicked(bool)),this,SLOT(doSelectKernelFile()));
    connect(ui->tBtnSelectRootfs,SIGNAL(clicked(bool)),this,SLOT(doSelectRootfsFile()));
    connect(ui->tBtnSelectSingle,SIGNAL(clicked(bool)),this,SLOT(doSelectSingleFile()));
    connect(ui->tBtnSelectUser,SIGNAL(clicked(bool)),this,SLOT(doSelectUserSpaceFile()));

    connect(ui->tBtnWriteUboot,SIGNAL(clicked(bool)),this,SLOT(doWriteUbootFile()));
    connect(ui->tBtnWriteTree,SIGNAL(clicked(bool)),this,SLOT(doWriteTreeFile()));
    connect(ui->tBtnWriteKernel,SIGNAL(clicked(bool)),this,SLOT(doWriteKernelFile()));
    connect(ui->tBtnWriteRootfs,SIGNAL(clicked(bool)),this,SLOT(doWriteRootfsFile()));
    connect(ui->tBtnWriteAll,SIGNAL(clicked(bool)),this,SLOT(doWriteAllFile()));
    connect(ui->tBtnWriteSingle,SIGNAL(clicked(bool)),this,SLOT(doWriteSingleFile()));
    connect(ui->tBtnWriteUser,SIGNAL(clicked(bool)),this,SLOT(doWriteUserSpaceFile()));


#ifdef Q_OS_WIN
    registerDevice();
#endif



}

Widget::~Widget()
{

    if(m_Process->Running)
    {
        m_Process->close();
        m_Process->deleteLater();
    }
    doDeleteTempFile();

    delete ui;
}


void Widget::doProcessFinished()
{
    doDeleteTempFile();
    ui->textEdit->append("----Execution completed, Please set to EMMC mode and restart the board.----");
}


void Widget::doDeleteTempFile()
{
     QStringList filepath;
     QFile file;
     filepath << "./scripts/emmc/write_boot_Temp.clst"
              << "./scripts/emmc/write_dtb_Temp.clst"
              << "./scripts/emmc/write_kernel_Temp.clst"
              << "./scripts/emmc/write_rootfs_Temp.clst"
              << "./scripts/emmc/write_user_file_Temp.clst"
              << "./scripts/emmc/write_all_Temp.clst"
              << "./scripts/sd/write_boot_Temp.clst"
              << "./scripts/sd/write_dtb_Temp.clst"
              << "./scripts/sd/write_kernel_Temp.clst"
              << "./scripts/sd/write_rootfs_Temp.clst"
              << "./scripts/sd/write_user_file_Temp.clst"
              << "./scripts/sd/write_all_Temp.clst";
     for(int i=0; i< filepath.count();i++)
     {
         file.setFileName(filepath[i]);
         if(file.exists() == true) file.remove();
     }
}




void Widget::CmdOutput()
{
   QByteArray abt = m_Process->readAllStandardOutput();
   QString strMsg = QString::fromLocal8Bit(abt);
   ui->textEdit->append(abt);
}


void Widget::doSelectBootFile()
{

    QString file_name = QFileDialog::getOpenFileName(NULL,"File",".","*.imx");
    ui->lineEdit_Uboot->setText(file_name);
}

void Widget::doSelectSingleFile()
{
    QString file_name = QFileDialog::getOpenFileName(NULL,"File",".","*.imx");
    ui->lineEdit_Single->setText(file_name);
}

void Widget::doSelectUserSpaceFile()
{
    QString file_name = QFileDialog::getOpenFileName(NULL,"File",".","*.*");
    ui->lineEdit_User->setText(file_name);
}

void Widget::doSelectTreeFile()
{
    QString file_name = QFileDialog::getOpenFileName(NULL,"File",".","*.dtb");
    ui->lineEdit_Tree->setText(file_name);
}

void Widget::doSelectKernelFile()
{
    QString file_name = QFileDialog::getOpenFileName(NULL,"File",".","*.*");
    ui->lineEdit_Kernel->setText(file_name);
}


void Widget::doSelectRootfsFile()
{
    QString file_name = QFileDialog::getOpenFileName(NULL,"File",".","*.*");
    ui->lineEdit_Rootfs->setText(file_name);
}



void Widget::doWriteUbootFile()
{
    if(ui->lineEdit_Uboot->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个Uboot文件!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_boot.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_boot_Temp.clst").arg(strDevicePath);

    // 读取脚本文件
    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_file","@"+ui->lineEdit_Uboot->text());
    file.write(strScript.toUtf8());
    file.close();

    QString strCmd = tr("%1 %2").arg("uuu").arg(strConfigTempFileName);
    m_Process->start(strCmd);

}





void Widget::doWriteTreeFile()
{
    if(ui->lineEdit_Tree->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个设备树文件!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_dtb.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_dtb_Temp.clst").arg(strDevicePath);


    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_file","@"+ui->lineEdit_Tree->text());
    QFileInfo fileTemp(ui->lineEdit_Tree->text());
    strScript = strScript.replace("_name",fileTemp.fileName());
    file.write(strScript.toUtf8());
    file.close();

    QString strCmd = tr("%1 %2").arg("uuu").arg(strConfigTempFileName);
    m_Process->start(strCmd);
}




void Widget::doWriteKernelFile()
{
    if(ui->lineEdit_Kernel->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个内核文件!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_kernel.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_kernel_Temp.clst").arg(strDevicePath);


    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_file","@"+ui->lineEdit_Kernel->text());
    QFileInfo fileTemp(ui->lineEdit_Kernel->text());
    strScript = strScript.replace("_name",fileTemp.fileName());
    file.write(strScript.toUtf8());
    file.close();


    QString strCmd = tr("%1 %2").arg("uuu").arg(strConfigTempFileName);
    m_Process->start(strCmd);
}




void Widget::doWriteRootfsFile()
{
    if(ui->lineEdit_Rootfs->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个文件系统!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_rootfs.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_rootfs_Temp.clst").arg(strDevicePath);


    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_file","@"+ui->lineEdit_Rootfs->text());
    file.write(strScript.toUtf8());
    file.close();


    QString strCmd = tr("%1 %2").arg("uuu").arg(strConfigTempFileName);
    m_Process->start(strCmd);
}





void Widget::doWriteAllFile()
{
    if(ui->lineEdit_Uboot->text() == "" ||
       ui->lineEdit_Tree->text() == ""  ||
       ui->lineEdit_Kernel->text() == ""||
       ui->lineEdit_Rootfs->text() == "")
    {
        QMessageBox::critical(this,"错误","Uboot,设备树,内核和根文件系统都不能为空!");
        return;
    }



    if(m_Process->Running) m_Process->close();
    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_all.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_all_Temp.clst").arg(strDevicePath);


    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }

    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_filerootfs","@"+ui->lineEdit_Rootfs->text());
    strScript = strScript.replace("_fileuboot","@"+ui->lineEdit_Uboot->text());

    strScript = strScript.replace("_filekernel","@"+ui->lineEdit_Kernel->text());
    QFileInfo fileTemp(ui->lineEdit_Kernel->text());
    strScript = strScript.replace("_kernelname",fileTemp.fileName());

    strScript = strScript.replace("_filetree","@"+ui->lineEdit_Tree->text());
    QFileInfo fileTemp1(ui->lineEdit_Tree->text());
    strScript = strScript.replace("_treename",fileTemp1.fileName());

    file.write(strScript.toUtf8());
    file.close();


    QString strCmd = tr("%1 %2").arg("uuu").arg(strConfigTempFileName);
    m_Process->start(strCmd);
}




void Widget::doWriteSingleFile()
{
    if(ui->lineEdit_Single->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个裸机文件!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_boot.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_boot_Temp.clst").arg(strDevicePath);



    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_file","@"+ui->lineEdit_Single->text());
    file.write(strScript.toUtf8());
    file.close();


    QString strCmd = tr("%1 %2").arg("uuu").arg(strConfigTempFileName);
    m_Process->start(strCmd);
}





void Widget::doWriteUserSpaceFile()
{
    if(ui->lineEdit_User->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个用户文件!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_user_file.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_user_file_Temp.clst").arg(strDevicePath);


    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_file","@"+ui->lineEdit_User->text());
    QFileInfo fileTemp(ui->lineEdit_User->text());
    strScript = strScript.replace("_name",fileTemp.fileName());
    file.write(strScript.toUtf8());
    file.close();



    QString strCmd = tr("%1 %2").arg("uuu").arg(strConfigTempFileName);
    m_Process->start(strCmd);
}




#ifdef Q_OS_WIN
void Widget::registerDevice()
{
    const GUID GUID_DEVINTERFACE_LIST[] = {
        { 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
        { 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } }};


    DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
    ZeroMemory(&NotifacationFiler,sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    for(int i=0;i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID);i++)
    {
        NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
        RegisterDeviceNotification((HANDLE)this->winId(),&NotifacationFiler,DEVICE_NOTIFY_WINDOW_HANDLE);
    }
}
#endif



#ifdef Q_OS_WIN
bool Widget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    MSG* msg = reinterpret_cast<MSG*>(message);//第一层解算
    UINT msgType = msg->message;
    if(msgType==WM_DEVICECHANGE)
    {
        PDEV_BROADCAST_HDR lpdb = PDEV_BROADCAST_HDR(msg->lParam);//第二层解算
        switch (msg->wParam) {
        case DBT_DEVICEARRIVAL:
            if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
            {
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);


                //QString device_name = "插入设备(name)："+QString::fromWCharArray(pDevInf->dbcc_name,int(pDevInf->dbcc_size)).toUtf8();
                //qDebug()<< device_name;



                QString strname = QString::fromWCharArray(pDevInf->dbcc_name);
                if (!strname.isEmpty()) {
                    if (strname.contains("USBSTOR")) {
                    }else {

                        QStringList listAll = strname.split('#');
                        QStringList listID = listAll.at(1).split('&');
                        QString strvid = listID.at(0).right(4);
                        QString strpid = listID.at(1).right(4);

                        setWindowTitle(tr("%1---[USB:已连接,PID:%2,VID:%3]").arg(strWindowTitle).arg(strpid).arg(strvid));
                    }
                }
            }
            break;
        case DBT_DEVICEREMOVECOMPLETE:
            if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
            {
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);

                //QString device_name = "移除设备(name)："+QString::fromWCharArray(pDevInf->dbcc_name,int(pDevInf->dbcc_size)).toUtf8();
                //qDebug()<< device_name;


                QString strname = QString::fromWCharArray(pDevInf->dbcc_name);
                if (!strname.isEmpty()) {
                    if (strname.contains("USBSTOR")) {
                    }else {
                        QStringList listAll = strname.split('#');
                        QStringList listID = listAll.at(1).split('&');

                        QString strvid = listID.at(0).right(4);
                        QString strpid = listID.at(1).right(4);
                        setWindowTitle(tr("%1---[USB:%2]").arg(strWindowTitle).arg("未连接"));
                    }
                }
            }
            break;
        }
    }
    return false;
}
#endif

