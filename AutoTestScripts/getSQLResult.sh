#!/bin/bash

source ./config.sh

#function getResutFromSQL()
#{
	$mySQL_path -h$mysqlHost -u$mysqlUser -p$mysqlPwd $mysqlDB -e"
	select txid,amount,address,vout from $1_deposit into outfile '/opt/$1_SQL_result'"
#}

#arr=$(echo $resetCoin|tr "," "\n")

#for line in ${arr[@]}
#do
#	getResutFromSQL $line
#	./cpResult.sh $line
#done

