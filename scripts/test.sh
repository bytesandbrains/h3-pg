#!/bin/bash
set -e

DISTRIBUTION=$1

BASEDIR=$(dirname $(realpath "$0"))
cd $BASEDIR

cd ../.github/actions/test
docker build -t h3-pg:develop .

cd ../../..
docker run --rm -v "$PWD":/github/workspace h3-pg:develop $DISTRIBUTION