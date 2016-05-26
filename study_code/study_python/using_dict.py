ab = {    "lufubo":23,
        "chengming":34,
        "yangg":45}
print("lufubo's age %s" % ab["lufubo"])
ab["lufubo"] = 99
print("lufubo's age %s" % ab["lufubo"])

print("ab len %d" % len(ab))
#del ab["lufubo"]
print("ab len %d" % len(ab))

for name, age in ab.items():
    print("name %s age:%d" % (name, age))
print("\n")

new_name = "lufubo"
if new_name in ab:
    print("%s's address is %s" % (new_name,ab["lufubo"]))