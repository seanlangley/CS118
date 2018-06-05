#!/bin/bash
./src/server
printf "\n*****RUNNING CHECK SCRIPT*****\n"
diff doge.jpg output.jpg >> output.diff
FILESIZE=$(stat -f%z output.diff)

if [ $FILESIZE -eq 0 ]
then
	printf "Files transmitted correctly\n"
	rm output.diff
else
	printf "Files differ after transmission, stored in output.diff\n"

fi
