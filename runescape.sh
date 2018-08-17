#!/bin/bash
prog=$1

while read -r line; do echo -e $line; done | $prog
