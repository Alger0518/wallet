#!/bin/bash

source ./config.sh

function reset()
{
	num=`ps -ef|grep "ScanDeposit --name=$1"|grep -v grep|grep -v PPID|awk '{ print $2}'|wc -l`
	if [ $num == 1 ];
	then
		processId=`ps -ef|grep "ScanDeposit --name=$1"|grep -v grep|grep -v PPID|awk '{ print $2}'`
		kill -9 $processId
	fi

	echo $resetScanPos > /usr/local/wallet/deposit/$1history.data

	mySQL_path=`which mysql`
	$mySQL_path -h$mysqlHost -u$mysqlUser -p$mysqlPwd -e"
	SET FOREIGN_KEY_CHECKS=0;
	use $mysqlDB;
	delete from $1_deposit; 
	update $1_address_pool set uid=0,used=0;
	SET FOREIGN_KEY_CHECKS=1;"

	cd /usr/local/wallet/deposit/

	datetime=`date +%Y%m%d%H%M%S`
	screen_name=$1$datetime
	screen -dmS $screen_name

	screen -x -S $screen_name -p 0 -X stuff "./ScanDeposit --name=$1 -daemon=0 -consolelog=1>>$1.log" 
	screen -x -S $screen_name -p 0 -X stuff "\n" 
	
	echo "compelete!"
}

arr=$(echo $resetCoin|tr "," "\n")

for line in ${arr[@]}
do
  reset $line
done
