#!/bin/bash

NAME=baumcrypt
BIN=../$NAME
KEY=.$NAME.key

if [ -f $KEY ]; then
	echo "key already exists, not encrypting"
else
	$BIN --encrypt $KEY --verbose
	$BIN --print
fi
