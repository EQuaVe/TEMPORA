set -e

cd "$(dirname $0)" 
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release  -DCMAKE_INSTALL_PREFIX=./build
cmake --build build -j 8
cmake --build build --target doc -j
cmake --install build
cmake --build build --target test -j
