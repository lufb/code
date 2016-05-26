import string

alphas  = string.ascii_letters+"_"
nums    = string.digits
other   = alphas + nums

print("Welcome to the idcheck V1.0")
myinput = input("Input id to check")

if len(myinput) > 1:
    if myinput[0] not in alphas:
        print("invalid: first symbol must be alphas")
    else:
        for otherChar in myinput[1:]:
            if otherChar not in other:
                print("invalid:remainint symbols must be other")
                break;
        else:
                print("okay")
else:
    print("little len")
