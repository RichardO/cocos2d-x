#!/bin/bash
# This script will perform a clean raspberrypi build of all targets in both
# debug and release configurations.  It will also ensure that all the required
# packages are installed.  For day-to-day work on the raspberrypi port it is
# faster/better to simply use 'make' either at the top level or in the subproject
# you are working on.

# Exit of first error.
set -e

# Change directory to the location of this script
cd $(dirname ${BASH_SOURCE[0]})

[ -z "$COCOS2DX_USEAPT" ] && COCOS2DX_USEAPT=true

if $COCOS2DX_USEAPT; then
    ./install-deps-raspberrypi.sh
fi

export MAKEFLAGS=-j10

make PLATFORM=raspberrypi DEBUG=1 clean -f Makefile.raspberrypi
make PLATFORM=raspberrypi DEBUG=0 clean -f  Makefile.raspberrypi

make PLATFORM=raspberrypi DEBUG=1 all -f Makefile.raspberrypi
make PLATFORM=raspberrypi DEBUG=0 all -f Makefile.raspberrypi

