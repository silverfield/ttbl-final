#!/bin/bash

dobuild=true
makeargs=$@
if [ "$#" = "1" ] && [ "$1" = "-s" ]
then
	dobuild=false
	makeargs=${@:2}
fi

/home/ferrard/c/tl.sh
cd ttblazer
if $dobuild
then
	../depmake.sh buildinfo.sh > makefile
fi
make $makeargs
cd ../
