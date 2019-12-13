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
            A.append(message)
        self.neigh = KNeighborsRegressor(n_neighbors=30,radius=1000, metric='euclidean',n_jobs=1)

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
            self.img=mpimg.imread('/home/student/git/iotprojectg2/backend/learning/knn/map.png')
            self.imgplot = plt.imshow(self.img)
            X = []
            Y = []
            for x in data:
                X.append(x[0])
                Y.append(x[1])
            plt.plot(X,Y,'b*')
            plt.show()
