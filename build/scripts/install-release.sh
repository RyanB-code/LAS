#!/bin/bash

# Run from top level directory of project
# Expects LD_LIBRARY_PATH to be set and exported to work


sudo cp bin/Environment/Release/*.exe   /usr/local/bin
sudo cp bin/SDK/Release/*so.[0-9]       /usr/local/lib/LAS
