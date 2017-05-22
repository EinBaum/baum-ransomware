#!/bin/bash

ME=`basename "$0"`

NAME=baumcrypt
BIN=../$NAME
KEY=_$NAME.key

if [ $ME = "makekey" ];
then
	$BIN --verbose --key $KEY
fi

if [ $ME = "delkey" ];
then
	rm -i $KEY
fi

if [ $ME = "encrypt" ];
then
	$BIN --verbose --encrypt $KEY
fi

if [ $ME = "decrypt" ];
then
	$BIN --verbose --decrypt $KEY
fi
