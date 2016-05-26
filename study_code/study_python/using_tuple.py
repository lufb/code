zoo = ("wolf", "element", "penguin")
print("number of animals in the zoo is", len(zoo))

new_zoo = ("monkey", "dolphin", zoo)
print("now number of animals in the zoo is", len(new_zoo))
print("all animals in new_zoo", new_zoo)
print("add animals in new_zoo is")
print(new_zoo[2])
print(new_zoo[2][1])

age = 12
name = "lufubo"
print("%s is %s year old" % (name, age))