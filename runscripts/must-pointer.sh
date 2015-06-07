#!/bin/bash

filename=$(basename "$1")
opt -mem2reg -analyze -load $LLVMLIB/CSE231.so -instnamer -must_pointer < $1/$filename.bc > /dev/null
