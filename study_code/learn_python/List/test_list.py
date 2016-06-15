res = [c*4 for c in "SAPM"]
print(res)

res = []
for c in "SAPM":
    res.append(c*4)

print(list(map(abs, [-1,-2,0,1,2])))

L = ["sapm", "Spam", "SPAM!"]
L[1:2] = []
print(L)
L.sort()