#!/bin/bash

configFileName=../../src/Config.h
soFileName=libLAS-SDK.so

IFS=$'\n' # set the Internal Field Separator to newline
for line in $(cat "$configFileName")
do
    if [[ $line == *"LAS_SDK_VERSION_MAJOR"* ]]; then
       major=$(echo "$line" | tr -d -c 0-9)
    fi

    if [[ $line == *"LAS_SDK_VERSION_MINOR"* ]]; then
        minor=$(echo "$line" | tr -d -c 0-9)
    fi

    if [[ $line == *"LAS_SDK_VERSION_PATCH"* ]]; then
        patch=$(echo "$line" | tr -d -c 0-9)
    fi
done

 cd ../../bin/

finalSOName=libLAS-SDK.so.$major.$minor.$patch

if [ -f $soFileName ]; 
then
  mv $soFileName $finalSOName
else
   echo "Could not find $soFileName."
   exit -1
fi