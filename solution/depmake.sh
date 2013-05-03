#!/bin/bash

#prerequisites are these set variables (sourced from the file provided by argument):
# src
# bin
# objects - an array with each field another array of form [path, target, necessary source file 1, necessary source file 2, ...]
# flags - an array with each field being the flags for compiling
# name - name of the final application
# mainflags - flags for final binary

source "$1"

echo -n "src = $src/
bin = $bin/

objects = "

for object in "${objects[@]}"
do
	declare -a temp=($object)
	echo -n "$bin/${temp[1]}.o "
done

echo -n "
$name : \$(objects)
	g++ -o $bin/$name \$(objects)"
if [ "$mainflags" != "_" ]
then
	echo -n " $mainflags"
fi
echo "

"

for (( k = 0; k < ${#objects[@]}; k++ ))
do
	object=${objects[$k]}
	flag=${flags[$k]}
	declare -a temp=($object)
	echo -n "$bin/"
	if [ "$flag" != "_" ]
	then
		g++ -M $src/${temp[0]}.cpp $flag
	else
		g++ -M $src/${temp[0]}.cpp 
	fi
	echo -n "	g++ -c -o $bin/${temp[1]}.o $src/${temp[0]}.cpp "
	for (( i = 2 ; i < ${#temp[@]} ; i++ ))
	do
		 echo -n "$src/${temp[$i]}.cpp "
	done
	if [ "$flag" != "_" ]
	then
		printf "%s\n\n" "$flag"
	else
		printf "\n\n"
	fi

done


echo -n ".PHONY : clean
clean : 
	rm $bin/*

.PHONEY : rebuild
rebuild :
	rm $bin/*
	make
"

exit

ttblazer : $(objects)
	g++ -o $(bin)ttblazer $(objects) read

$(bin)logger.o : $(src)logger.cpp $(src)common.h
	g++ -c -o $(bin)logger.o $(src)logger.cpp
$(bin)common.o : $(src)common.cpp $(src)logger.h
	g++ -c -o $(bin)common.o $(src)common.cpp
$(bin)main.o : $(src)main.cpp $(src)common.h $(src)logger.h
	g++ -c -o $(bin)main.o $(src)main.cpp
$(bin)cmdproc.o : $(src)cmdproc.cpp $(src)common.h $(src)logger.h
	g++ -c -o $(bin)cmdproc.o $(src)cmdproc.cpp
$(bin)communicator.o : $(src)communicator.cpp $(src)common.h $(src)logger.h
	g++ -c -o $(bin)communicator.o $(src)communicator.cpp -lpthread 

.PHONY : clean
clean :
	rm $(bin)*


