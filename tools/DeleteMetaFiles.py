import os
import sys

def DeleteMetaFiles(_, directory, files):
    for file in files:
        fullPath = os.path.join(directory, file).replace("\\","/")

        seperatorSplit = fullPath.split(".")

        if len(seperatorSplit) > 0 and seperatorSplit[-1] == "meta":
            print "Removing " + fullPath
            os.remove(fullPath)

def Main(path):
    os.path.walk(path, DeleteMetaFiles, None)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        Main("./../assets/")
    else:
        print sys.argv[1]
        Main(sys.argv[1])