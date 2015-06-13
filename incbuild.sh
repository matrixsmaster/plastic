#!/bin/sh

BUILD=`cat $1 | awk '{ print $3 }'`
echo $BUILD
BUILD=`expr $BUILD + 1`
echo "#define BUILDNO $BUILD" 1> $1
