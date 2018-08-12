#!/bin/sh
wget -t 10 -w 60 -T 30 -O temp_pd.html $1
echo "...pulldown html has download!..."

