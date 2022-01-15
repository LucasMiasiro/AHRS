from matplotlib.pyplot import axis
import pythonUtils
import os
#-------------------------------------------------------------------

file = "52.CSV"
path = "/run/media/raccoon/INS_Data/"
# path = "logs/"

savePath = "logs/"
fileType = ".pdf"
os.system('mkdir -p ' + savePath + file[:-4])

saveDir = savePath + file[:-4] + "/"
fileout = saveDir + file[:-4] + fileType
fileoutLatLon = saveDir + file[:-4] + "_LatLon" + fileType
fileout3D = saveDir + file[:-4] + "_3D" + fileType
fileoutX = saveDir + file[:-4] + "_X" + fileType
fileoutTheta = saveDir + file[:-4] + "_Theta" + fileType
isY = False
typeNum = 2

size = [14, 9]
size2 = [8, 7]
size3 = [6, 4]
size4 = [6, 5]
size5 = [6, 3]

cutTime = 0

#-------------------------------------------------------------------

df, labels = pythonUtils.readLog(path + file)
df = df[df[labels[0][0]] > cutTime]

print(df)
# pythonUtils.plot2D(df, labels, size, fileout = fileout)
# pythonUtils.plotTrajectory(df, labels, figSize = size2, fileout = fileout3D)
# pythonUtils.plotLatLon(df, labels, size3, fileout = fileoutLatLon)
# pythonUtils.plot2D_X(df, labels, size4, fileout = fileoutX, isY = isY)
pythonUtils.plot2D_theta(df, labels, size5, fileout = fileoutTheta, typeNum = typeNum)