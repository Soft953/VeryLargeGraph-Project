#!/bin/bash

#Test on ip graph

make -C src/gen-louvain/
./src/gen-louvain/convert -i graph/$1 -o graph/$1.bin
./src/gen-louvain/louvain graph/$1.bin -l -1 -v > graph/$1.tree
./src/gen-louvain/hierarchy graph/$1.tree

make -C src/reordering/
./src/reordering/reordering graph/$1 graph/$1_reord
./src/gen-louvain/convert -i graph/$1_reord -o graph/$1_reord.bin
./src/gen-louvain/louvain graph/$1_reord.bin -l -1 -v > graph/$1_reord.tree
./src/gen-louvain/hierarchy graph/$1_reord.tree
