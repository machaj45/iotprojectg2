import numpy as np
from matplotlib import pyplot as plt
from sklearn.neighbors import KNeighborsClassifier

neigh = KNeighborsClassifier(n_neighbors=3, metric='euclidean',n_jobs=2)
A = []
B = []
for i in range(1,10):
    for j in range(1,5):
        B.append(j+i)
    A.append(B)
    B=[]
pos=[]
for i in range(1,10):
    for j in range(1,3):
        B.append(j+i)
    pos.append(B)
    B=[]
neigh.fit(A, pos)
print(neigh.predict([[2.1,3.1, 3.9, 5]]))
#print(neigh.predict_proba([[2.1, 3.1, 3.9, 5]]))
plt.show()
