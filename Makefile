PYTHON_VER=2
DOCKER_DEPS=docker-deps-py$(PYTHON_VER)
CONTAINER_LABEL=texpp:python$(PYTHON_VER)

ifeq ($(DEBUG), 1)
  BUILD_DIR=build-py$(PYTHON_VER)-debug
  BUILD_TYPE=Debug
  RESULTS_DIR=results/$(shell uname)-py$(PYTHON_VER)-debug/
else
  BUILD_DIR=build-py$(PYTHON_VER)
  BUILD_TYPE=Release
  RESULTS_DIR=results/$(shell uname)-py$(PYTHON_VER)/
endif

ifeq ($(PYTHON_VER), 2)
  PYTHON_PREFIX=$(shell python-config --prefix)
else
  PYTHON_PREFIX=$(shell python$(PYTHON_VER)-config --prefix)
endif

.PHONY: docker-build docker-run clean build version

help:
	@echo "Use \`make <target>\` with one of targets (you can override python" \
		  "version using \`make <target> PYTHON_VER=3\`):"
	@echo "  build         build right here"
	@echo "  clean         clean previous build"
	@echo "  docker-build  build container"
	@echo "  docker-run    start container to build new version"
	@echo "  version       check python versions"
	@echo "\nAdd \`DEBUG=1\` to make a debug build."

clean:
	rm -rf build-py2
	rm -rf build-py3
	rm -rf docker-deps-py2
	rm -rf docker-deps-py3

build:
	cp CMakeListsFolder/CMakeListsPython$(PYTHON_VER).txt CMakeLists.txt
	@mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) \
		&& cmake -DPYTHON_EXECUTABLE=`which python$(PYTHON_VER)` \
			-DPYTHON_LIBRARY=$(PYTHON_PREFIX)/Python \
			-DPYTHON_INCLUDE_DIR=$(PYTHON_PREFIX)/Headers \
			-DICU_ROOT_DIR=/usr/local/opt/icu4c \
			-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) .. \
		&& make
	@mkdir -p $(RESULTS_DIR)
	cp $(BUILD_DIR)/hrefkeywords/_chrefliterals.so $(BUILD_DIR)/texpy/texpy.so $(RESULTS_DIR)

docker-build:
	@mkdir -p $(DOCKER_DEPS)
	cp Dockerfile-py$(PYTHON_VER) $(DOCKER_DEPS)/Dockerfile
	cp -r CTestConfig.cmake docker-build.sh hrefkeywords tests texpp texpy $(DOCKER_DEPS)
	docker build -t $(CONTAINER_LABEL) $(DOCKER_DEPS)

docker-run:
	cp CMakeListsFolder/CMakeListsPython$(PYTHON_VER).txt $(DOCKER_DEPS)/CMakeLists.txt
	docker run -it --rm --volume="$(CURDIR)/$(DOCKER_DEPS):/code" \
						--volume="$(CURDIR)/results/Linux-py$(PYTHON_VER):/results" \
			$(CONTAINER_LABEL) #/code/docker-build.sh

version:
	@echo PYTHON_VER=$(PYTHON_VER)
	@echo BUILD_DIR=$(BUILD_DIR)
	@echo CONTAINER_LABEL=$(CONTAINER_LABEL)
	@echo RESULTS_DIR=$(RESULTS_DIR)
	@echo PYTHON_PREFIX=$(PYTHON_PREFIX)
