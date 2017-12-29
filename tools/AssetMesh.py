
from AssetBase import *

class Mesh(Asset):
    def __init__(self):
        Asset.__init__(self)
        self.metaData["type"] = "mesh"
        self.metaData["scale"] = 1.
        self.metaData["flipz"] = True
        self.metaData["normals"] = "import"

        self.acceptedFileExtensions = ["fbx", "obj"]

if __name__ == "__main__":
    pass

