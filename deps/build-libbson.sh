#! /bin/sh
#
# build-libbson.sh
# Copyright (C) 2017 denghongcai <denghongcai@sais-MacBook-Pro.local>
#
# Distributed under terms of the MIT license.
#

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd $DIR/libbson

./autogen.sh
./configure --enable-static=yes --prefix=$DIR/libbson/dists && make -j4 && make install
