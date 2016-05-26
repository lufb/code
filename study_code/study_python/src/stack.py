from setuptools.sandbox import pushd
from pickle import POP
from idlelib.textView import view_file
stack = []

def pushit():
    stack.append(input("enter new string:").strip())

def popit():
    if len(stack) == 0:
        print("cannot pop from an empty stack")
    else:
        print("Removed [", stack.pop(), "]")

def viewstack():
    print(stack)
    
CMDs = {'u': pushit, 'o': popit, 'v':viewstack}

def showmenu():
    pr="""
    push
    POP
    view
    quit
    
    enter choice:"""
    
while True:
    while True:
        try:
            choice = input("enter").strip().lower()
        except(EOFError, KeyboardInterrupt, IndentationError):
            choice = "q"
        print("you picked:[%s]" % choice)
        
        if choice not in "uovq":
            print("Ivalid option, try again")
        else:
            break
        
    if choice == "q":
        break
    CMDs[choice]()
    
if __name__ == "__main__":
    showmenu()