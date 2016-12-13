#!/usr/bin/python2.7

from __future__ import print_function

import serial
import time
import sys

reopen_cnt = 0

read_bytes = 1

pen_step = 0.0625

if (len(sys.argv)<2):
    port = "COM5"
else:
    port = sys.argv[1]

while True:
    try:
        with serial.Serial(port, 921600) as ser:
            pen_x = 0.0
            pen_y = 0.0
            pen_z = 0.0
            reopen_cnt = 0
            while True:
                if (ser.in_waiting >= read_bytes):
                    data = ser.read(read_bytes)
                    val = ord(data[0])
                    val -= 33               # magic value added in arduino
                    x_st = (val>>0)&0x01
                    y_st = (val>>1)&0x01
                    z_st = (val>>2)&0x01
                    x_dir = (val>>3)&0x01
                    y_dir = (val>>4)&0x01
                    z_dir = (val>>5)&0x01
                    if (x_st):
                        if (x_dir):
                            pen_x += pen_step
                        else:
                            pen_x -= pen_step
                    if (y_st):
                        if (y_dir):
                            pen_y += pen_step
                        else:
                            pen_y -= pen_step
                    if (z_st):
                        if (z_dir):
                            pen_z += pen_step
                        else:
                            pen_z -= pen_step

                    print(pen_x, pen_y, pen_z)
                    #print(" x", x_st, x_dir, end='')
                    #print(" y", y_st, y_dir, end='')
                    #print(" z", z_st, z_dir, end='')
                    #print("")
                    sys.stdout.flush()
            ser.close()
            print("Receive done")

    except serial.SerialException:
        if (reopen_cnt == 0):
            print("COM port disconnected")
        reopen_cnt += 1
        if (reopen_cnt >= 6):
            print(".", end='')
            reopen_cnt = 1
        time.sleep(0.1)
    except :
        ser.close()
        raise
