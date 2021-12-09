from matplotlib import pyplot as plt, rcParams
rcParams['grid.linewidth'] = 1.6
rcParams['grid.color'] = 'white'
from mpl_toolkits.mplot3d import Axes3D

def plotAxis3D(figSize, axisLimits):
    plt.ion()
    fig = plt.figure(figsize = figSize, tight_layout = True, facecolor = None)
    ax = fig.add_subplot(1, 1, 1, aspect='auto', projection = '3d')
    ax.set_box_aspect([1,1,1])
    ax.w_xaxis.set_pane_color((0.93, 0.93, 0.93, 1.0))
    ax.w_yaxis.set_pane_color((0.93, 0.93, 0.93, 1.0))
    ax.w_zaxis.set_pane_color((0.93, 0.93, 0.93, 1.0))
    ax.set_xlim(axisLimits)
    ax.set_ylim(axisLimits)
    ax.set_zlim(axisLimits)
    ax.set_xlabel(r'x')
    ax.set_ylabel(r'y')
    ax.set_zlabel(r'z')
    ax.invert_xaxis()
    ax.invert_yaxis()
    ax.invert_zaxis()

    return plt, fig, ax