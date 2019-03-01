#!/bin/bash
today=`date +"%Y-%m-%d"`
content=`cat release/m1_airdata.txt | grep -v "^$"`
curl "https://pushbear.ftqq.com/sub?sendkey=$1&text=${today}" -d "&desp=${content}"
sed -i '3,$d' release/m1_airdata.txt