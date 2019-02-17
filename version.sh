#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

BINARY=WS2801_Arduino_BitBanger
VERSION=`git describe --always --dirty`
DATE=`date -u +%FT%T%z`
echo "${BINARY}: compiled at ${DATE} with version ${VERSION}"

echo "// version.hpp
#ifndef VERSION_HPP
#define VERSION_HPP

#define BUILD_DATE    ${DATE}
#define BUILD_VERSION ${VERSION}
#define BUILD_PROGRAM ${BINARY}

#endif
" > ${DIR}/version.hpp
