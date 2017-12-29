from AssetBase import *

class Logic(Asset):
    def __init__(self):
        Asset.__init__(self)
        self.metaData["type"] = "logic"
        self.requiresMeta = False
        self.isLogic = True

        self.acceptedFileExtensions = ["hpp"]

if __name__ == "__main__":
    pass
