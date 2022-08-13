#!/bin/bash

SCRIPT_DIR=$(cd $(dirname $0); pwd)
CUR_DIR=$(pwd)

# build g2o
cd ${SCRIPT_DIR}/g2o/ && mkdir build
cd build
cmake -GNinja -DCMAKE_INSTALL_PREFIX=${SCRIPT_DIR}/g2o/build/install -DCMAKE_BUILD_TYPE=Release ..
ninja && ninja install
cd ${CUR_DIR}

# build pcl
cd ${SCRIPT_DIR}/pcl/ && mkdir build
cd build
cmake -GNinja -DCMAKE_INSTALL_PREFIX=${SCRIPT_DIR}/pcl/build/install -DCMAKE_BUILD_TYPE=Release ..
ninja && ninja install
cd ${CUR_DIR}
