from AssetBase import *

class Texture(Asset):
    def __init__(self):
        Asset.__init__(self)
        self.metaData["type"] = "texture"
        self.metaData["filtermode"] = "bilinear"
        self.metaData["wrapmode"] = "clamp"
        self.metaData["format"] = "RGBA32"
        self.metaData["anisotropic"] = 1
        self.metaData["mipmaps"] = True
        self.metaData["flipy"] = True

        self.acceptedFileExtensions = ["jpg", "png"]

if __name__ == "__main__":
    pass
