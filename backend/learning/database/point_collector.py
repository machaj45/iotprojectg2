import json

#first measurement of room1
points_room1 = []
points_room1 = [[289, 202], [289, 264], [289, 327], [345, 327],
                [409, 202], [409, 264], [409, 327], [470, 327],
                [532, 202], [532, 264], [532, 327], [595, 327],
                [673, 202], [673, 264], [673, 327], [746, 327],
                [809, 202], [809, 264], [809, 327], [870, 327],
                [932, 202], [932, 264], [932, 327], [993, 327],
                [1054, 202], [1054, 264], [1054, 327]]

#first measurement of room2
points_room2 = []
points_room2 = [[290, 535], [290, 466], [290, 408], [341, 408],
	[397, 535], [397, 466], [397, 408], [449, 408],
	[503, 535], [503, 466], [503, 408], [556, 408],
	[611, 535], [611, 466], [611, 408], [664, 408],
	[718, 535], [718, 466], [718, 408], [772, 408],
	[828, 535], [828, 466], [828, 408], [879, 408],
	[936, 535], [936, 466], [936, 408]]


#second measurement tables of room1
tables_room1 = []
tables_room1 = [[347, 179], [347, 257],
                [471, 179], [471, 257],
                [595, 179], [595, 257],
                [746, 179], [746, 257],
                [870, 179], [870, 257],
                [994, 179], [994, 257]]

#second measurement windows of room1
windows_room1 = []
windows_room1 = [[265,147], [292, 147], [339, 147], [375, 147], [407, 147], [454, 147],
                [496, 147], [531, 147], [575, 147], [613, 147], [650, 147], [693, 147],
                [731, 147], [770, 147], [813, 147], [849, 147], [888, 147], [930, 147],
                [961, 147], [1002, 147], [1051, 147], [1079, 147]]

#second measurement tables of room2
tables_room2 = []
tables_room2 = [[340, 554], [340, 449],
                [448, 554], [448, 449],
                [556, 554], [556, 449],
                [665, 554], [665, 449],
                [772, 554], [772, 449],
                [881, 554], [881, 449]]

#second measurement windows of room2
windows_room2 = []
windows_room2 = [[964,593], [938, 593], [886, 593], [848, 593], [814, 593], [774, 593],
                [730, 593], [700, 593], [655, 593], [614, 593], [581, 593], [530, 593],
                [496, 593], [462, 593], [417, 593], [377, 593], [343, 593], [299, 593], [265, 593]]

collection = []
#all_points = points_room1
#for n in points_room2:
#	all_points.append(n)

for elem in points_room1:
    for i in range(6):
        collection.append(elem)

all_points_1 = []
all_points_1 = tables_room1
for n in windows_room1:
    all_points_1.append(n)

for elem in all_points_1:
    for i in range(10):
        collection.append(elem)

for elem in points_room2:
    for i in range(6):
        collection.append(elem)

all_points_2 = []
all_points_2 = tables_room2
for n in windows_room2:
    all_points_2.append(n)

for elem in all_points_2:
    for i in range(10):
        collection.append(elem)

length = len (collection)
dictonary={}
dictonary["length"]=length
dictonary["points"]=collection
with open('temp_points.json', 'w') as output_file:
    json.dump(dictonary,output_file,indent=2,sort_keys=False)

print(collection)
