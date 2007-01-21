#!/bin/sh
aclocal
libtoolize
autoconf
automake --foreign --add-missing
