mkdir lib

cd lib

git clone https://github.com/nlohmann/json
git clone https://github.com/g-truc/glm
git clone https://github.com/nothings/stb
git clone https://github.com/assimp/assimp
git clone https://github.com/Perlmint/glew-cmake
git clone https://github.com/Bl4ckb0ne/delaunay-triangulation

cd assimp
mkdir build
cd build
cmake -G"Visual Studio 14 2015 Win64" -DBUILD_SHARED_LIBS=OFF -DLIBRARY_SUFFIX="" -DCMAKE_CXX_FLAGS_RELEASE="/MT" ..
MSBuild.exe Assimp.sln /t:Build /p:Configuration=Release
MSBuild.exe Assimp.sln /t:Build /p:Configuration=Debug
cd ..
cd ..

cd glew-cmake
mkdir build
cd build
cmake -G"Visual Studio 14 2015 Win64" -DCMAKE_CXX_FLAGS_RELEASE="/MT" ..
MSBuild.exe glew.sln /t:Build /p:Configuration=Release
MSBuild.exe glew.sln /t:Build
cd ..
cd ..

cd ..
