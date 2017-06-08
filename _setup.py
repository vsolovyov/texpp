#!/usr/bin/env python

from setuptools import setup, Distribution


class BinaryDistribution(Distribution):
    def has_ext_modules(self):
        return True


setup(
    name='texpy',
    description='',
    url='https://github.com/vsolovyov/texpp',
    version='2.4.1',
    packages=['texpylib'],
    package_data={
        'texpylib': ['texpy.so'],
    },
    distclass=BinaryDistribution,
)
