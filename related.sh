#!/bin/sh
wget -t 10 -w 60 -T 30 -O ori.html $1
grep ref=related ori.html >temp_sug.html
echo "...related html has download!..."
