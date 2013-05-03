#!/bin/bash

cd commander
../depmake.sh buildinfo.sh > makefile
make $@
cd ..
