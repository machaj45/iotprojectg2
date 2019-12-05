import json

points_room1 = []
points_room1 = [[289, 202], [289, 264], [289, 327], [345, 327],
                [409, 202], [409, 264], [409, 327], [470, 327],
                [532, 202], [532, 264], [532, 327], [595, 327],
                [673, 202], [673, 264], [673, 327], [746, 327],
                [809, 202], [809, 264], [809, 327], [870, 327],
                [932, 202], [932, 264], [932, 327], [993, 327],
                [1054, 202], [1054, 264], [1054, 327]]

points_room2 = []
points_room2 = [[290, 535], [290, 466], [290, 408], [341, 408],
	[397, 535], [397, 466], [397, 408], [449, 408],
	[503, 535], [503, 466], [503, 408], [556, 408],
	[611, 535], [611, 466], [611, 408], [664, 408],
	[718, 535], [718, 466], [718, 408], [772, 408],
	[828, 535], [828, 466], [828, 408], [879, 408],
	[936, 535], [936, 466], [936, 408]]

collection = []
all_points = points_room1
for n in points_room2:
	all_points.append(n)

for elem in all_points:
    for i in range(4):
            collection.append(elem)

length = len (collection)
dictonary={}
dictonary["length"]=length
dictonary["points"]=collection
with open('temp_points.json', 'w') as output_file:
    json.dump(dictonary,output_file,indent=2,sort_keys=False)

print(collection)
