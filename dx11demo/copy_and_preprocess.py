import sys
import re
import os
 
inputfilepath = sys.argv[1]
outputdir = sys.argv[2]
filename = os.path.basename(inputfilepath)
inputfile = open(inputfilepath,'r+')
outputfile = open(outputdir + filename,'w+')
names=["View.", "Primitive.", "DrawRectangleParameters.", "DeferredLightUniform.",]
print "preprocessing..." + inputfilepath
for line in inputfile.readlines():
    for name in names:
        line=line.replace(name, "")
    outputfile.write(line)


inputfile.close()
outputfile.close()