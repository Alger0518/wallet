#!/usr/bin/expect -f

set var [lindex $argv 0]

spawn scp -r root@192.168.199.217:/opt/$var'_SQL_result' /usr/local/wallet/reset/
expect "*password:" { send "123456\n" }
expect eof

spawn ssh root@192.168.199.217 "rm -rf /opt/$var'_SQL_result'"
expect "*password:" { send "123456\n" }
expect eof
