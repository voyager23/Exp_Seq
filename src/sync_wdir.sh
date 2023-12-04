#!/bin/bash

# Set hosts to syncronise
#hosts="host144 host143 host142 host141 fedora39"

hosts="inteli7"	# local group

# Set directory path to syncronise
dir="/home/mike/Wdir/OpenMpi"

for host in $hosts
do
	# test for active host
	ping -c1 -w2 $host &> /dev/null
	if [ $? -eq 0 ]
	then
		rsync -av $dir/ mike@$host:$dir &>/dev/null
		if [ $? -ne 0 ]
		then
			echo "$host - Failed to sync."
		else
			echo "$host - syncronized."
		fi
	else
		echo "$host - No ping."
	fi
done
