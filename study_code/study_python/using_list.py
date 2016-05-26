shoplist = ["apple", "carrot", "banana", "abc"]

print("item 0 is ", shoplist[0])
print("item 1 is ", shoplist[1])
print("item 2 is ", shoplist[2])
print("item 3 is ", shoplist[3])
print("item -1 is ", shoplist[-1]) #最后一个元素
print("item -2 is ", shoplist[-2]) #倒数第二个

print("item 1 to 3 is", shoplist[1:3]);
print("item 2 to end is", shoplist[2:]);
print("item 1 to -1 is", shoplist[1:-1]);
print("item start to end is", shoplist[:]);

name = "swaroop"
print("characters 1 to 3 is", name[1:3])
print("characters 2 to end is", name[2:])
print("characters 1 to -1 is", name[1:-1])
print("characters start to end is", name[:])


'''
print("i have", len(shoplist), "items to purchase.")
print("This items are:")
for item in shoplist:
    print(item)
print ("\nalso have to buy rice.")
shoplist.append("rice2")

print("my shopping list is now", shoplist)
print("i will sort my list now")
shoplist.sort(key=None, reverse=False)
print("my sorted shopping list is now", shoplist)

print("the first item i wil buy is", shoplist[0])
olditem = shoplist[0]
del olditem
print("i bought the", shoplist[0])
print("my shoppint list is now", shoplist)
'''