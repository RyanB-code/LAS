#!/bin/bash

cd ~
ln -s /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Environment/bin/libLAS-SDK.so.0.1.0 libLAS-SDK.so

cp libLAS-SDK.so /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Environment/lib/

rm -rf libLAS-SDK.so

mv /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Environment/bin/libLAS-SDK.so.0.1.0 /mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Environment/bin/libLAS-SDK.so.0