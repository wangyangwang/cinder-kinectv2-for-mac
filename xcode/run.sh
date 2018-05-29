#!/usr/bin/env bash

pushd ./build/Debug/*.app/Contents/MacOS/
LIBFREENECT2_PIPELINE=cl ./basic
popd
