from matplotlib import pyplot as plt, rcParams, use
rcParams['grid.linewidth'] = 1.6
plt.style.use('seaborn-darkgrid')
rcParams['axes.facecolor'] = [0.93, 0.93, 0.93, 1.0]
import math as m
import pandas as pd

def plot2D(df, labels, figSize, ncols = 3, axisLimits = None, fileout = None):
    n = len(labels[0])
    nrows = m.ceil((n - 1)/ncols)
    fig, ax = plt.subplots(ncols = ncols, nrows = nrows,
                        figsize = figSize,
                        tight_layout = True, sharex = True)

    time = [labels[0][0], labels[1][0]]
    labels = [labels[0][1:], labels[1][1:]]
    axisLimits = [df[time[0]].min(), df[time[0]].max()]
    for i in range(0, nrows):
        for j in range(0, ncols):
            ax[i][j].plot(df[time[0]],
                            df[labels[0][i + j*ncols]],
                            label = labels[1][i + j*ncols])
            ax[i][j].set(title = labels[1][i + j*ncols],
                        xlim = axisLimits)
            if i == n - 1:
                ax[i][j].set(xlabel = r'$t~[s]$')
    if fileout is not None:
        plt.savefig(fileout, dpi = 400)

    plt.show()

    return plt, fig, ax

def readLog(file, sep = ',', header = None,
            dataSpecs = None):
    if dataSpecs is None:
        dataSpecs = {'time': (1, r'$t~[s]$'),
                    'unused0': (0, ''),
                    'unused1': (0, ''),
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