from matplotlib import pyplot as plt, rcParams
rcParams['grid.linewidth'] = 1.5
plt.style.use('seaborn-darkgrid')
from mpl_toolkits.mplot3d import Axes3D
import pythonUtils
#-------------------------------------------------------------------

dataHeader = "MAG"
port = '/dev/ttyUSB0'
baudRate = 115200

color = 'royalblue'
alpha = 0.5
origin = [0, 0, 0]
l = 300
axisLimits = [-600, 600]
plotNEU = True
figSize = (7, 7)

#-------------------------------------------------------------------

sp = pythonUtils.serial.Serial(port, baudrate = baudRate,
                    timeout = None, xonxoff = True, rtscts = True, dsrdtr = True)

plt.ion()
fig = plt.figure(figsize = figSize)
ax = fig.add_subplot(1, 1, 1, aspect='auto', projection = '3d')
ax.set_xlim(axisLimits)
ax.set_ylim(axisLimits)
ax.set_zlim(axisLimits)

v = [[l, 0, 0],
    [0, l, 0],
    [0, 0, l]]

def vector2Line(vector, origin):
    return [[origin_i, origin_i + vector_i] for origin_i, vector_i in zip(origin, vector)]

mx, my, mz = [], [], []
axis = plt.plot(mx, my, mz, ls = '', color = color, marker = 'o', alpha = alpha)

if plotNEU:
    for i, v_i in enumerate(v):
        plt.plot(*vector2Line(v_i, origin), c = 'black')

while True:
    mag = pythonUtils.getData(sp, dataHeader = dataHeader)
    if mag is not None:
        mx.append(mag[0])
        my.append(mag[1])
        mz.append(mag[2])
        axis[0].set_data_3d(mx, my, mz)
        fig.canvas.draw()
        fig.canvas.flush_events()