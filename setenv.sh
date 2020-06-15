#!/bin/bash

export ARCH="mips"
export CROSS_COMPILE="mips64el-linux-gnuabi64-"



export __bd=$PWD
alias bd='cd ${__bd}'
alias BD='__bd=$PWD'

alias m='cd bsp/ls2kdev/'

