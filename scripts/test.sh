#!/bin/bash
set -e

DISTRIBUTION=$1

BASEDIR=$(dirname $(realpath "$0"))
cd $BASEDIR

ARCH=i386

cd ../.github/tools
docker build --build-arg ARCH=$ARCH -t h3-pg:develop .

cd ../..
docker run --rm -v "$PWD":/github/workspace h3-pg:develop