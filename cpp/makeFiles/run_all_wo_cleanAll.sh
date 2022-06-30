#!/bin/bash

#---------------------------------------
# vn cpp sdk is made by cmake files at
# C:\Users\wus1\Projects\2021\pnt\debianEnv\pkg\vnproglib\cpp\build
# then manually copied to pnt/build/libs

# make -f Makefile_vn_sdk.mak -j 3
# make -f Makefile_is_sdk2.mak -j 3

make -f Makefile_util.mak -j 3
make -f Makefile_mat.mak  -j 3
make -f Makefile_test.mak  -j 3
