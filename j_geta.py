# -*- coding: utf-8 -*-
"""
Created on Tue Jul 21 16:56:51 2015

@author: xialulee
"""

from Tkinter import *
from ttk import *
import json
import socket
import threading
import win32clipboard as clipboard
import subprocess as sp

import j_ip

class ServerThread(threading.Thread):
    def __init__(self, serverData):
        threading.Thread.__init__(self)
        sockobj = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sockobj.bind(('', int(serverData['port widget'].get())))
        self.__sockobj  = sockobj
        self.__serverData = serverData

    def run(self):
        while True:
            data, addr  = self.__sockobj.recvfrom(32 * 1024)
            pwd         = self.__serverData['password widget'].get()
            data        = json.loads(data.decode('utf-8'))
            if data['password'] == pwd:
                text    = data['message']
                print 'text from {}'.format(addr[0])
                print text
                print
                clipboard.OpenClipboard(0)
                clipboard.EmptyClipboard()
                clipboard.SetClipboardData(clipboard.CF_UNICODETEXT, text)
                clipboard.CloseClipboard()
            elif addr[0] == '127.0.0.1':
                if data['message'] == 'exit':
                    print 'Server Stopped.'
                    break

class MainWindow(Frame):
    def __init__(self, *args, **kwargs):
        Frame.__init__(self, *args, **kwargs)
        Label(self, text='Server Port:').pack(side=TOP, expand=YES, fill=X)
        entPort = Entry(self)
        entPort.pack(side=TOP, expand=YES, fill=Y)
        self.__entPort    = entPort

        Label(self, text='Password:').pack(side=TOP, expand=YES, fill=X)
        entPwd  = Entry(self)
        entPwd.pack(side=TOP, expand=YES, fill=Y)
        self.__entPwd     = entPwd

        self.__serverData = {'password widget':entPwd, 'port widget':entPort}

        btnStart    = Button(self, text='Start Server', command=self.onStart)
        btnStart.pack(side=TOP, expand=YES, fill=X)
        self.__btnStart     = btnStart
        btnStop     = Button(self, text='Stop Server', command=self.onStop)
        btnStop.pack(side=TOP, expand=YES, fill=X)
        self.__btnStop      = btnStop

        btnQR       = Button(self, text='QR Code', command=self.onQRCode)
        btnQR.pack(side=TOP, expand=YES, fill=X)

        self.__serverThread = None

    def onStart(self):
        if self.__serverThread is None:
            self.__serverThread     = ServerThread(self.__serverData)
        print 'Start Server'
        print
        self.__btnStart['state']    = DISABLED
        self.__serverThread.start()

    def onStop(self):
        sockobj = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sockobj.sendto('{"password":"", "message":"exit"}', ('127.0.0.1', int(self.__entPort.get())))
        self.__serverThread     = None
        self.__btnStart['state']    = ACTIVE

    def onQRCode(self):
        p = sp.Popen('C:\Program Files (x86)\IronPython 2.7\ipy.exe barcode.py --stdin --display --width=400 --height=400', stdin=sp.PIPE)
        # selfIP = socket.gethostbyname(socket.gethostname())//得到本地IP
        selfIP=j_ip.GetLocalIP()#得到被
        data = {'server ip':selfIP, 'server port':int(self.__entPort.get()), 'password':self.__entPwd.get()}
        data = json.dumps(data)
        print data
        p.stdin.write(data)
        p.stdin.close()

# if __name__ == '__main__':
#     root = Tk()
#     mainFrm = MainWindow(root)
#     mainFrm.pack(expand=YES, fill=BOTH)
#     root.mainloop()
if __name__ == '__main__':
    root = Tk()
    mainFrm = MainWindow(root)
    mainFrm.pack(expand=YES, fill=BOTH)
    root.mainloop()
