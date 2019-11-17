import numpy as np 
from sklearn.neighbors import KNeighborsClassifier
from sklearn.neighbors import RadiusNeighborsRegressor
from sklearn.neighbors import KNeighborsRegressor
import json
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
with open ("database.json","r") as readfile:
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
neigh = KNeighborsRegressor(n_neighbors=3,radius=1.5, metric='euclidean',n_jobs=2)
pos=[[285,252],[285,252],[285,252],[285,326],[285,326],[285,326],[408,326],[408,326],[408,326],[408,252],[408,252],[408,252],[534,326],[534,326],[534,326],[534,252],[534,252],[671,326],[671,326],[671,326],[671,252],[671,252],[671,252]]
neigh.fit(A, pos)
point = neigh.predict([[53,50, 43, 38]])[0]
print(point)
#print(neigh.predict_proba([[53,50, 43, 38]]))
#print((neigh.kneighbors([[53,50, 43, 38]], n_neighbors=3, return_distance=False)))
img=mpimg.imread('map.png')
imgplot = plt.imshow(img)
plt.plot(point[0],point[1],'r*')
plt.show()
