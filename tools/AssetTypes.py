from AssetTexture import *
from AssetMesh import *
from AssetGLSL import *
from AssetJSON import *
from AssetLogic import *

def GetAssetType(postfix):
    if postfix in Texture().acceptedFileExtensions:
        return Texture()

    if postfix in Mesh().acceptedFileExtensions:
        return Mesh()

    if postfix in GLSL().acceptedFileExtensions:
        return GLSL()

    if postfix in JSON().acceptedFileExtensions:
    	return JSON()

    if postfix in Logic().acceptedFileExtensions:
    	return Logic()

if __name__ == "__main__":
    pass
