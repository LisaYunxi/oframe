#!/bin/bash

echo build version is: [$1]

echo '#define INNER_BASE_VERSION' \"$1\" > ../src_base/function/versionValue.h
echo '#define INNER_DUE_DATE' \"20240630\" >> ../src_base/function/versionValue.h

thispath=`pwd`
if [ -f ../libs/setstatic.sh ]
then
	cd ../libs
	chmod u+x setstatic.sh
	./setstatic.sh
	cd $thispath
fi

#编译所有的模块
for project in `ls -F | grep "/$"| awk -F / '{print $1}'`
do 
	cd $project
	if [ -f makefile.linux ]
	then
		echo make $project
		make -j3 -f makefile.linux 1>output 2>&1
		grep error: output
	fi
	if [ -f build.sh ]
	then
		echo build $project
		chmod u+x build.sh
		./build.sh
	else
		for project in `ls -F | grep "/$"| awk -F / '{print $1}'`
		do 
			cd $project
			if [ -f makefile.linux ]
			then
				echo make $project
				make -j3 -f makefile.linux 1>output 2>&1
				grep error: output
			fi
			if [ -f build.sh ]
			then
				echo build $project
				chmod u+x build.sh
				./build.sh
			fi	
			cd ..
		done 
	fi	
	cd ..
done 

