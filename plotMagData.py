import pythonUtils
#-------------------------------------------------------------------

dataHeader = "MAG"
port = '/dev/ttyUSB0'
baudRate = 115200

color = 'royalblue'
colors = ['tomato', 'mediumspringgreen', 'royalblue']
alpha = 0.3
origin = [0, 0, 0]
l = 800
axisLimits = [-600, 600]
plotNEU = True
figSize = (7, 7)

#-------------------------------------------------------------------

sp = pythonUtils.serial.Serial(port, baudrate = baudRate,
                    timeout = None, xonxoff = True, rtscts = True, dsrdtr = True)
v = [[l, 0, 0],
    [0, l, 0],
    [0, 0, l]]

plt, fig, ax = pythonUtils.plotAxis(figSize, axisLimits)

def vector2Line(vector, origin):
    return [[origin_i, origin_i + vector_i] for origin_i, vector_i in zip(origin, vector)]

mx, my, mz = [], [], []
axis = plt.plot(mx, my, mz, ls = '', color = color, marker = 'o', alpha = alpha)

if plotNEU:
    for i, v_i in enumerate(v):
        plt.plot(*vector2Line(v_i, origin), c = colors[i],
                alpha = 0.7, ls = '--', lw = '1.3')

while True:
    mag = pythonUtils.getData(sp, dataHeader = dataHeader)
    if mag is not None:
        mx.append(mag[0])
        my.append(mag[1])
        mz.append(mag[2])
        axis[0].set_data_3d(mx, my, mz)
        fig.canvas.draw()
        fig.canvas.flush_events()