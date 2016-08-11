#!/bin/sh

cmake -DCMAKE_BUILD_TYPE=Release /code
make
cp hrefkeywords/_chrefliterals.so /results/
cp texpy/texpy.so /results/
