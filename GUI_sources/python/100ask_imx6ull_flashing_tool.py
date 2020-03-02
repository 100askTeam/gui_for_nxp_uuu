import os
import re
import sys
import uuid
import base64
import ctypes
import socket
import platform
import subprocess
import webbrowser
import urllib.request
from json import load
from time import sleep
from icon import img
from picture import *
from threading import Thread
from shutil import copy, rmtree
import tkinter as tk
from tkinter import *
from tkinter import ttk
from tkinter import Menu
import tkinter.filedialog
from tkinter import scrolledtext


class APPData:
    def __init__(self):
        self.Version = None
		
        self.Tab1IMXFile = None
        self.Tab1IMXFilePath = None
        self.Tab1UploadFile = None
        self.Tab1UploadFilePath = None

        self.Tab2FWFile = None
        self.Tab2FWFilePath = None
        self.Tab2IMXFile = None
        self.Tab2IMXFilePath = None
        self.Tab2UploadFile = None
        self.Tab2UploadFilePath = None

        self.LEDFlag = None

def TKgetPicture(name):
    tmp = open("tmp.png", "wb+")
    tmp.write(base64.b64decode(name))
    tmp.close()
    pic = tk.PhotoImage(file='tmp.png')
    os.remove("tmp.png")
    return pic

def ThreadLEDMonitor(aDate):
    global DeviceLEDOff, DeviceLEDOn, FWLEDOff, FWLEDOn
    global Tab1Label1, Tab1Label2, Tab2Label1, Tab2Label2
    global Tab2RunButton

    cmd = ".\\bin\\uuu.exe -lsusb"
    while True:
        # print(cmd)
        p = subprocess.Popen(cmd, shell=True,
                             stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        while True:
            result = p.stdout.readline()
            if result != b'':
                if (result.decode('gbk').find("FB") >= 0):
                    aDate.LEDFlag = 2
                    break
                elif (result.decode('gbk').find("SDP") >= 0):
                    aDate.LEDFlag = 1
                    break
                else:
                    aDate.LEDFlag = 0
            else:
                break

        if (aDate.LEDFlag == 2):
            Tab1Label1.config(image=DeviceLEDOn)
            Tab1Label2.config(image=FWLEDOn)
            Tab2Label1.config(image=DeviceLEDOn)
            Tab2Label2.config(image=FWLEDOn)

            Tab2RunButton.config(state="disabled")

        elif (aDate.LEDFlag == 1):
            Tab1Label1.config(image=DeviceLEDOn)
            Tab1Label2.config(image=FWLEDOff)
            Tab2Label1.config(image=DeviceLEDOn)
            Tab2Label2.config(image=FWLEDOff)

            Tab2RunButton.config(state="normal")
        else:
            Tab1Label1.config(image=DeviceLEDOff)
            Tab1Label2.config(image=FWLEDOff)
            Tab2Label1.config(image=DeviceLEDOff)
            Tab2Label2.config(image=FWLEDOff)

            Tab2RunButton.config(state="disabled")

        global Tab2ComboxList3, Tab2ComboxList4
        type = Tab2ComboxList3.get()
        if (type == "FAT"):
            scriptsList4 = ["/"]
            Tab2ComboxList4["values"] = scriptsList4
            Tab2ComboxList4.current(0)
        else:
            Tab2ComboxList4.config(state="normal")
            scriptsList4 = ["/", "/boot/", "自定义"]
            Tab2ComboxList4["values"] = scriptsList4
        sleep(1)

def Tab1MessageTXTPrint(type,path, str1, str2):
    global Tab1MessageTXT
    if ((os.path.splitext(path)[1] != type) and
            (os.path.splitext(path)[1] != type.upper())):
        Tab1MessageTXT.insert(END, str1)
        Tab1MessageTXT.see(END)
    else:
        Tab1MessageTXT.delete(0.0, END)
        Tab1MessageTXT.insert(END, str2)
        Tab1MessageTXT.see(END)

def Tab2MessageTXTPrint(type,path, str1, str2):
    global Tab2MessageTXT
    if ((os.path.splitext(path)[1] != type) and
            (os.path.splitext(path)[1] != type.upper())):
        Tab2MessageTXT.insert(END, str1)
        Tab2MessageTXT.see(END)
    else:
        Tab2MessageTXT.delete(0.0, END)
        Tab2MessageTXT.insert(END, str2)
        Tab2MessageTXT.see(END)

def getFWFile(aDate):
    aDate.Tab2FWFilePath = tkinter.filedialog.askopenfilename()
    FileTempList = aDate.Tab2FWFilePath.split("/")
    Num = len(FileTempList) - 1
    aDate.Tab2FWFile = FileTempList[Num]

    global Tab2FWFile
    Tab2FWFile.delete(0.0, END)
    Tab2FWFile.insert(END, aDate.Tab2FWFile)

    type = ".imx"
    str1 = "文件类型选择错误，请重新选择IMX文件！\n"
    str2 = "当前选择：" + aDate.Tab2FWFilePath + "\n"
    Tab2MessageTXTPrint(type,aDate.Tab2FWFilePath, str1, str2)

def getTab2IMXFile(aDate):
    aDate.Tab2IMXFilePath = tkinter.filedialog.askopenfilename()
    FileTempList = aDate.Tab2IMXFilePath.split("/")
    Num = len(FileTempList) - 1
    aDate.Tab2IMXFile = FileTempList[Num]

    global Tab2IMXFile
    Tab2IMXFile.delete(0.0, END)
    Tab2IMXFile.insert(END, aDate.Tab2IMXFile)

    type = ".imx"
    str1 = "文件类型选择错误，请重新选择IMX文件！\n"
    str2 = "当前选择：" + aDate.Tab2IMXFilePath + "\n"
    Tab2MessageTXTPrint(type, aDate.Tab2IMXFilePath, str1, str2)

def getTab2IMGFile():
    aDate.Tab2IMGFilePath = tkinter.filedialog.askopenfilename()
    FileTempList = aDate.Tab2IMGFilePath.split("/")
    Num = len(FileTempList) - 1
    aDate.Tab2IMGFile = FileTempList[Num]

    global Tab2IMGFile
    Tab2IMGFile.delete(0.0, END)
    Tab2IMGFile.insert(END, aDate.Tab2IMGFile)

    type = ".img"
    str1 = "文件类型选择错误，请重新选择IMG文件！\n"
    str2 = "当前选择：" + aDate.Tab2IMGFilePath + "\n"
    Tab2MessageTXTPrint(type,aDate.Tab2IMGFilePath,  str1, str2)

def getTab2UploadFile(aDate):
    aDate.Tab2UploadFilePath = tkinter.filedialog.askopenfilename()
    FileTempList = aDate.Tab2UploadFilePath.split("/")
    Num = len(FileTempList) - 1
    aDate.Tab2UploadFile = FileTempList[Num]

    global Tab2UploadFile
    Tab2UploadFile.delete(0.0, END)
    Tab2UploadFile.insert(END, aDate.Tab2UploadFile)

    global Tab2MessageTXT
    Tab2MessageTXT.delete(0.0, END)
    Tab2MessageTXT.insert(END, "当前选择：" + aDate.Tab2UploadFilePath + "\n")
    Tab2MessageTXT.see(END)


def getTab1IMXFile(aDate):
    aDate.Tab1IMXFilePath = tkinter.filedialog.askopenfilename()
    FileTempList = aDate.Tab1IMXFilePath.split("/")
    Num = len(FileTempList) - 1
    aDate.Tab1IMXFile = FileTempList[Num]

    global Tab1IMXFile
    Tab1IMXFile.delete(0.0, END)
    Tab1IMXFile.insert(END, aDate.Tab1IMXFile)

    type = ".imx"
    str1 = "文件类型选择错误，请重新选择IMX文件！\n"
    str2 = "当前选择：" + aDate.Tab1IMXFilePath + "\n"
    Tab1MessageTXTPrint(type, aDate.Tab1IMXFilePath, str1, str2)

def getTab1UploadFile(aDate):
    aDate.Tab1UploadFilePath = tkinter.filedialog.askopenfilename()
    FileTempList = aDate.Tab1UploadFilePath.split("/")
    Num = len(FileTempList) - 1
    aDate.Tab1UploadFile = FileTempList[Num]

    global Tab1UploadFile
    Tab1UploadFile.delete(0.0, END)
    Tab1UploadFile.insert(END, aDate.Tab1UploadFile)

    global Tab1MessageTXT
    Tab1MessageTXT.delete(0.0, END)
    Tab1MessageTXT.insert(END, "当前选择：" + aDate.Tab1UploadFilePath + "\n")
    Tab1MessageTXT.see(END)

def CMDAndPrintTab1(cmd, str):
    print(cmd)
    p = subprocess.Popen(cmd, shell=True,
                         stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    while True:
        result = p.stdout.readline()
        if result != b'':
            Tab1MessageTXT.insert(END, result.decode('gbk'))
            Tab1MessageTXT.see(END)
            Tab1MessageTXT.update_idletasks()
        else:
            Tab1MessageTXT.insert(END, str)
            Tab1MessageTXT.update_idletasks()
            break

def CMDAndPrintTab2(cmd, str):
    print(cmd)
    p = subprocess.Popen(cmd, shell=True,
                         stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    while True:
        result = p.stdout.readline()
        if result != b'':
            Tab2MessageTXT.insert(END, result.decode('gbk'))
            Tab2MessageTXT.see(END)
            Tab2MessageTXT.update_idletasks()
        else:
            Tab2MessageTXT.insert(END, str)
            Tab2MessageTXT.update_idletasks()
            break

def RunTab2FW(aDate):
    if aDate.Tab2FWFilePath == None:
        aDate.Tab2FWFilePath = "./firmware/u-boot-dtb_fastboot_100ask.imx"
    cmd = '.\\bin\\uuu.exe -v -t 1 ' + aDate.Tab2FWFilePath
    str = "-----Execution completed.----\n"
    CMDAndPrintTab2(cmd, str)

def repairScript(scriptPath, filePath, key, num):
    file_data = ""
    temp = ""
    with open(scriptPath, "r") as f:
        for line in f:
            if key in line:
                line = re.sub(' +', ' ', line)
                tempList = line.split(" ")
                for i in range(0, num):
                    temp = temp + tempList[i] + " "
                temp = temp + ">" + filePath + "\n"
                line = temp
            file_data += line
    with open(scriptPath,"w") as f:
        f.write(file_data)

def FlashingTab2IMX(aDate):
    if aDate.LEDFlag < 2:
        RunTab2FW(aDate)

    global Tab2ComboxList1
    if Tab2ComboxList1.get() == "SD/TF":
        device = "sd"
    else:
        device = Tab2ComboxList1.get().lower()

    scriptPath = '.\\scripts\\pro\\' + device + '\\write_boot.clst'
    repairScript(scriptPath, aDate.Tab2IMXFilePath, "FB: flash bootloader", 3)

    cmd = '.\\bin\\uuu.exe ' + scriptPath
    str = "-----Execution completed.----\n"
    CMDAndPrintTab2(cmd, str)

def FlashingTab2IMG(aDate):
    if aDate.LEDFlag < 2:
        RunTab2FW(aDate)

    global Tab2ComboxList1
    if Tab2ComboxList1.get() == "SD/TF":
        device = "sd"
    else:
        device = Tab2ComboxList1.get().lower()

    scriptPath = '.\\scripts\\pro\\' + device + '\\write_all.clst'
    repairScript(scriptPath, aDate.Tab2IMGFilePath, "FB: flash -raw2sparse all", 4)

    cmd = '.\\bin\\uuu.exe ' + scriptPath
    str = "-----Execution completed.----\n"
    CMDAndPrintTab2(cmd, str)

def UploadTab2File(aDate):
    if aDate.LEDFlag < 2:
        RunTab2FW(aDate)

    cmd = '.\\bin\\uuu.exe FB: download -f ' + aDate.Tab2UploadFilePath
    str = "-----Download completed.----\n"
    CMDAndPrintTab2(cmd, str)

    global Tab2ComboxList1, Tab2ComboxList2, Tab2ComboxList3, Tab2ComboxList4

    if Tab2ComboxList1.get() == "SD/TF":
        device = "sd"
    else:
        device = Tab2ComboxList1.get().lower()
    flash = device.upper()
    partition = Tab2ComboxList2.get()
    type = Tab2ComboxList3.get()

    path = Tab2ComboxList4.get()
    pathList = path.split("/")
    if (pathList[len(pathList) - 1]) == "":
        path = path + aDate.Tab2UploadFile
    elif (pathList[len(pathList) - 1]) != aDate.Tab2UploadFile:
        path = path + "/" + aDate.Tab2UploadFile
    elif (pathList[len(pathList) - 1]) == aDate.Tab2UploadFile:
        path = path
    else:
        print("错误路径：" + path)

    cmd = '.\\bin\\uuu.exe FB: ucmd setenv SEL_DEV ' + flash
    str = " "
    CMDAndPrintTab2(cmd, str)

    cmd = '.\\bin\\uuu.exe FB: ucmd setenv PART ' + partition
    str = " "
    CMDAndPrintTab2(cmd, str)

    cmd = '.\\bin\\uuu.exe FB: ucmd setenv FSTYPE ' + type
    str = " "
    CMDAndPrintTab2(cmd, str)

    cmd = '.\\bin\\uuu.exe FB: ucmd setenv TARGET_FILE ' + path
    str = " "
    CMDAndPrintTab2(cmd, str)

    cmd = '.\\bin\\uuu.exe .\\scripts\\pro\\' + device + '\\write_user_file.clst'
    str = "-----Execution completed.----"
    CMDAndPrintTab2(cmd, str)

def RunBasic(aDate, ScriptFile):
    if aDate.LEDFlag == 1:
        FWFilePath = "./firmware/u-boot-dtb_fastboot_100ask.imx"
        cmd = '.\\bin\\uuu.exe -v -t 1 ' + FWFilePath
        str = " "
        CMDAndPrintTab1(cmd, str)

    global Tab1ComboxList
    if Tab1ComboxList.get() == "SD/TF":
        device = "sd"
    else:
        device = Tab1ComboxList.get().lower()
    cmd = '.\\bin\\uuu.exe -v .\\scripts\\basic\\' + device + '\\' + ScriptFile
    str = "-----Execution completed, Please set to "+ Tab1ComboxList.get() +" mode and restart the board.----\n"
    CMDAndPrintTab1(cmd, str)

def FlashingTab1IMX(aDate):
    if aDate.LEDFlag == 1:
        FWFilePath = "./firmware/u-boot-dtb_fastboot_100ask.imx"
        cmd = '.\\bin\\uuu.exe -v -t 1 ' + FWFilePath
        str = " "
        CMDAndPrintTab1(cmd, str)

    global Tab1ComboxList
    if Tab1ComboxList.get() == "SD/TF":
        device = "sd"
    else:
        device = Tab1ComboxList.get().lower()

    scriptPath = '.\\scripts\\basic\\' + device + '\\write_noos.clst'
    repairScript(scriptPath, aDate.Tab1IMXFilePath, "FB: flash bootloader", 3)

    cmd = '.\\bin\\uuu.exe ' + scriptPath
    str = "-----Execution completed, Please set to "+ Tab1ComboxList.get() +" mode and restart the board.----\n"
    CMDAndPrintTab1(cmd, str)

def UploadTab1File(aDate):
    if aDate.LEDFlag == 1:
        FWFilePath = "./firmware/u-boot-dtb_fastboot_100ask.imx"
        cmd = '.\\bin\\uuu.exe -v -t 1 ' + FWFilePath
        str = " "
        CMDAndPrintTab1(cmd, str)

    cmd = '.\\bin\\uuu.exe FB: download -f ' + aDate.Tab1UploadFilePath
    str = "-----Download completed.----\n"
    CMDAndPrintTab1(cmd, str)

    global Tab1ComboxList

    if Tab1ComboxList.get() == "SD/TF":
        device = "sd"
    else:
        device = Tab1ComboxList.get().lower()

    cmd = '.\\bin\\uuu.exe FB: ucmd setenv TARGET_FILE /' + aDate.Tab1UploadFile
    str = " "
    CMDAndPrintTab1(cmd, str)

    cmd = '.\\bin\\uuu.exe .\\scripts\\basic\\' + device + '\\write_user_file.clst'
    str = "-----Execution completed, Please set to " + Tab1ComboxList.get() + " mode and restart the board.----\n"
    CMDAndPrintTab1(cmd, str)


def ThreadButton(aDate, str):
    if (str == "Tab1AllButton"):
        thread1 = Thread(target=RunBasic, args=(aDate, "write_all.clst",))
    elif (str == "Tab1KernelButton"):
        thread1 = Thread(target=RunBasic, args=(aDate, "write_kernel.clst",))
    elif (str == "Tab1DTBButton"):
        thread1 = Thread(target=RunBasic, args=(aDate, "write_dtb.clst",))
    elif (str == "Tab1BootButton"):
        thread1 = Thread(target=RunBasic, args=(aDate, "write_boot.clst",))
    elif (str == "Tab1HardwareButton"):
        thread1 = Thread(target=FlashingTab1IMX, args=(aDate,))
    elif (str == "Tab1UploadButton"):
        thread1 = Thread(target=UploadTab1File, args=(aDate,))

    elif (str == "Tab2RunButton"):
        thread1 = Thread(target=RunTab2FW, args=(aDate,))
    elif (str == "Tab2HardwareButton"):
        thread1 = Thread(target=FlashingTab2IMX, args=(aDate,))
    elif (str == "Tab2AllButton"):
        thread1 = Thread(target=FlashingTab2IMG, args=(aDate,))
    elif (str == "Tab2UploadButton"):
        thread1 = Thread(target=UploadTab2File, args=(aDate,))

    thread1.start()


def creatMainGUI(aDate):
    win = tk.Tk()
    win.title("100ASK IMX6ULL Flashing Tool" + " V" + aDate.Version)

    tmp = open("tmp.ico", "wb+")
    tmp.write(base64.b64decode(img))
    tmp.close()
    win.iconbitmap("tmp.ico")
    os.remove("tmp.ico")

    win.geometry("1110x620")
    OSVersion = platform.platform().split("-")[1]
    if OSVersion == "10":
        ctypes.windll.shcore.SetProcessDpiAwareness(1)
    else:
        ctypes.windll.user32.SetProcessDPIAware(1)
    win.tk.call('tk', 'scaling', 125 / 75)
    win.resizable(False, False)

    tabControl = ttk.Notebook(win)
    tab1 = ttk.Frame(tabControl)
    tabControl.add(tab1, text='基础版')
    tab2 = ttk.Frame(tabControl)
    tabControl.add(tab2, text='专业版')
    tab3 = ttk.Frame(tabControl)
    tabControl.add(tab3, text='帮助')
    tabControl.pack(expand=1, fill="both")

    # =============== tab1:基础版 ===============
    Tab1cv1 = Canvas(tab1)
    Tab1cv1.create_rectangle(25, 70, 315, 245)
    Tab1cv2 = Canvas(tab1)
    Tab1cv2.create_rectangle(25, 5, 315, 95)
    Tab1cv3 = Canvas(tab1)
    Tab1cv3.create_rectangle(25, 5, 315, 95)

    global DeviceLEDOff, DeviceLEDOn, FWLEDOff, FWLEDOn
    global Tab1Label1, Tab1Label2

    DeviceLEDOff = TKgetPicture(led_off1)
    DeviceLEDOn = TKgetPicture(led_on1)
    Tab1Label1 = tkinter.Label(tab1, image=DeviceLEDOff)

    FWLEDOff = TKgetPicture(led_off2)
    FWLEDOn = TKgetPicture(led_on2)
    Tab1Label2 = tkinter.Label(tab1, image=FWLEDOff)

    Tab1Label3 = tkinter.Label(tab1, text="选择设备：")
    global Tab1ComboxList
    Tab1ComboxList = ttk.Combobox(tab1, state='readonly')
    scriptsPath = ".\\scripts\\basic\\"
    scriptsList = []
    for root, dirs, files in os.walk(scriptsPath):
        if dirs:
            scriptsList = dirs
    if scriptsList:
        for i in range(0, len(scriptsList)):
            if scriptsList[i] == "sd":
                scriptsList[i] = "sd/tf"
            scriptsList[i] = scriptsList[i].upper()
    else:
        scriptsList = ("EMMC", "SD/TF", "NAND")
    Tab1ComboxList["values"] = scriptsList
    Tab1ComboxList.current(0)

    Tab1AllButton = Button(tab1, text="烧写整个系统", command=lambda: ThreadButton(aDate, "Tab1AllButton"))
    Tab1KernelButton = Button(tab1, text="更新　内核　", command=lambda: ThreadButton(aDate, "Tab1KernelButton"))
    Tab1DTBButton = Button(tab1, text="更新　设备树", command=lambda: ThreadButton(aDate, "Tab1DTBButton"))
    Tab1BootButton = Button(tab1, text="更新　Uboot", command=lambda: ThreadButton(aDate, "Tab1BootButton"))

    global Tab1IMXFile
    Tab1IMXFile = tkinter.Text(tab1, width=18, height=1)
    Tab1SelectButton = Button(tab1, text="选择裸机文件", command=lambda: getTab1IMXFile(aDate))
    Tab1HardwareButton = Button(tab1, text="烧写裸机", command=lambda: ThreadButton(aDate, "Tab1HardwareButton"))

    global Tab1UploadFile
    Tab1UploadFile = tkinter.Text(tab1, width=18, height=1)
    Tab1ChooseButton = Button(tab1, text="选择用户文件", command=lambda: getTab1UploadFile(aDate))
    Tab1UploadButton = Button(tab1, text="上传文件", command=lambda: ThreadButton(aDate, "Tab1UploadButton"))

    Tab1Label4 = Label(tab1, text="开发板零利润   全网价最低   减配劣质?开源！ 欢迎高手评稿", fg="blue", cursor="hand2")
    Tab1Label4.bind("<Button-1>", lambda e: Webcallback("https://100ask.taobao.com/"))
    Tab1Label5 = Label(tab1, text="入门视频免费   答疑还到位   这不赚钱那免费  咋活？你猜！", fg="blue", cursor="hand2")
    Tab1Label5.bind("<Button-1>", lambda e: Webcallback("https://100ask.taobao.com/"))
    Tab1Label6 = tkinter.Label(tab1, text="Copyright © 2020 百问科技")

    global Tab1MessageTXT
    Tab1MessageTXT = scrolledtext.ScrolledText(tab1, width=81, height=27)

    Tab1cv1.place(x=0, y=20)  
    Tab1cv2.place(x=0, y=282)
    Tab1cv3.place(x=0, y=394)

    Tab1Label1.place(x=35, y=10) 
    Tab1Label2.place(x=180, y=10)

    Tab1Label3.place(x=25, y=50)  
    Tab1ComboxList.place(x=105, y=50)

    Tab1AllButton.place(x=110, y=100)  
    Tab1KernelButton.place(x=110, y=140)
    Tab1DTBButton.place(x=110, y=180)
    Tab1BootButton.place(x=110, y=220)

    Tab1IMXFile.place(x=138, y=302)  
    Tab1SelectButton.place(x=30, y=295)
    Tab1HardwareButton.place(x=125, y=335)

    Tab1UploadFile.place(x=138, y=415) 
    Tab1ChooseButton.place(x=30, y=408)
    Tab1UploadButton.place(x=125, y=448)

    Tab1Label4.place(x=348, y=505)  
    Tab1Label5.place(x=348, y=530)
    Tab1Label6.place(x=445, y=560)

    Tab1MessageTXT.place(x=345, y=30)  

    # =============== tab2:专业版 ===============
    Tab2cv1 = Canvas(tab2, highlightthickness=0, borderwidth=0)
    pic1 = TKgetPicture(background1)
    Tab2cv1.create_image(0, 0, image=pic1, anchor='nw')

    Tab2cv2 = Canvas(tab2, highlightthickness=0, borderwidth=0)
    pic2 = TKgetPicture(background2)
    Tab2cv2.create_image(0, 0, image=pic2, anchor='nw')

    Tab2cv3 = Canvas(tab2, highlightthickness=0, borderwidth=0)
    pic3 = TKgetPicture(background3)
    Tab2cv3.create_image(0, 0, image=pic3, anchor='nw')

    global Tab2Label1, Tab2Label2
    Tab2Label1 = tkinter.Label(tab2, image=DeviceLEDOff)
    Tab2Label2 = tkinter.Label(tab2, image=FWLEDOff)

    pic6 = TKgetPicture(run1)
    global Tab2FWFile, Tab2RunButton
    Tab2FWFile = tkinter.Text(tab2, width=20, height=1)
    Tab2FWFile.delete(0.0, END)
    Tab2FWFile.insert(END, "u-boot-dtb_fastboot_100ask.imx")
    Tab2SelectButton1 = Button(tab2, text="固件/裸机", command=lambda: getFWFile(aDate))
    Tab2RunButton = Button(tab2, text="运行", image=pic6, command=lambda: ThreadButton(aDate, "Tab2RunButton"))

    Tab2Label3 = tkinter.Label(tab2, text="选择设备：")
    global Tab2ComboxList1
    Tab2ComboxList1 = ttk.Combobox(tab2, width=18)
    scriptsPath = ".\\scripts\\pro\\"
    scriptsList = []
    for root, dirs, files in os.walk(scriptsPath):
        if dirs:
            scriptsList = dirs
    if scriptsList:
        for i in range(0, len(scriptsList)):
            if scriptsList[i] == "sd":
                scriptsList[i] = "sd/tf"
            scriptsList[i] = scriptsList[i].upper()
        scriptsList.append("自定义")
    else:
        scriptsList = ("EMMC", "SD/TF", "NAND", "自定义")
    Tab2ComboxList1["values"] = scriptsList
    Tab2ComboxList1.current(0)

    pic7 = TKgetPicture(download1)
    global Tab2IMXFile
    Tab2IMXFile = tkinter.Text(tab2, width=20, height=1)
    Tab2ChooseButton1 = Button(tab2, text="Boot/裸机", command=lambda: getTab2IMXFile(aDate))
    Tab2HardwareButton = Button(tab2, text="烧写裸机/Uboot", image=pic7,
                                command=lambda: ThreadButton(aDate, "Tab2HardwareButton"))
								
    pic8 = TKgetPicture(download2)
    global Tab2IMGFile
    Tab2IMGFile = tkinter.Text(tab2, width=20, height=1)
    Tab2ChooseButton2 = Button(tab2, text="映像文件", command=lambda: getTab2IMGFile())
    Tab2AllButton = Button(tab2, text="烧写整个系统", image=pic8,
                           command=lambda: ThreadButton(aDate, "Tab2AllButton"))

    global Tab2UploadFile
    Tab2UploadFile = tkinter.Text(tab2, width=20, height=1)
    Tab2ChooseButton3 = Button(tab2, text="任意文件", command=lambda: getTab2UploadFile(aDate))

    Tab2Label4 = tkinter.Label(tab2, text="选择分区：")
    global Tab2ComboxList2
    Tab2ComboxList2 = ttk.Combobox(tab2, width=18)
    scriptsList2 = ["1", "2", "自定义"]
    Tab2ComboxList2["values"] = scriptsList2
    Tab2ComboxList2.current(0)

    Tab2Label5 = tkinter.Label(tab2, text="分区格式：")
    global Tab2ComboxList3
    Tab2ComboxList3 = ttk.Combobox(tab2, width=18, state='readonly')
    scriptsList3 = ["FAT", "EXT4"]
    Tab2ComboxList3["values"] = scriptsList3
    Tab2ComboxList3.current(0)

    Tab2Label6 = tkinter.Label(tab2, text="目标路径：")
    global Tab2ComboxList4
    Tab2ComboxList4 = ttk.Combobox(tab2, width=18, state='readonly')
    scriptsList4 = ["/"]
    Tab2ComboxList4["values"] = scriptsList4
    Tab2ComboxList4.current(0)

    pic9 = TKgetPicture(upload1)
    Tab2UploadButton = Button(tab2, text="上传文件", image=pic9,
                              command=lambda: ThreadButton(aDate, "Tab2UploadButton"))

    global Tab2MessageTXT
    Tab2MessageTXT = scrolledtext.ScrolledText(tab2, width=81, height=31)

    Tab2cv1.place(x=20, y=42) 
    Tab2cv2.place(x=20, y=150)
    Tab2cv3.place(x=20, y=383)

    Tab2Label1.place(x=35, y=10) 
    Tab2Label2.place(x=180, y=10)

    Tab2SelectButton1.place(x=30, y=50)  
    Tab2FWFile.place(x=125, y=58)
    Tab2RunButton.place(x=131, y=95)

    Tab2Label3.place(x=40, y=160)  
    Tab2ComboxList1.place(x=123, y=160)

    Tab2ChooseButton1.place(x=30, y=200)  
    Tab2IMXFile.place(x=125, y=208)
    Tab2HardwareButton.place(x=90, y=243)

    Tab2ChooseButton2.place(x=38, y=297) 
    Tab2IMGFile.place(x=125, y=305)
    Tab2AllButton.place(x=95, y=342)

    Tab2ChooseButton3.place(x=38, y=395) 
    Tab2UploadFile.place(x=125, y=403)
    Tab2Label4.place(x=40, y=440)
    Tab2ComboxList2.place(x=123, y=440)
    Tab2Label5.place(x=40, y=470)
    Tab2ComboxList3.place(x=123, y=470)
    Tab2Label6.place(x=40, y=500)
    Tab2ComboxList4.place(x=123, y=500)
    Tab2UploadButton.place(x=115, y=533)

    Tab2MessageTXT.place(x=345, y=42)  

    Thread1 = Thread(target=ThreadLEDMonitor, args=(aDate,))
    Thread1.start()

    win.mainloop()


if __name__ == "__main__":
    aDate = APPData()
    aDate.Version = "2.0"
    aDate.LEDFlag = 0

    creatMainGUI(aDate)
