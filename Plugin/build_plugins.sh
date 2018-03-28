#!/bin/bash

#
# Build all common Toolbox plugins
#


CPP=g++
C_FLAGS="-std=c++14 -Wall -pedantic"
LD_FLAGS="-fPIC -shared"

PLUGIN_EXT=".so"


for PLUGIN_SRC in `for DIR in \`find . -type d | grep -v './example' | grep / | sed 's/^\.\///'\`; do ls $DIR/*.cpp; done`; do
	PLUGIN_TYPE=`echo $PLUGIN_SRC | sed 's/\/.*$//'`
	PLUGIN=`echo $PLUGIN_SRC | sed "s/${PLUGIN_TYPE}\///" | sed "s/\.cpp/${PLUGIN_EXT}/"`

	# Make sure the target directory exists
#	if [ ! -e "${PLUGIN_DIR}/${PLUGIN_TYPE}" ]; then
#		mkdir -p ${PLUGIN_DIR}/${PLUGIN_TYPE}
#	fi

	echo "Building ${PLUGIN_TYPE}: ${PLUGIN}..."

	# HACK: Make/use a better build system

	if [ "${PLUGIN}" == "OpenGL.so" ]; then
		${CPP} ${C_FLAGS} -o ${PLUGIN_TYPE}/${PLUGIN} ${PLUGIN_SRC} ${LD_FLAGS} -lGL
	elif [ "${PLUGIN}" == "SDL2.so" ]; then
		${CPP} ${C_FLAGS} -o ${PLUGIN_TYPE}/${PLUGIN} ${PLUGIN_SRC} ${LD_FLAGS} -lSDL2
	elif [ "${PLUGIN_TYPE}" == "Image" ] && [[ "${PLUGIN}" == "SDL2"* ]]; then
		${CPP} ${C_FLAGS} -o ${PLUGIN_TYPE}/${PLUGIN} ${PLUGIN_SRC} ${LD_FLAGS} -lSDL2_image
	elif [ "${PLUGIN_TYPE}" == "Network" ] && [ "`echo "${PLUGIN}" | grep "asio"`" != "" ]; then
		${CPP} ${C_FLAGS} -o ${PLUGIN_TYPE}/${PLUGIN} ${PLUGIN_SRC} ${LD_FLAGS} -lpthread
	elif [ "${PLUGIN_TYPE}" == "SQL" ] && [ "`echo "${PLUGIN}" | grep "libmysqlclient"`" != "" ]; then
		${CPP} ${C_FLAGS} -o ${PLUGIN_TYPE}/${PLUGIN} ${PLUGIN_SRC} ${LD_FLAGS} -lmysqlclient
	else
		${CPP} ${C_FLAGS} -o ${PLUGIN_TYPE}/${PLUGIN} ${PLUGIN_SRC} ${LD_FLAGS}
	fi
done


