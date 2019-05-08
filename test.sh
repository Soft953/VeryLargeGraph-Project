#!/bin/bash

make -C src/gen-louvain/
./src/gen-louvain/convert -i graph/inet -o graph/inet.bin
./src/gen-louvain/louvain graph/inet.bin -l -1 -v > graph/inet.tree
./src/gen-louvain/hierarchy graph/inet.tree

make -C src/reordering/
./src/reordering/reordering graph/inet graph/inet_reord
./src/gen-louvain/convert -i graph/inet_reord -o graph/inet_reord.bin
./src/gen-louvain/louvain graph/inet_reord.bin -l -1 -v > graph/inet_reord.tree
./src/gen-louvain/hierarchy graph/inet_reord.tree
