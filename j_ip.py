# -*- coding: utf-8 -*-
# 
import re,urllib2
from subprocess import Popen, PIPE

def GetLocalIP():
    LocalIP=re.search('192.168.\d+\.\d+',Popen('ipconfig', stdout=PIPE).stdout.read()).group(0)
    return LocalIP


if __name__ == '__main__':
    print "本机的私网IP地址为：" + GetLocalIP()