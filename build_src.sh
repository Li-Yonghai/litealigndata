rm -rf build
mkdir build
cd build
../third-party/stacks/cmake/usr/local/bin/./cmake ../
#just only compiling src code, if building all(include UT), not require cd src
#cd src
make -j4
cd ..
