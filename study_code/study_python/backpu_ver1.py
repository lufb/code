import os
import time

source = ["D:\pcre"]
target_dir = "D:\\pcre_back"
target = target_dir + time.strftime("%Y%m%d%H%M%S") + ".zip"
zip_command="zip -qr '%s' %s" %(target,' '.join(source))
print(zip_command)
if os.system(zip_command) == 0:
    print("successful backup to", target)
else:
    print("backup failed")


