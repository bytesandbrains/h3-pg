#!/usr/bin/env sh
set -e

service postgresql start

# first regular cmake
sudo -u postgres cmake -S . -B build
sudo -u postgres cmake --build build
cmake --install build --component extension # sudo
sudo -u postgres cmake --build build --target installcheck

rm -rf build

# also try makefile pgxn wrapper
sudo -u postgres make all
make install # sudo
sudo -u postgres make installcheck
