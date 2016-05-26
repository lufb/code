
def safe_float(obj):
    try:
        retval = float(obj)
    except ValueError:
        retval = "could not convert non-number to float"
    except TypeError:
        retval = "object type cannot be converted to float"
    
    return retval


def safe_float2(obj):
    try:
        retval = float(obj)
    except (ValueError, TypeError):
        retval = "could not convert non-number to float"
    
    return retval

def safe_float3(obj):
    try:
        retval = float(obj)
    except BaseException:               #所有异常
        retval = "convert error"
    
    return retval


try:
    float("abc123")
except:
    import sys
    exc_tuple = sys.exc_info()
    
print(exc_tuple)