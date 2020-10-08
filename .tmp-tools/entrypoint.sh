#!/usr/bin/env sh
set -e

sudo -u postgres mkdir build

service postgresql start

cd build

# first regular cmake
sudo -u postgres cmake -S ..
sudo -u postgres cmake --build .
cmake --install . --component extension # sudo
sudo -u postgres ctest --output-on-failure

cd ..
rm -rf build

# also try makefile pgxn wrapper
#sudo -u postgres make all
#make install # sudo
#sudo -u postgres make test
