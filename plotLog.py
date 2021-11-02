import pythonUtils
#-------------------------------------------------------------------

file = "logs/0.CSV"
fileout = file[:-4] + ".eps"
size = [14, 7]

#-------------------------------------------------------------------

df, labels = pythonUtils.readLog(file)
pythonUtils.plot2D(df, labels, size, fileout = fileout)