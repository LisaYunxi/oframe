#!/bin/bash

for project in `ls -F | grep "/$"| awk -F / '{print $1}'`
do 
	cd $project
	if exist list.xml
	then
		echo gen list $project
		genprj list.xml 1>output 2>&1
	fi
	if exist gen.sh
	then
		echo gen $project
		rm -rf makefile*
		chmod u+x gen.sh
		./gen.sh
	else
		for project in `ls -F | grep "/$"| awk -F / '{print $1}'`
		do 
			cd $project
			if exist list.xml
			then
				echo gen list $project
				genprj list.xml 1>output 2>&1
			fi
			if exist gen.sh
			then
				echo gen $project
				rm -rf makefile*
				chmod u+x gen.sh
				./gen.sh
			cd ..
		done
	fi
	cd ..
done 
