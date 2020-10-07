#!/usr/bin/env bash
set -e
set -u

rm -rf build

docker build -t tmp --file .tmp-tools/Dockerfile .
docker run --rm -v "$PWD":/github/workspace tmp
