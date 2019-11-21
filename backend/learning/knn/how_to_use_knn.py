from KNN import *

kn = KNN('../database_test3.json')
signal = [[53,50, 43, 38]]
point = kn.getpoint(signal)
print(point)
kn.plotmap(point)

