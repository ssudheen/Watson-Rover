#!/bin/bash
cd ~/Watson/Navigation

if [ $1 -eq 1 ]
then
	echo "close up mode"
	#raspistill -w 1555 -h 1166 -o tempImg.jpg
	fswebcam -r 1155x1166 --no-banner tempImg.jpg
	./search_pattern_close_v0.1 $1
	exit 0
fi

fswebcam -r 640x480 --no-banner tempImg.jpg
./search_patternv0.1 $1


# Save image file - for demo purpose
#dt=`date +%M%S`
#fname=tempImg.$dt.jpg
#mv tempImg.jpg ./archive/$fname

exit 0







