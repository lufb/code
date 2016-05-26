from functools import  partial as pto
from tkinter import Tk, Button, X, messagebox

WARN = "warn"
CRIT = "crit"
REGU = "regu"

signs = {"do not enter:":CRIT,
         "railroad crossing":WARN,
        "55\nspeed limit":REGU,
        "wront way":CRIT,
        "merging traffic":WARN,
        "one way":REGU,
         }

critCB = lambda: messagebox.showerror("Error", "Error button Pressed!")
warnCB = lambda: messagebox.showerror("Warning", "Warnig Button Pressed!")
infoCB = lambda: messagebox.showinfo("Info", "Info button Pressed!")

top = Tk()
top.title("Road signs")
Button(top, text = 'Quit', command = top.quit, bg = "red", fg = "white").pack()

mybotton = pto(Button, top)
CritButton = pto(mybotton, command = critCB)
WarnButton = pto(mybotton, command = warnCB)
ReguButton = pto(mybotton, command = infoCB)

for each in signs:
    signtype = signs[each]
    cmd = "%sButton(text=%r%s).pack(fill=X, expand=True)" % (signtype.title(), each,".upper()" if signs == CRIT else ".title()")
    eval(cmd)
    
top.mainloop()
