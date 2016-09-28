#!/bin/sh

cmake -DCMAKE_BUILD_TYPE=Release /code
make
cp texpy/texpy.so /results/
[[ -e hrefkeywords/_chrefliterals.so ]] && cp hrefkeywords/_chrefliterals.so /results/
mkdir texpylib
touch texpylib/__init__.py
cp texpy/texpy.so texpylib
python$PYTHON_VERSION /code/_setup.py bdist_wheel
cp dist/*.whl /results
