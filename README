The original project can be found here: [Texpp on the Google Code](http://code.google.com/p/texpp/)

# Texpp

Texpp is a threefold C++ library with a Python 3.4 interface containing:

* Stemmer
* Terms extractor
* TeX parser

Each part offers a set of Python API methods. Internally the library is tightly binded and reuses all components for efficient terms extraction from TeX documents (while being perfectly capable of working with any text sources).

*TODO: describe pythonic API methods and illustrate with examples*

## Installation


### Requirements:


```bash
  sudo apt-get update
  sudo apt-get install git gcc g++ cmake libicu-dev python3.4-dev
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
  ./bootstrap.sh --with-python=/usr/bin/python3 --with-python-root=/usr --prefix=/usr/local
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
  git checkout portingToPython3
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  make
```

To run library tests also type 

```bash
  make tests
```

*Note: many tests are failing at the moment. Working on it*

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
