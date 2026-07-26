#!/bin/bash

if [ ! -d "/tmp/my_test_dir" ]; then 
	echo "Folder is missing!"; 

fi

sleep 30 &
echo "The sleep PID is $!"

count=1; until [ $count -eq 5 ]; do echo "Count is $count"; count=$((count + 1)); sleep 1 ; done ; echo "Finished!"
