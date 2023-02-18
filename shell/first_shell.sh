#!/bin/bash
str="Hello World!"
echo $str
sum=0
for i in {1..100}; do
	((sum+=$i))
done
echo $sum
for i in {0..5}; do
	ping -c 2 -i 0.5 192.168.93.$i &>/dev/null
	if [ $? -eq 0 ]; then
		printf "192.168.93.$i is up\n"
	else
		echo "192.168.93.$i is down"
	fi
done
ip=`ifconfig -a|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'`
echo $ip
