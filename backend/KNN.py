import numpy as np
from sklearn.neighbors import KNeighborsClassifier
import json

with open ("database2.repair.json","r") as readfile:
    data = json.load(readfile)
    print(data)














def knn():
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
