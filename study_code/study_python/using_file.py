poem='''\
Programming is funWhen the work is doneif you wanna make your work also fun: use

python
'''

f=open("poem.txt","w") # open for 'w'riting
f.write(poem) # write text to file
f.close() # close the file

f=open('poem.txt')# if no mode is specified, 'r'ead mode is assumed by default
while True: 
    line=f.readline() 
    if len(line)==0: # Zero length indicates EOF 
        break 
    print (line) # Notice comma to avoid automatic newline added by Python
f.close() # close the file