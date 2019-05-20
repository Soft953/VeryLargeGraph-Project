#!/bin/bash


make -C src/gen-louvain/
make -C src/reordering/
./src/gen-louvain/convert -i graph/$1 -o graph/$1.bin

x=1
while [ $x -le 20 ]
do
echo "Iteration -> $x"
./src/gen-louvain/louvain graph/$1.bin -l -1 -v > graph/$1.tree
./src/reordering/reordering graph/$1 graph/$1_reord $2
./src/gen-louvain/convert -i graph/$1_reord -o graph/$1_reord.bin
./src/gen-louvain/louvain graph/$1_reord.bin -l -1 -v > graph/$1_reord.tree
x=$(( $x + 1 ))
done
