import numpy as np
from sklearn.neighbors import KNeighborsClassifier
import json

with open ("database2.repair.json","r") as readfile:
    data = json.load(readfile)

A = []
B = []
C = []
i=0;
for message in data.get("messages"):
    for gateway in message:
        i=i+1;
        for gateways in message[gateway]:
            if gateways.get("gateway") != None:
                B.append(gateways.get("rxLevel"))
                C.append(gateways.get("gateway"))
        B = [x for _,x in sorted(zip(C,B))] 
        A.append(B)
        print(B)
        B=[]
print(i)
neigh = KNeighborsClassifier(n_neighbors=3, metric='euclidean',n_jobs=2)
pos=[[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0]]
neigh.fit(A, pos)
print(neigh.predict([[53,50, 43, 38]]))
print(neigh.predict_proba([[53,50, 43, 38]]))
