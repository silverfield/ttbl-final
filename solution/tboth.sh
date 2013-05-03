#!/bin/bash

dobuild=true
args=$@
if [ $# -ge 1 ] && [ "$1" = "-s" ]
then
	dobuild=false
	args=${@:2}
fi

if $dobuild
then
	./tcomp.sh
else
	./tcomp.sh "-s"
fi

./trun.sh $args
