'''
使用元组来作为参数：
def functin_name([formal_args,] *vargs_tuple)
'''
def tuplevarargs(arg1, arg2="defaultB", *theOther):
    "dispay regular args and non-keyword variable"
    "元组参数  *vargs_tuple"
    print("formal arg1:%s" % arg1)
    print("formal arg2:%s" % arg2)
    for each in theOther:
        print ("another arg: %s" % each)

'''
使用变量参数字典来作为参数
def functon_name([formal_args, ][*vargst,] **vargsd):
'''
def dictvarargs(arg1, arg2 = "default", **other):
    'display 2 regular args and keyword variable args'
    print("formal arg1:%s" % arg1)
    print("formal arg2:%s" % arg2)
    for each in other.keys():
        print ("another arg: %s :%s" % (each, str(other[each])))
        

def newfoo(arg1, arg2, *nkw, **kw):
    'display regular args and all variable args'
    print("formal arg1:%s" % arg1)
    print("formal arg2:%s" % arg2)
    for eachnkw in nkw:
        print("addition non-keyword arg:%s" % eachnkw)
    for eachkw in kw.keys():
        print("additional keyword args: '%s': '%s'" %(eachkw, str(kw[eachkw])))
   
newfoo("wolf", 3, "projects", zreud = 90, gamble = 60)     
print("#################")

newfoo(2, 4, *(6,8), **{"foo":10, "bar":12})     
print("#################")

'''
dictvarargs("abc")
print("-------------")

dictvarargs("abc", "def")
print("-------------")

dictvarargs(arg1 = "abc", arg2 = 123, c = 456, d = 789)
print("-------------")

dictvarargs(arg1 = "abc", arg2 = 123, d =("hello", "world"), c = 456)
print("-------------")
'''
