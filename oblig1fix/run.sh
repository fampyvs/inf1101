#!/bin/bash
# Author: Odin Bjerke <odin.bjerke@uit.no>

# -- Usage --
# This script runs one of the specified binary targets, prioritizing the release build if one exists.
# The development (debug) build is run with the --test flag, in addition to input filepath
#
# Note that this script will not compile any binary target for you. Remember to re-run `make` for any 
# source codechanges to register.
# -----------

# run the app with this file as input
FPATH_INPUT="data/oxford_dict.txt"

# binary target(s)
BIN_TARGET="wordfreq"
BIN_DEBUG="bin/debug/$BIN_TARGET"
BIN_RELEASE="bin/release/$BIN_TARGET"

# name of this script; to make it distinct if the output is from the binary or this script
SELF_NAME="$(basename "$0")"

if [ -f $BIN_RELEASE ]; then
    if [ -f $BIN_DEBUG ]; then
        echo "$SELF_NAME: Found multiple binary targets. Defaulting to release build at $BIN_RELEASE."
    fi
    echo "$SELF_NAME: Running release build with $FPATH_INPUT as input."
    exec $BIN_RELEASE --fpath=$FPATH_INPUT
elif [ -f $BIN_DEBUG ]; then
    echo "$SELF_NAME: Running development build with tests enabled and $FPATH_INPUT as input."
    exec $BIN_DEBUG --test --fpath=$FPATH_INPUT
else
  echo "$SELF_NAME: Error - No binary target found. Compile with make before running this script."
  exit 1
fi
