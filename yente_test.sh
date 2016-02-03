#! /bin/bash

if [ $1 ]
then
  if [ ! -d "output" ]
  then
    mkdir output
  fi
  ./yente "127.0.0.1:3306" "dbuser" "dbpass" "dbname" "output/$1"
else
  ./yente "127.0.0.1:3306" "dbuser"  "dbpass" "dbname" "OutputFile.txt"
fi
#valgrind --leak-check=full --show-reachable=yes ./yente "127.0.0.1:3306"  "dbuser"  "dbpass" "dbname" "OutputFile.txt"

exit 0
