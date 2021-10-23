import numpy as np
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from scipy.spatial.transform import Rotation as R
import serial #Library: pyserial
#-------------------------------------------------------------------

dataHeader = 'ATT'
port = '/dev/ttyUSB0'
baudRate = 115200

origin = np.array([0, 0, 0])
l = 1
colors = ['red', 'green', 'blue']
axisLimits = [-1.2, 1.2]
plotNEU = True

#-------------------------------------------------------------------

sp = serial.Serial(port, baudrate = baudRate,
                    timeout = None, xonxoff = True, rtscts = True, dsrdtr = True)

def checkHeader(dataHeader, lineString):
    if dataHeader in lineString:
        return True
    return False

def getAttitude(sp, lineEnd = "\n"):
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
            print(dataArray)
            return dataArray
        return None

    except Exception as e:
        print(e)
        return None


plt.ion()
fig = plt.figure(figsize=(7.2, 7.2))
ax = fig.add_subplot(1, 1, 1, aspect='auto', projection = '3d')
ax.set_xlim(axisLimits)
ax.set_ylim(axisLimits)
ax.set_zlim(axisLimits)

v = [[l, 0, 0],
    [0, l, 0],
    [0, 0, l]]
axis = [0, 0, 0]

def vector2Line(vector, origin):
    return [[origin_i, origin_i + vector_i] for origin_i, vector_i in zip(origin, vector)]

for i, v_i in enumerate(v):
    axis[i] = plt.plot(*vector2Line(v_i, origin), c = colors[i], marker = 'o')

if plotNEU:
    for i, v_i in enumerate(v):
        plt.plot(*vector2Line(v_i, origin), c = 'black')

while True:
    att = getAttitude(sp)
    if att is not None:
        att.reverse()
        rotation = R.from_euler('zyx', att, degrees = True)
        for i, v_i in enumerate(v):
            v_i = rotation.apply(v_i)
            axis[i][0].set_data_3d(*vector2Line(v_i, origin))
        fig.canvas.draw()
        fig.canvas.flush_events()