#!/usr/bin/env python
#
# Copyright 2015 Matthias Puech.
#
# Author: Matthias Puech (matthias.puech@gmail.com)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# 
# See http://creativecommons.org/licenses/MIT/ for more information.
#
# -----------------------------------------------------------------------------
#
# GUI Flash utility for STM32 projects, based on stm32loader

import sys
sys.path.insert(0, 'pyserial')
sys.path.insert(0, 'stm32loader')

import glob

from Tkinter import *
import tkMessageBox

from stm32loader import *

import serial

firmware_name = "batumi-0.41"

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def center(toplevel):
    toplevel.update_idletasks()
    w = toplevel.winfo_screenwidth()
    h = toplevel.winfo_screenheight()
    size = tuple(int(_) for _ in toplevel.geometry().split('+')[0].split('x'))
    x = w/2 - size[0]/2
    y = h/2 - size[1]/2
    toplevel.geometry("%dx%d+%d+%d" % (size + (x, y)))

class Application(Frame):

    def flash(self):
        cmd = CommandInterface()
        self.status.set("Opening port \""+self.port+"\"...")
        root.update()
        try:
            cmd.open(self.port, 115200)
            self.status.set("Initializing chip...")
            root.update()
            try:
                cmd.initChip()
            except:
                raise CmdException("Can't find the chip. Please check jumper position.")
            bootversion = cmd.cmdGet()
            id = cmd.cmdGetID()

            self.status.set("Erasing memory...")
            root.update()
            cmd.cmdEraseMemory()
            self.status.set("Writing firmware (may take a minute)...")
            root.update()
            cmd.writeMemory(0x08000000, self.data)
            self.status.set("Done.")
            self.done = 42;
            self.ok['state'] = 'normal'
            self.ok['text'] = 'Quit'
            self.instruct.set("Now you can put the jumper back in its original position, plug Batumi back in your case and have fun!")
            root.update()

        except CmdException as e:
            self.status.set("Error: "+str(e))
            self.start()
        finally:
            cmd.releaseChip()

    def ok(self):
        if not(hasattr(self, 'ports')):
            self.status.set("listing available devices...")
            self.ports = serial_ports()
            self.status.set("Ready.")
            self.instruct.set("Now plug Batumi to this computer via USB. It should not be connected to the Eurorack power. Make sure to set the jumper on the back to the Update position (left).")
        elif not(hasattr(self, 'port')):
            self.status.set("Detecting new devices...")
            newports = serial_ports()
            l = list(set(newports) - set(self.ports))
            if len(l) < 1:
                self.status.set("Error: No new device detected.")
                self.start()
                return
            elif len(l) > 1:
                self.status.set("Several new devices detected. Please connect one at a time.")
                self.start()
                return

            self.port = l[0]
            self.status.set("found new device at port \""+self.port+"\".")
            self.instruct.set("Are you really sure you want to flash Batumi with firmware \""+firmware_name+"\"?")
        elif (not (hasattr(self, 'done'))):
            self.instruct.set("")
            self.ok['state'] = 'disabled'
            root.update()
            self.flash()

        else:
            root.destroy()

    def start(self):
        if (hasattr(self, 'ports')): delattr(self, 'ports')
        if (hasattr(self, 'port')): delattr(self, 'port')
        if (hasattr(self, 'done')): delattr(self, 'done')
        self.ok['state'] = 'normal'
        self.instruct.set("Make sure Batumi is unplugged and click Ok.")

    def createWidgets(self):

        logo = PhotoImage(file="logo.gif")
        label = Label(self, image=logo)
        label.image = logo
        label.pack()

        Label(self,
              text="Batumi firmware update tool",
              font=("Helvetica", 20)).pack(padx=10, pady=10)

        Label(self,
              text=firmware_name,
              font=("Courier", 16)).pack()

        self.instruct = StringVar()

        Label(self,
              textvariable=self.instruct,
              anchor=W,
              justify=LEFT,
              height=4,
              wraplength=300).pack()

        self.ok = Button(self, text="Ok", command=self.ok, font=("Helvetica", 14))
        self.ok.pack(pady=10)

        self.status = StringVar()
        self.status.set("ready.")
        Message(self,
              textvariable=self.status,
              justify=LEFT,
              width=300,
              relief=SUNKEN,
              anchor=W).pack(fill=X)

        self.start()

    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.data = map(lambda c: ord(c), file(firmware_name+".bin", 'rb').read())
        self.pack(padx=2, pady=2, fill=BOTH)
        self.createWidgets()

root = Tk()
root.resizable(width=FALSE, height=FALSE)
root.title("Batumi firmware update tool")
x = (root.winfo_screenwidth() - root.winfo_reqwidth()) / 3
y = (root.winfo_screenheight() - root.winfo_reqheight()) / 3
root.geometry("+%d+%d" % (x, y))
root.wm_attributes("-topmost", 1)
root.focus_force()

app = Application(master=root)
app.mainloop()
