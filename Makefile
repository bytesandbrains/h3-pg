# this file only exists to support pgxnclient

all: cmake
	cmake --build build

cmake:
	cmake -S . -B build

install:
	cmake --install build --component extension

installcheck:
	cmake --build build --target installcheck

.PHONY: cmake