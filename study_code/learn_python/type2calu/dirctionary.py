D = {"food":"spam", "quantiey":4, "color":"pink"}
print(D)

print(D["food"])
D["color"] = 1
print(D)

Dirctionary = {}
Dirctionary["name"] = "lufubo"
Dirctionary["age"] = 29
print(Dirctionary)

rec = {"name":{"first":"lu", "last":"fubo"},
       "job":["dev", "dev"],
        "age":29
}
rec["job"].append("software enginer")
print(rec)







square1 = [x ** 2 for x in[1,2,3,4,5]]
print(square1)

square2 = []
for x in [1,2,3,4,5]:
    square2.append(x**2)
print(square2)


D1 = {"a1":1,"b":2,"c":3,"d":4}
for key in sorted(D1):
    print(key," => " , D1[key])

if "a" in D1:
    print(D1["a"])
else:
    print("missing")
value = D1.get("a1", 0)
print(value)
