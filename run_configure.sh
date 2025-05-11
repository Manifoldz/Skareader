#!/bin/bash

INSTALL_DIR=$PWD/install

aclocal
autoheader
autoconf
automake --add-missing

mkdir -p "$INSTALL_DIR"
./configure -prefix="$INSTALL_DIR"
