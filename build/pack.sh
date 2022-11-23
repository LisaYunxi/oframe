#!/bin/bash
tarstr=`ls -F */* | grep "*"`
tarstr1=${tarstr//\*//}
cp -r ../libs/ofserviceapi ./
for project in `ls -F | grep "/$"| awk -F / '{print $1}'`
do 
	cd $project
	if [ -f pack.sh ]
	then
		echo pack $project
		chmod u+x pack.sh
		./pack.sh
	fi
	cd ..
done


chmod u+x */*.so
tarstr=`find . -maxdepth 2 -name "*" -type f -executable | grep -v ".sh"`
tar -cvf bin.tar $tarstr ./ofserviceapi
gzip bin.tar

