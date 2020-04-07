#!/bin/bash
set -e

BASEDIR=$(dirname $(realpath "$0"))
cd $BASEDIR

REPOSITORY="docker.pkg.github.com/bytesandbrains/h3-pg"

docker build -t $REPOSITORY/test:amd64 --build-arg ARCH=amd64 ./docker
docker build -t $REPOSITORY/test:i386  --build-arg ARCH=i386  ./docker

docker push $REPOSITORY/test:amd64
docker push $REPOSITORY/test:i386