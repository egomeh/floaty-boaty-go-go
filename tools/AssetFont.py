from AssetBase import *

class Font(Asset):
    def __init__(self):
        Asset.__init__(self)
        self.metaData["type"] = "ttf"

        self.acceptedFileExtensions = ["ttf"]

        self.requiresMeta = False
        self.isJson = False

if __name__ == "__main__":
    pass
