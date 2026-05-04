FROM ubuntu
MAINTAINER litealigndata litealigndata@email.com
WORKDIR /root
sudo apt install make -y
sudo apt-get install pkg-config
sudo apt-get install  gcc-8  g++-8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 10
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 20
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 10
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 20
sudo update-alternatives --display g++
sudo update-alternatives --display gcc
