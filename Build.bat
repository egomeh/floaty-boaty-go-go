mkdir distribution

cd distribution
cmake -G"Visual Studio 14 2015 Win64" -DSHADER_DEBUG_INFO=OFF -DDO_ONLY_GUI=ON -DCHECK_OPENGL_ERRORS=OFF -DPACK_ASSETS=ON -DMULTICORE_COMPILE=ON ..
MSBuild.exe FloatyBoatyGoGo.sln /t:Build /p:Configuration=Release
cd ..
