#!/bin/bash

set -e

mkdir -p ./vendor

if [ ! -d "./vendor/raylib" ]; then
    git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git ./vendor/raylib
fi

if [ ! -d "./vendor/cglm" ]; then
    git clone --depth 1 --branch v0.9.6 https://github.com/recp/cglm.git ./vendor/cglm
fi

if [ ! -d "./vendor/googletest" ]; then
    git clone --depth 1 --branch v1.17.0 https://github.com/google/googletest.git ./vendor/googletest
fi

if [ ! -d "./vendor/benchmark" ]; then
    git clone --depth 1 --branch v1.9.5 https://github.com/google/benchmark.git ./vendor/benchmark
fi
