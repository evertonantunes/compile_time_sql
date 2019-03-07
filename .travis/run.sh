#!/bin/bash

set -e
set -x

if [[ "$(uname -s)" == 'Darwin' ]]; then
    if which pyenv > /dev/null; then
        eval "$(pyenv init -)"
    fi
    pyenv activate conan
fi

conan remote clean
conan remote add conan_center https://conan.bintray.com
conan remote add remote_repository $REPOSITORY --insert
conan user $USER -r remote_repository -p $API_KEY

conan install . -s compiler.libcxx=libstdc++11 --build=missing
conan upload "*" --all -r remote_repository -c

mkdir -p ./build
cd ./build

cmake ..
cmake --build . --target all --config Release
cmake --build . --target test --config Release

