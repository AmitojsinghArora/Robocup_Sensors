import serial
import time

arduino = serial.Serial(port='COM6',  baudrate=9600, timeout=.1) #"/dev/ttyUSB0" for linux and "COM6" for Windows


def write_read(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.05)
    data = arduino.readline()
    return  data


while True:
    task = input("Command: ")
    value  = write_read(task)
    print(value)
