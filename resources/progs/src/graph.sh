#!/bin/sh

INPUT=$1 # input file name

make

# Create jgr files
./bin/graph $INPUT

# Call jgraph on every jgr file
jgraph -P jgr/numCycles.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/numCycles.jpg
jgraph -P jgr/ipc.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/ipc.jpg
jgraph -P jgr/insts.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/insts.jpg
jgraph -P jgr/l1i-acc.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1i-acc.jpg
jgraph -P jgr/l1i-hits.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1i-hits.jpg
jgraph -P jgr/l1i-miss.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1i-miss.jpg
jgraph -P jgr/l1d-acc.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1d-acc.jpg
jgraph -P jgr/l1d-hits.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1d-hits.jpg
jgraph -P jgr/l1d-miss.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l1d-miss.jpg
jgraph -P jgr/l2-acc.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l2-acc.jpg
jgraph -P jgr/l2-hits.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l2-hits.jpg
jgraph -P jgr/l2-miss.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/l2-miss.jpg
jgraph -P jgr/meta-acc.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/meta-acc.jpg
jgraph -P jgr/meta-hits.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/meta-hits.jpg
jgraph -P jgr/meta-miss.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/meta-miss.jpg
jgraph -P jgr/mem-reads.jgr | ps2pdf - | magick -density 300 - -quality 100 jpg/mem-reads.jpg
