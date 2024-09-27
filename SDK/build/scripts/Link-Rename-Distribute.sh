#!/bin/bash

mode=$1

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

# =======================================================
# Set variables
if [[ "$mode" = "-r" ]]; then
    shortFileName=libLAS-SDK.so # This is what ever the build shared object library is called
else
    shortFileName=libLAS-SDK-DEV.so
fi

finalRealName=libLAS-SDK.so.$major.$minor.$patch
finalSOName=libLAS-SDK.so.$major


# =======================================================
# Link object files into SO file
g++ -fdiagnostics-color=always -shared -Wl,-soname,libLAS-SDK.so.$major -o $shortFileName ../build/intermediate/*.o 2>&1
if [[ $? -eq 0 ]]; then
    echo "Linking object files successful"
else
    echo "Failed to link object files into SO file"
    exit 2
fi

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
ln -s /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/SDK/bin/$finalRealName $shortFileName 1>/dev/null 2>&1
if [[ $? -eq 0 ]]; then
    echo "Linking into short name successful"
else
    echo "Failed to link short name and real name"
    exit 2
fi

# =======================================================
# If in release mode, move link to /usr/lib
if [[ "$mode" = "-r" ]]; then
    echo Performing Release mode tasks...
   
    # Copy to lib folder for compilation to Environment
    cp  $shortFileName /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Environment/lib/
    if [[ $? -eq 0 ]]; then
        echo "Copied to Environment/lib"
    else
        echo "Could not copy $shortname to Environment/lib"
    fi

    # Move lib to where it can be found by OS
    sudo mv /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/SDK/bin/$finalRealName /usr/local/lib/LAS/$finalSOName 1>/dev/null 2>&1
    if [[ $? -eq 0 ]]; then
        echo "Library added to /usr/local/lib/LAS"
    else
        echo "Library not added to /usr/local/lib/LAS"
        rm -rf $shortFileName 1>/dev/null 2>&1  # Delete .so from home directory when done
        exit 1
    fi
    
    rm -rf $shortFileName 1>/dev/null 2>&1  # Delete .so from home directory when done
    exit 0
else
    echo Performing Debug mode tasks...
fi

# =======================================================
# Copy to lib folder for compilation to Environment and Module-SDK
cp  $shortFileName /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Environment/lib/
if [[ $? -eq 0 ]]; then
    echo "Copied to Environment/lib"
else
    echo "Could not copy to Environment/lib"

    # Delete .so from home directory when done
    rm -rf $shortFileName 1>/dev/null 2>&1

    exit 2
fi

cp  $shortFileName /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Modules/Test-Module/lib/
if [[ $? -eq 0 ]]; then
    echo "Copied to Test-Module/lib"
else
    echo "Could not copy to Test-Module/lib"

    # Delete .so from home directory when done
    rm -rf $shortFileName 1>/dev/null 2>&1

    exit 2
fi

cp  $shortFileName /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Modules/Shooter-Central/lib/
if [[ $? -eq 0 ]]; then
    echo "Copied to Shooter-Central/lib"
else
    echo "Could not copy to Shooter-Central/lib"

    # Delete .so from home directory when done
    rm -rf $shortFileName 1>/dev/null 2>&1
    exit 2
fi

# =======================================================
# Move from binary folder here to Environment/bin with real soname
mv /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/SDK/bin/$finalRealName /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Environment/bin/$finalSOName 1>/dev/null 2>&1

# =======================================================
# Delete .so from home directory when done
rm -rf $shortFileName 1>/dev/null 2>&1
