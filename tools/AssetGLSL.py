from AssetBase import *

class GLSL(Asset):
    def __init__(self):
        Asset.__init__(self)
        self.metaData["type"] = "glsl"

        self.acceptedFileExtensions = ["glsl"]

        self.requiresMeta = False
        self.isJson = False

if __name__ == "__main__":
    pass
