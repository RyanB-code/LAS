#!/bin/bash

configFileName=../../src/Config.h

# =======================================================
# Parse the Config file amd set variables

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

shortFileName=libLAS-SDK.so
finalRealName=libLAS-SDK.so.$major.$minor.$patch
finalSOName=libLAS-SDK.so.$major

# =======================================================
# Rename the new so with the real name
if [ -f $shortFileName ]; 
then
  mv $shortFileName $finalRealName
else
   echo "Could not find $shortFileName."
   exit -1
fi

# =======================================================
# Create links (have to use non-NAS directory since issue with link creation on NAS)
cd ~

# Create link from binary to vanilla soname
ln -s /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/SDK-Shared-Lib/bin/$finalRealName $shortFileName 1>/dev/null 2>&1

# Copy to lib folder for compilation to Environment and Module-SDK
cp  $shortFileName /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Environment/lib/ 1>/dev/null 2>&1
cp  $shortFileName /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Module-SDK/lib/ 1>/dev/null 2>&1

# Move from binary folder here to Environment/bin with real soname
mv /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/SDK-Shared-Lib/bin/$finalRealName /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Environment/bin/$finalSOName 1>/dev/null 2>&1

# Delete when done
rm -rf $shortFileName 1>/dev/null 2>&1
