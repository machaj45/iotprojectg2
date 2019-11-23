import tkinter as tk
from tkinter import filedialog, Text
import os, sys, subprocess
apps=[]
if os.path.isfile('save'):
    with open('save','r') as f:
        ta = f.read()
        ta=ta.split(',') 
        apps = [x for x in ta if x.strip()]

def addApp():
    filename=filedialog.askopenfilename(initialdir="~/gui",title="Select file",filetypes=(("python","*.py"),("shell file","*.sh"),("all files","*.*")))
    apps.append(filename)
    for w in frame.winfo_children():
        w.destroy()
    for app in apps:
        label = tk.Label(frame,text=app,bg="gray")
        label.pack()
def runAps():
    for app in apps:
        open_file(app)

def open_file(filename):
    if sys.platform == "win32":
        os.startfile(filename)
    else:
        opener ="open" if sys.platform == "darwin" else "xdg-open"
        subprocess.call([opener, filename])
root = tk.Tk()

canvas =tk.Canvas(root,height=700,width=700,bg="#263D42")
canvas.pack()

frame = tk.Frame(root,bg="white")
frame.place(relwidth=0.8,relheight=0.8,rely=0.1,relx=0.1)

bopenfile= tk.Button(root,text="Open file",padx=10,pady=5,fg="white",bg="#263d42",command=addApp)

bopenfile.pack()

brunaps= tk.Button(root,text="Run apps",padx=10,pady=5,fg="white",bg="#263d42",command=runAps)
brunaps.pack()

for app in apps:
    label = tk.Label(frame,text=app,bg="gray")
    label.pack()
root.mainloop()
with open('save','w') as f:
    for app in apps:
        f.write(app + ',')

