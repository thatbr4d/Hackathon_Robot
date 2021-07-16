import serial
import tkinter as tk
from tkinter import *
from tkinter import ttk

ser = serial.Serial(port="COM3", baudrate=9600,
                    bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)


class robotarm(Frame):
    def __init__(self):
        tk.Frame.__init__(self)
        self.pack()
        self.master.title("Robot Arm")

        self.btnStop = Button(self, text="Stop", width=25, height=5,
                              command=self.stop)
        self.btnStop.grid(row=1, column=1, columnspan=1, padx=5, pady=5)

        self.btnLeft = Button(self, text="Left", width=25, height=5,
                              command=self.left)
        self.btnLeft.grid(row=1, column=0, columnspan=1, padx=5, pady=5)

        self.btnRight = Button(self, text="Right", width=25, height=5,
                               command=self.right)
        self.btnRight.grid(row=1, column=2, columnspan=1, padx=5, pady=5)

        self.btnUp = Button(self, text="Up", width=25, height=5,
                            command=self.up)
        self.btnUp.grid(row=0, column=1, columnspan=1, padx=5, pady=5)

        self.btnDown = Button(self, text="Down", width=25, height=5,
                              command=self.down)
        self.btnDown.grid(row=2, column=1, columnspan=1, padx=5, pady=5)

        self.btnOpen = Button(self, text="Open", width=25, height=5,
                              command=self.open)
        self.btnOpen.grid(row=3, column=0, columnspan=1, padx=5, pady=5)

        self.btnClose = Button(self, text="Close", width=25, height=5,
                               command=self.close)
        self.btnClose.grid(row=3, column=2, columnspan=1, padx=5, pady=5)

    def stop(self):
        ser.write('0'.encode('Ascii'))

    def left(self):
        ser.write('1'.encode('Ascii'))

    def right(self):
        ser.write('2'.encode('Ascii'))

    def up(self):
        ser.write('3'.encode('Ascii'))

    def down(self):
        ser.write('4'.encode('Ascii'))

    def open(self):
        ser.write('5'.encode('Ascii'))

    def close(self):
        ser.write('6'.encode('Ascii'))

    def __del__(self):
        ser.close()


def main():
    robotarm().mainloop()


if __name__ == '__main__':
    main()
