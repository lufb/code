f = open("data.txt", "w")
f.write("hello ")
f.write("world\n")
f.close()

f = open("data.txt", "r")
text = f.read()
print(f)
print(text.split())
