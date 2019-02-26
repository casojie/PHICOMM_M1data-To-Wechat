#!/bin/bash
today=`date +"%Y-%m-%d"`
content=`cat m1_airdata.txt | grep -v "^$"`
curl "https://sc.ftqq.com/$1.send?text=${today}" -d "&desp=${content}"
sed -i '3,$d' m1_airdata.txt