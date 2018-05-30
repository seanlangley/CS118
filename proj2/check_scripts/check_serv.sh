#!/bin/bash
./server
diff input.txt output.txt >> output.diff
FILESIZE=$(stat -f%z output.diff)
if [ $FILESIZE -eq 0 ]
then
	echo "Files transmitted correctly"
else
	echo "Files differ after transmission"
fi
