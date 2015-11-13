#!/bin/bash

#
# Build all common Toolbox plugins
#


CPP=g++
C_FLAGS="-std=c++14 -Wall -pedantic"
LD_FLAGS="-fPIC -shared"

PLUGIN_EXT=".so"

PLUGIN_DIR="../Common"


for PLUGIN_TYPE in `ls -d */ | sed 's/\///'`; do

	for PLUGIN_SRC in `ls  ${PLUGIN_TYPE}/*.cpp 2> /dev/null`; do
		PLUGIN=`echo $PLUGIN_SRC | sed "s/${PLUGIN_TYPE}\///" | sed "s/\.cpp/${PLUGIN_EXT}/"`

		# Make sure the target directory exists
		if [ ! -e "${PLUGIN_DIR}/${PLUGIN_TYPE}" ]; then
			mkdir -p ${PLUGIN_DIR}/${PLUGIN_TYPE}
		fi

		echo "Building ${PLUGIN_TYPE}: ${PLUGIN}..."
		${CPP} ${C_FLAGS} -o ${PLUGIN_DIR}/${PLUGIN_TYPE}/${PLUGIN} ${PLUGIN_SRC} ${LD_FLAGS}
	done
done


