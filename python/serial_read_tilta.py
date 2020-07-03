import serial

ser = serial.Serial(
    port='/dev/tty.usbmodem1461401',\
    baudrate=115200,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)

print("connected to: " + ser.portstr)

line = []

while True:
    for c in ser.read():
        line.append(c)
        if c == '\n':
            if((len(line) > 12)):
                if(''.join(line).replace('\r\n', '') != ':96060002000161' and ''.join(line).replace('\r\n', '') != ':C90680000000B1'):
                    focus = '0x' + line[9] + line[10] + line[11] + line[12]
                    an_integer = int(focus, 16)
                    print('focus', an_integer / 10)
            line = []
            break

ser.close()