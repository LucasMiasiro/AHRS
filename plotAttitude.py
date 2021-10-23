# import matplotlib.pyplot as plt
# import mpl_toolkits.mplot3d.axes3d as p3
# import matplotlib.animation as animation
import serial #Library: pyserial
#-------------------------------------------------------------------

dataHeader = 'ATT'
port = '/dev/ttyUSB0'
baudRate = 115200

#-------------------------------------------------------------------

sp = serial.Serial(port, baudrate = baudRate,
                    timeout = None, xonxoff = True, rtscts = True, dsrdtr = True)
lineEnd = '\n'

def checkHeader(dataHeader, lineString):
    if dataHeader in lineString:
        return True
    return False

while True:
    EOL = False
    lineString = ''
    try:
        while not EOL:
            data = sp.read(1)
            if data:
                char = data.decode('ascii')
                EOL = (char == lineEnd)
                if not EOL:
                    lineString += char
            else:
                lineString = None

        if checkHeader(dataHeader, lineString):
            lineSplit = lineString.split(' ')
            dataArray = [float(x) for x in lineSplit[1:-1]]
            # print(lineSplit, end = ' -> ')
            print(dataArray)

    except Exception as e:
        print(e)
        pass