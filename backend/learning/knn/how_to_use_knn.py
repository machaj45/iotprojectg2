from KNN import *
import statistics

kn = KNN()
signal = [[53,46, 55, 39]]
point = kn.getpoint(signal)
print(point)
point=[0,0]
#signal = [[45,42,55,34],[54,30,59,34],[52,38,55,32],[41,45,42,27],[41,41,52,39]]
signal = [[58,45,53,44], [52,42,42,31], [52,54,44,34], [64,60,50,36]]
A = []
B = []
C = []
D = []
for i in signal:
	A.append(i[0])
	B.append(i[1])
	C.append(i[2])
	D.append(i[3])
X = []
X.append(statistics.mean(A))
X.append(statistics.mean(B))
X.append(statistics.mean(C))
X.append(statistics.mean(D))

A = []
A.append(X)
point = kn.getpoint(A)
print(point)
kn.plotmap(point)

