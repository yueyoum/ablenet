# -*- coding: utf-8 -*-
"""
Author:        Wang Chao <yueyoum@gmail.com>
Filename:      c.py
Date created:  2015-11-14 18:12:49
Description:

"""
import sys
import time
import socket

NAME = sys.argv[1]

s = socket.socket()
print "start connect"
s.connect(("127.0.0.1", 9090))
print "finish connect"

for i in range(10):
    data = "{0}-{1}".format(NAME, i+1)
    s.send(data)

    x = s.recv(160)
    print "GOT", x
    time.sleep(1)
