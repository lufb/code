'''import tkinter

top = tkinter.Tk()

hello = tkinter.Label(top, text = "hello world!")
hello.pack()

quit = tkinter.Button(top, text = "Quit", command = top.quit, bg='red', fg = "white")
quit.pack(fill = tkinter.X, expand = 1)

tkinter.mainloop()'''

from tkinter import *

def resize(ev = None):
    label.config(font="Helvetica -%d bold" % scale.get())
    
top = Tk()
top.geometry("250x150")

label = Label(top, text = "hello world", font = "Helvetica -12 bold")
label.pack(fill = Y, expand = 1)

scale = Scale(top, from_ = 10, to = 40, orient = HORIZONTAL, command = resize)
scale.set(12)
scale.pack(fill = X, expand = 1)

quit = Button(top, text = "Quit", command = top.quit, activeforeground = "white", activebackground = "red")
quit.pack()

mainloop()