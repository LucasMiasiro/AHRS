from matplotlib.pyplot import axis
import pythonUtils
#-------------------------------------------------------------------

file = "18.CSV"
path = "/run/media/raccoon/INS_Data/"
# path = "logs/"
saveDir = "logs/"
fileout = saveDir + file[:-4] + ".eps"
fileoutLatLon = saveDir + file[:-4] + "LatLon" + ".eps"
fileout3D = saveDir + file[:-4] + "3D" + ".eps"
size = [14, 7]

cutTime = 0

#-------------------------------------------------------------------

df, labels = pythonUtils.readLog(path + file)
df = df[df[labels[0][0]] > cutTime]

print(df)
pythonUtils.plot2D(df, labels, size, fileout = fileout)
pythonUtils.plotTrajectory(df, labels, figSize = size, fileout = fileout3D)
pythonUtils.plotLatLon(df, labels, size, fileout = fileoutLatLon)