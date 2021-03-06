#!/bin/bash
./src/server 21235
printf "\n*****RUNNING CHECK SCRIPT*****\n"
rm output.diff
diff doge.jpg output.jpg >> output.diff
FILESIZE=$(stat -f%z output.diff)

if [ $FILESIZE -eq 0 ]
then
	printf "Files transmitted correctly\n"
	rm output.diff
else
	printf "Files differ after transmission, stored in output.diff\n"

fi
