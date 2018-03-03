#!/bin/bash

HOST=fixme

DIRECTORY=`dirname $1`
TARGET=`basename $1|cut -d'.' -f1 | sed -e "s|/|_|g"`

echo DEPLOYING $TARGET to $HOST
ssh -p 22 $HOST  "rm -f /tmp/$TARGET"
scp -P 22 bin/$TARGET $HOST:/tmp
ssh -t -p 22 $HOST /tmp/$TARGET
