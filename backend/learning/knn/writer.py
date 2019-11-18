import json
points=[[285,252],[285,252],[285,252],[285,326],[285,326],[285,326],[408,326],[408,326],[408,326],[408,252],[408,252],[408,252],[534,326],[534,326],[534,326],[534,252],[534,252],[671,326],[671,326],[671,326],[671,252],[671,252],[671,252]]
lenght = len (points)
dictonary = {"length":lenght,"points":points}
with open('points.json', 'w') as output_file:
    json.dump(dictonary,output_file,indent=2,sort_keys=False)
