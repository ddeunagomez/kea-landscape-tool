#!/bin/bash


export LD_LIBRARY_PATH=~/lib/armadillo-7.800.2/lib/usr/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/lib/petsc-3.7.6/arch-linux2-c-debug/lib/
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
$DIR/kea-core $@

