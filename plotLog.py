import pythonUtils
#-------------------------------------------------------------------

file = "logs/0.CSV"
fileout = file[:-4] + ".eps"
fileoutLatLon = file[:-4] + "LatLon" + ".eps"
size = [14, 7]

#-------------------------------------------------------------------

df, labels = pythonUtils.readLog(file)
print(df)
pythonUtils.plot2D(df, labels, size, fileout = fileout)
pythonUtils.plotLatLon(df, labels, size, fileout = fileoutLatLon)