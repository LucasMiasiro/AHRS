from matplotlib import pyplot as plt, rcParams
import matplotlib
import matplotlib.cm as cmx
import numpy as np
from matplotlib.ticker import MultipleLocator, AutoMinorLocator

# from pythonUtils.plotAxis import plotAxis
# rcParams['grid.linewidth'] = 1.6
# plt.style.use('seaborn-darkgrid')
# rcParams['axes.facecolor'] = [0.93, 0.93, 0.93, 1.0]

rcParams['axes.facecolor'] = [1.0, 1.0, 1.0, 1.0]
matplotlib.rcParams['text.usetex'] = True
matplotlib.rcParams['mathtext.fontset'] = 'cm'

import math as m
import pandas as pd

def plot2D_theta(df, labels, figSize, axisLimits = None, fileout = None, typeNum = 0):
    rcParams['grid.linewidth'] = 0.6
    rcParams['grid.color'] = 'black'
    fig, ax = plt.subplots(ncols = 1, nrows = 1,
                        figsize = figSize,
                        tight_layout = True, sharex = True)

    time = [labels[0][0], labels[1][0]]
    labels = [labels[0][3:], labels[1][3:]]
    axisLimits = [df[time[0]].min(), df[time[0]].max()]

    ax.plot(df[time[0]],
                    df[labels[0][typeNum]], label = labels[1][typeNum])
    ax.set(ylabel = labels[1][typeNum], xlim = axisLimits)

    ax.tick_params(which='minor', width=0.2)
    ax.yaxis.set_minor_locator(AutoMinorLocator(5))
    ax.xaxis.set_minor_locator(AutoMinorLocator(5))
    ax.grid(b = True, which = 'minor', color = 'lightgray')
    ax.grid(b = True, which = 'major')

    ax.set(xlabel = r'$t~[s]$')

    if fileout is not None:
        plt.savefig(fileout, dpi = 400)

    plt.show()

    return plt, fig, ax

def plot2D_X(df, labels, figSize, axisLimits = None, fileout = None, isY = False):
    rcParams['grid.linewidth'] = 0.6
    rcParams['grid.color'] = 'black'
    fig, ax = plt.subplots(ncols = 1, nrows = 2,
                        figsize = figSize,
                        tight_layout = True, sharex = True)

    time = [labels[0][0], labels[1][0]]
    labels = [labels[0][3:], labels[1][3:]]
    axisLimits = [df[time[0]].min(), df[time[0]].max()]

    if isY: 
        ax[0].plot(df[time[0]],
                        df[labels[0][4]], label = labels[1][4])
        # ax[0].set(title = labels[1][3], xlim = axisLimits)
        ax[0].set(ylabel = labels[1][4], xlim = axisLimits)

        ax[1].plot(df[time[0]],
                        df[labels[0][7]], label = labels[1][7])
        # ax[1].set(title = labels[1][6], xlim = axisLimits)
        ax[1].set(ylabel = labels[1][7], xlim = axisLimits)

    else:
        ax[0].plot(df[time[0]],
                        df[labels[0][3]], label = labels[1][3])
        # ax[0].set(title = labels[1][3], xlim = axisLimits)
        ax[0].set(ylabel = labels[1][3], xlim = axisLimits)

        ax[1].plot(df[time[0]],
                        df[labels[0][6]], label = labels[1][6])
        # ax[1].set(title = labels[1][6], xlim = axisLimits)
        ax[1].set(ylabel = labels[1][6], xlim = axisLimits)

    for i in range(2):
        ax[i].tick_params(which='minor', width=0.2)
        ax[i].yaxis.set_minor_locator(AutoMinorLocator(5))
        ax[i].xaxis.set_minor_locator(AutoMinorLocator(5))
        ax[i].grid(b = True, which = 'minor', color = 'lightgray')
        ax[i].grid(b = True, which = 'major')

    ax[1].set(xlabel = r'$t~[s]$')

    if fileout is not None:
        plt.savefig(fileout, dpi = 400)

    plt.show()

    return plt, fig, ax

def plotAxis(figSize, axisLimits):

    fig = plt.figure(figsize = figSize, tight_layout = True, facecolor = None)
    ax = fig.add_subplot(1, 1, 1, projection = '3d')
    # ax.set_box_aspect([1,1,1])
    ax.w_xaxis.set_pane_color((0.93, 0.93, 0.93, 1.0))
    ax.w_yaxis.set_pane_color((0.93, 0.93, 0.93, 1.0))
    ax.w_zaxis.set_pane_color((0.93, 0.93, 0.93, 1.0))
    ax.set_xlabel(r'$x~[m]$')
    ax.set_ylabel(r'$y~[m]$')
    ax.set_zlabel(r'$z~[m]$')
    ax.set_xlim(axisLimits[0])
    ax.set_ylim(axisLimits[1])
    ax.set_zlim(axisLimits[2])
    ax.invert_xaxis()
    # ax.invert_yaxis()
    ax.invert_zaxis()
    ax.view_init(30, 45)

    return plt, fig, ax

def plot2D(df, labels, figSize, ncols = 3, axisLimits = None, fileout = None):
    rcParams['grid.linewidth'] = 0.6
    rcParams['grid.color'] = 'black'
    n = len(labels[0]) - 3
    nrows = m.ceil((n - 1)/ncols)
    fig, ax = plt.subplots(ncols = ncols, nrows = nrows,
                        figsize = figSize,
                        tight_layout = True, sharex = True)

    time = [labels[0][0], labels[1][0]]
    labels = [labels[0][3:], labels[1][3:]]
    axisLimits = [df[time[0]].min(), df[time[0]].max()]
    for i in range(0, nrows):
        for j in range(0, ncols):
            ax[i][j].plot(df[time[0]],
                            df[labels[0][i + j*ncols]],
                            label = labels[1][i + j*ncols])
            # ax[i][j].set(title = labels[1][i + j*ncols],
            ax[i][j].set(ylabel = labels[1][i + j*ncols],
                        xlim = axisLimits)
            ax[i][j].tick_params(which='minor', width=0.2)
            ax[i][j].yaxis.set_minor_locator(AutoMinorLocator(5))
            ax[i][j].xaxis.set_minor_locator(AutoMinorLocator(5))
            ax[i][j].grid(b = True, which = 'minor', color = 'lightgray')
            ax[i][j].grid(b = True, which = 'major')
            if i == nrows - 1:
                ax[i][j].set(xlabel = r'$t~[s]$')
    if fileout is not None:
        plt.savefig(fileout, dpi = 400)

    plt.show()

    return plt, fig, ax

def plotTrajectory(df, labels, figSize, fileout = None):

    rcParams['grid.linewidth'] = 2.0
    rcParams['grid.color'] = 'white'

    axisLimits = []
    for i in [6, 7, 8]:
        axisLimits.append([df[labels[0][i]].min(), df[labels[0][i]].max()])
    print(axisLimits)
    plt, fig, ax = plotAxis(figSize, axisLimits)

    # cm = plt.get_cmap('RdBu')
    cm = plt.get_cmap('plasma')
    t = df[labels[0][0]]
    cNorm = matplotlib.colors.Normalize(vmin=min(t), vmax=max(t))
    scalarMap = cmx.ScalarMappable(norm=cNorm, cmap=cm)

    # ax.scatter([0, 0], [0, 0], [0, 0], c = 'black', marker = 'x', zorder = 2, s = 300)

    ax.scatter(df[labels[0][6]], df[labels[0][7]], df[labels[0][8]],
                c = scalarMap.to_rgba(t), marker = 'o', zorder = 1)

    ax.set_box_aspect((np.ptp(df[labels[0][6]]),
                        np.ptp(df[labels[0][7]]),
                        10*np.ptp(df[labels[0][8]])))

    ax.plot(df[labels[0][6]], df[labels[0][7]], df[labels[0][8]],
                alpha = 1.0, color = 'k', ls = '-', lw = '0.5')

    scalarMap.set_array(t)

    if fileout is not None:
        plt.savefig(fileout, dpi = 400)

    plt.show()

def plotLatLon(df, labels, figSize, axisLimits = None, fileout = None):

    rcParams['grid.linewidth'] = 0.6
    rcParams['grid.color'] = 'black'

    fig, ax = plt.subplots(ncols = 1, nrows = 1,
                        figsize = figSize,
                        tight_layout = True)

    ax.set(xlabel = r'${}^{S}y~[m]$', ylabel = r'${}^{S}x~[m]$')

    ax.plot(df[labels[0][2]], df[labels[0][1]], ls = '-', color = 'royalblue',
                marker = 'o', markeredgecolor = 'k', mew = 1.1)

    # ax.scatter([0, 0], [0, 0], c = 'black', marker = 'x', zorder = 2, s = 300)
    ax.axis('equal')

    ax.tick_params(which='minor', width=0.2)
    ax.yaxis.set_minor_locator(AutoMinorLocator(5))
    ax.xaxis.set_minor_locator(AutoMinorLocator(5))
    ax.grid(b = True, which = 'minor', color = 'lightgray')
    ax.grid(b = True, which = 'major')

    if fileout is not None:
        plt.savefig(fileout, dpi = 400)

    plt.show()

    return plt, fig

def readLog(file, sep = ',', header = 1,
            dataSpecs = None):
    if dataSpecs is None:
        dataSpecs = {'time': (1, r'$t~[s]$'),
                    'lat': (1, r'Latitude$~[{}^\circ]$'),
                    'lon': (1, r'Longitude$~[{}^\circ]$'),
                    'roll': (1, r'$\phi~[{}^\circ]$'),
                    'pitch': (1, r'$\theta~[{}^\circ]$'),
                    'yaw': (1, r'$\psi~[{}^\circ]$'),
                    'posX': (1, r'$x~[m]$'),
                    'posY': (1, r'$y~[m]$'),
                    'posZ': (1, r'$z~[m]$'),
                    'velX': (1, r'$\dot{x}~[m/s]$'),
                    'velY': (1, r'$\dot{y}~[m/s]$'),
                    'velZ': (1, r'$\dot{z}~[m/s]$'),
                    'unused2': (0, '')
                    }
    
    useCols = []
    texCols = []
    for key, use in dataSpecs.items():
        if use[0] == 1:
            useCols.append(key)
            texCols.append(use[1])

    print('Using:', useCols)

    df = pd.read_csv(file, sep = sep, header = header,
                    names = dataSpecs.keys())[useCols]
    df.fillna(0, inplace = True)

    df['roll'] *= 180/m.pi
    df['pitch'] *= 180/m.pi
    df['yaw'] *= 180/m.pi
    df['time'] -= df['time'][0]
    df['time'] *= 1/1000

    labels = [useCols, texCols]
    return df, labels

if __name__ == '__main__':
    file = "../logs/0.CSV"
    df, labels = readLog(file)
    plot2D(df, labels, [14, 7])