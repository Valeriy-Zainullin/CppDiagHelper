#!/bin/bash

for fileName in *.cpp; do
  baseName="${fileName%.*}"
  binaryName="$baseName"
  outputName="$baseName.txt"
  LANG="C" g++ -pedantic -Wall -Wextra -Wshadow -Werror -std=c++17 "$fileName" -o "$binaryName" \
  	1> "$outputName" 2>&1
done
