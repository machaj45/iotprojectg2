import numpy as np 
import os
from sklearn.neighbors import KNeighborsClassifier
from sklearn.neighbors import RadiusNeighborsRegressor
from sklearn.neighbors import KNeighborsRegressor
import json
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
class KNN:
    def __init__(self,*arg):
        filename='database.json'
        filename2='points.json'
        if len(arg)==1:
            filename=arg[0]
        elif len(arg)==2:
            filename=arg[0]
            filename2=arg[1]

        with open (filename,"r") as readfile:
            data = json.load(readfile)
        A = []
        for message in data.get("messages"):
<<<<<<< HEAD
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
        #print(i)
        self.neigh = KNeighborsRegressor(n_neighbors=6,radius=0.2, metric='euclidean',n_jobs=2)
=======
            A.append(message)

<<<<<<< HEAD
        #self.neigh = KNeighborsRegressor(n_neighbors=3,radius=1.5, metric='euclidean',n_jobs=2)
>>>>>>> ebc25bed65c8879cfa9886642a36caf15ede6a77
=======
        self.neigh = KNeighborsRegressor(n_neighbors=20,radius=10, metric='euclidean',n_jobs=2)
>>>>>>> b96219c5f2611b3ad5a469201452583877bed67a

        with open (filename2,"r") as readfile:
            data = json.load(readfile)
        self.pos=data["points"]
        self.neigh.fit(A, self.pos)

    def getpoint(self,signal):
        point = self.neigh.predict(signal)
        return point
        
    def plotmap(self,data):
        #if len(data)!=2:
            #data = self.neigh.predict(data)
        if os.environ.get('DISPLAY','') == '':
            print('No display found! TRY ssh -X student@student-ect')
        else:
            self.img=mpimg.imread('map.png')
            self.imgplot = plt.imshow(self.img)
            X = []
            Y = []
            for x in data:
                X.append(x[0])
                Y.append(x[1])
            plt.plot(X,Y,'r*')
            plt.show()
