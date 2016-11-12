#!/bin/bash

echo "Compile KGMiner..."
cmake - build .
make gbserver -j4

echo "Download infobox and facts..."
if [ ! -f ./data.zip ] 
	then
	wget "https://www.dropbox.com/s/xa55p0clno4k2l0/data.zip?dl=1" -O data.zip
	fi
if [ ! -f ./facts.zip ] 
	then
	wget "https://www.dropbox.com/s/uezdxht4elob6b3/facts.zip?dl=1" -O facts.zip
	fi
unzip data.zip 
unzip facts.zip 
mv "facts-2" facts

echo "Done"
