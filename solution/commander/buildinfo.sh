#!/bin/bash

src=src
bin=bin
name=commander
curdir=`pwd`
commonflags='-g -I '$curdir'/../boostlib'
mainflags="-lpthread "$commonflags
#			pathnamesource-file(s)...
i=0
objects[$i]='../../common/src/logger logger'
flags[$i]=$commonflags
i=$i+1
objects[$i]='../../common/src/sender sender'
flags[$i]=$commonflags
i=$i+1
objects[$i]='../../common/src/receiver receiver'
flags[$i]=$commonflags
i=$i+1
objects[$i]='../../common/src/common common'
flags[$i]=$commonflags
i=$i+1
objects[$i]='commander commander'
flags[$i]=$commonflags
