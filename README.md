The original project can be found here: [Texpp on the Google Code](http://code.google.com/p/texpp/)

# Texpp

Texpp is a threefold C++ library with a Python interface containing:

* Stemmer
* Terms extractor
* TeX parser

Each part offers a set of Python API methods. Internally the library is tightly binded and reuses all components for efficient terms extraction from TeX documents (while being perfectly capable of working with any text sources).

*TODO: describe pythonic API methods and illustrate with examples*

## Installation

During TeXpp installation you should specify version of Python(between 2 and 3). Сurrent version of Python avaliable to install from repository is 2.7 and 3.4. So, the recipy of installation TeXpp for certain version of Python is a bit different. Presumably installation make under Ubuntu 14.04 Trusty Tahr.

### Requirements:

Here you can exclude one of Python version(Leave one of them: python3.4-dev or python2.7-dev).

```bash
  sudo apt-get update
  sudo apt-get install git gcc g++ cmake libicu-dev python3.4-dev python2.7-dev
  sudo apt-get install libboost-dev libboost-filesystem-dev libboost-regex-dev libboost-python-dev libboost-test-dev
```

Additionally, for testing purposes one can install also a TeX Live package (caution: it is an about **3 Gb** download)

```bash
  sudo apt-get install texlive-full 	# Required only for tests
```

### Boost library installation

For Ubuntu 14.04 there is no Boost package for Python 3, so we have to compile it from the source.

```bash
  wget -O boost_1_56_0.tar.gz http://sourceforge.net/projects/boost/files/boost/1.56.0/boost_1_56_0.tar.gz/download
  tar xzvf boost_1_56_0.tar.gz
  cd boost_1_56_0/
  sudo apt-get update
```
boost build configuration in case Python3:
```bash
  ./bootstrap.sh --with-python=/usr/bin/python3 --with-python-root=/usr --prefix=/usr/local
```
boost build configuration in case Python2:
```bash
  ./bootstrap.sh --with-python=/usr/bin/python2 --with-python-root=/usr --prefix=/usr/local
```
start Boost installation:
```bash
  sudo ./b2 --with=all install
  sudo sh -c 'echo "/usr/local/lib" >> /etc/ld.so.conf.d/local.conf'
  sudo ldconfig
```

Additionally, to make compilation faster, you can force `b2` to use maximum available amount of cores:

```bash
  # Count available cores
  n=`cat /proc/cpuinfo | grep "cpu cores" | uniq | awk '{print $NF}'`
  sudo ./b2 --with=all -j $n install
```

###	Building the library

```bash
  git clone https://github.com/Domest0s/texpp.git
  cd texpp
```

The CMakeLists.txt is different for different versions of Python, so you need to copy nesessary CMakeFiles.txt from CMakeListsFolder.
in case Python2 type:

```bash
  cp CMakeListsFolder/CMakeListsPython2.txt CMakeLists.txt
```

in case Python3 type:

```bash
  cp CMakeListsFolder/CMakeListsPython3.txt CMakeLists.txt
```

build:

```bash
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  make
```

To run library tests also type 

```bash
  export BOOST_TEST_CATCH_SYSTEM_ERRORS="no"
  make tests
```

###	Linking

Copy built libraries to the `/opt/texpp/` directory and register them in the system.

```bash
  sudo mkdir /opt/texpp
  sudo cp texpy/texpy.so /opt/texpp/
  sudo cp hrefkeywords/_chrefliterals.so /opt/texpp/
  echo "/opt/texpp" >> texpp.conf
  sudo mv texpp.conf /etc/ld.so.conf.d/
  sudo ldconfig
```
