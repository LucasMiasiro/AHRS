from matplotlib import pyplot as plt, rcParams
import matplotlib
import matplotlib.cm as cmx
import numpy as np

# from pythonUtils.plotAxis import plotAxis
rcParams['grid.linewidth'] = 1.6
# plt.style.use('seaborn-darkgrid')
rcParams['axes.facecolor'] = [0.93, 0.93, 0.93, 1.0]
matplotlib.rcParams['mathtext.fontset'] = 'stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'

import math as m
import pandas as pd

def plotAxis(figSize, axisLimits):

    fig = plt.figure(figsize = figSize, tight_layout = True, facecolor = None)
    ax = fig.add_subplot(1, 1, 1, projection = '3d')
    # ax.set_box_aspect([1,1,1])
    # ax.w_xaxis.set_pane_color((0.93, 0.93, 0.93, 1.0))
    # ax.w_yaxis.set_pane_color((0.93, 0.93, 0.93, 1.0))
    # ax.w_zaxis.set_pane_color((0.93, 0.93, 0.93, 1.0))
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
            ax[i][j].set(title = labels[1][i + j*ncols],
                        xlim = axisLimits)
            ax[i][j].grid(b = True, which = 'both')
            if i == n - 1:
                ax[i][j].set(xlabel = r'$t~[s]$')
    if fileout is not None:
        plt.savefig(fileout, dpi = 400)

    plt.show()

    return plt, fig, ax

def plotTrajectory(df, labels, figSize, fileout = None):

    axisLimits = []
    for i in [6, 7, 8]:
        axisLimits.append([df[labels[0][i]].min(), df[labels[0][i]].max()])
    print(axisLimits)
    plt, fig, ax = plotAxis(figSize, axisLimits)

    cm = plt.get_cmap('RdBu')
    t = df[labels[0][0]]
    cNorm = matplotlib.colors.Normalize(vmin=min(t), vmax=max(t))
    scalarMap = cmx.ScalarMappable(norm=cNorm, cmap=cm)

    ax.scatter(df[labels[0][6]], df[labels[0][7]], df[labels[0][8]],
                c = scalarMap.to_rgba(t), marker = 'o')

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

    fig, ax = plt.subplots(ncols = 1, nrows = 1,
                        figsize = figSize,
                        tight_layout = True)

    ax.set(xlabel = r'$y~[m]$', ylabel = r'$x~[m]$')

    ax.plot(df[labels[0][2]], df[labels[0][1]], ls = '-', color = 'royalblue',
                marker = 'o', markeredgecolor = 'k', mew = 1.2)
    ax.axis('equal')
    ax.grid()

    if fileout is not None:
        plt.savefig(fileout, dpi = 400)

    plt.show()

    return plt, fig

def readLog(file, sep = ',', header = 1,
            dataSpecs = None):
    if dataSpecs is None:
        dataSpecs = {'time': (1, r'$t~[s]$'),
                    'lat': (1, r'Latitude$~[º]$'),
                    'lon': (1, r'Longitude$~[º]$'),
                    'roll': (1, r'$\phi~[º]$'),
                    'pitch': (1, r'$\theta~[º]$'),
                    'yaw': (1, r'$\psi~[º]$'),
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