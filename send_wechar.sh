#!/bin/bash
today=`date +"%Y-%m-%d"`
content=`cat m1_airdata.txt | grep -v "^$"`
curl "https://sc.ftqq.com/SCU34310T59d5701493a443f694e73757af5989f25bc72eff8281e.send?text=${today}" -d "&desp=${content}"
sed -i '3,$d' m1_airdata.txt