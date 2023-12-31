#!/usr/bin/env python3

import os
import sys
import getpass
import shutil

#!/usr/bin/env python3

import os
import sys
import getpass
import shutil

comp_path = sys.path[0]
print("comp_path:")
print(comp_path)

# original folder
org_image_path = comp_path + "/resources/image"
org_font_path = comp_path + "/resources/font"

# new folder
data_path = comp_path + "/../../hardware/chip/haas1000/prebuild/data/data"
image_path = data_path + "/ai_demo_image"
font_path = data_path + "/font"

# delete prebuild/data resources
if os.path.exists(image_path):
    print ('Delete /data/ai_demo_image firstly')
    shutil.rmtree(image_path)

if os.path.exists(font_path):
    print ('Delete /data/font firstly')
    shutil.rmtree(font_path)

# copy resources
shutil.copytree(org_image_path, image_path)
shutil.copytree(org_font_path, font_path)

# result
print("run external script success")
