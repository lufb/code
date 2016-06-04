tmplate = "{0},{1} and {2}"
print(tmplate.format("lufubo", "he","eggs"))

template2 = "{motto}, {pork} and {food}"
result = template2.format(motto = "lufubo", pork = "love", food = "eggs")
print(result)

template3 = "{motto}, {0} and {food}"
result = template3.format("lufubo", motto = "love", food = "eggs")
print(result)

import sys
r1 = "my{1[spam]} runs {0.platform}".format(sys, {"spam":"laptop"})
print(r1)
r2 = "my {config[spam]} runs {sys.platform}".format(sys=sys,config = {"spam":"laptop"})
print(r2)

somelist = list("SAPM")
r3 = "first = {0[0]}, thried = {0[2]}".format(somelist)
print(r3)
r4 = "fist = {0}, third = {1}".format(somelist[0], somelist[2])
print(r4)
parts = somelist[0], somelist[-1], somelist[1:3]
print(parts)
r5 = "first = {0}, last={1}, middle={2}".format(*parts)
print(r5)

print("{0:b}".format((2**16)-1))