#!/bin/bash

set -e

mkdir -p ./vendor

if [ ! -d "./vendor/raylib" ]; then
    git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git ./vendor/raylib
fi
