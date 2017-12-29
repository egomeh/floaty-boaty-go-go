import json

class Asset:
    def __init__(self):
        self.metaData = {}
        self.acceptedFileExtensions = []
        self.requiresMeta = True
        self.isJson = False
        self.isLogic = False

    def GetJsonData(self):
        return json.dumps(self.metaData, indent=4, sort_keys=True)

    def ReadMetaData(self, externalMetaData):
        for key in self.metaData:
            if not key in externalMetaData:
                externalMetaData[key] = self.metaData[key]

        self.metaData = externalMetaData

if __name__ == "__main__":
    pass
