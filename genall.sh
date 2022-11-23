#!/bin/bash

for project in `ls -F | grep build | grep "/$"| awk -F / '{print $1}'`
do 
	cd $project
	if [ -f gen.sh ]
	then
		echo gen $project
		chmod u+x gen.sh
		./gen.sh
	fi
	cd ..
done