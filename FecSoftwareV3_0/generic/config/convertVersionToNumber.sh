#!/bin/sh
echo $1 | awk -F . '{ print ($1*65536 + $2*256 + $3) }' | cat 
