import pandas as pd
import math as m
import matplotlib.pyplot as plt
import matplotlib.style as stl
from matplotlib import rcParams

rcParams['text.usetex'] = True
rcParams['mathtext.fontset'] = 'cm'
# stl.use('seaborn-darkgrid')

rcParams['grid.linewidth'] = 1.6
rcParams['grid.color'] = 'white'

df = pd.read_csv("magData.csv", sep = ",")

xSize = 200
xlimits = [-500, 500]
xlimits2 = [-400, 400]

colors = ['royalblue', 'coral']

lim = [ 
        [min(df['mx']), max(df['mx'])],
        [min(df['my']), max(df['my'])],
        [min(df['mz']), max(df['mz'])]
        ]

r = [(maxi - mini)/2 for mini, maxi in lim]
bias = [(mini + maxi)/2 for mini, maxi in lim]
R = (r[0]*r[1]*r[2])**(1/3)

df['mxCal'] = (df['mx'] - bias[0])*R/r[0]
df['myCal'] = (df['my'] - bias[1])*R/r[1]
df['mzCal'] = (df['mz'] - bias[2])*R/r[2]

fig, ax = plt.subplots(nrows = 2, ncols = 2, figsize = [8, 8])

ax[1][1].grid('major')
ax[1][1].axes.set_facecolor((0.95, 0.95, 0.95, 1.0))
ax[1][1].set_axisbelow(True)
ax[1][1].scatter(df['mx'], df['my'], s = 1, alpha = 0.4, color = colors[0])
ax[1][1].scatter(df['mxCal'], df['myCal'], s = 1, alpha = 0.4, color = colors[1])
# ax[1][1].scatter(0, 0, marker = '+', color = 'black', s = 200, alpha = 1.0)
ax[1][1].plot([0, xSize], [0, 0], color = 'red', lw = 2.0)
ax[1][1].plot([0, 0], [0, xSize], color = 'green', lw = 2.0)
ax[1][1].axis('equal')
ax[1][1].set(xlim = xlimits, ylim = xlimits, xlabel =r'$M_x~[mG]$')#, ylabel =r'$M_y~[mG]$')

ax[0][1].grid('major')
ax[0][1].axes.set_facecolor((0.95, 0.95, 0.95, 1.0))
ax[0][1].set_axisbelow(True)
ax[0][1].scatter(df['mx'], df['mz'], s = 1, alpha = 0.4, color = colors[0])
ax[0][1].scatter(df['mxCal'], df['mzCal'], s = 1, alpha = 0.4, color = colors[1])
# ax[1][1].scatter(0, 0, marker = '+', color = 'black', s = 200, alpha = 1.0)
ax[0][1].plot([0, xSize], [0, 0], color = 'red', lw = 2.0)
ax[0][1].plot([0, 0], [0, xSize], color = 'blue', lw = 2.0)
ax[0][1].axis('equal')
ax[0][1].set(xlim = xlimits, ylim = xlimits[::-1], ylabel =r'$M_z~[mG]$')#, ylabel =r'$M_y~[mG]$')

ax[1][0].grid('major')
ax[1][0].axes.set_facecolor((0.95, 0.95, 0.95, 1.0))
ax[1][0].set_axisbelow(True)
ax[1][0].scatter(df['mz'], df['my'], s = 1, alpha = 0.4, color = colors[0])
ax[1][0].scatter(df['mzCal'], df['myCal'], s = 1, alpha = 0.4, color = colors[1])
# ax[1][1].scatter(0, 0, marker = '+', color = 'black', s = 200, alpha = 1.0)
ax[1][0].plot([0, xSize], [0, 0], color = 'blue', lw = 2.0)
ax[1][0].plot([0, 0], [0, xSize], color = 'green', lw = 2.0)
ax[1][0].axis('equal')
ax[1][0].set(xlim = xlimits[::-1], ylim = xlimits, xlabel =r'$M_z~[mG]$', ylabel =r'$M_y~[mG]$')

fig.savefig("magCal1.pdf")

fig2 = plt.figure(figsize = [3, 3], tight_layout = True, facecolor = None)
ax2 = fig2.add_subplot(1, 1, 1, aspect='auto', projection = '3d')
ax2.set_box_aspect([1,1,1])
ax2.w_xaxis.set_pane_color((0.95, 0.95, 0.95, 1.0))
ax2.w_yaxis.set_pane_color((0.95, 0.95, 0.95, 1.0))
ax2.w_zaxis.set_pane_color((0.95, 0.95, 0.95, 1.0))
ax2.set_xlim(xlimits2)
ax2.set_ylim(xlimits2)
ax2.set_zlim(xlimits2)
ax2.set_xlabel(r'$M_x~[mG]$')
ax2.set_ylabel(r'$M_y~[mG]$')
ax2.set_zlabel(r'$M_z~[mG]$')
ax2.scatter(df['mx'], df['my'], df['mz'], s = 1, color = colors[0], alpha = 0.2)
ax2.scatter(df['mxCal'], df['myCal'], df['mzCal'], s = 1, color = colors[1], alpha = 0.2)
ax2.plot([0, xSize*2.0], [0, 0], [0,0], color = 'red', alpha = 1.0, lw = 2.0)
ax2.plot([0, 0], [0, xSize*2.0], [0, 0], color = 'green', alpha = 1.0, lw = 2.0)
ax2.plot([0, 0], [0, 0], [0, xSize*2.0], color = 'blue', alpha = 1.0, lw = 2.0)

ax2.invert_xaxis()
ax2.invert_yaxis()
ax2.invert_zaxis()

fig2.savefig("magCal2.pdf")

plt.show()