from AssetBase import *

class JSON(Asset):
    def __init__(self):
        Asset.__init__(self)
        self.metaData["type"] = "json"

        self.acceptedFileExtensions = ["json"]

        self.requiresMeta = False
        self.isJson = True

if __name__ == "__main__":
    pass
