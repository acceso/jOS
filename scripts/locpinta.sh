#!/bin/sh

# Gets a graph of my evolution,
# I need feedback :)

gnuplot > loccount.png <<EOG
reset
set terminal png
set xdata time
set timefmt "%Y%m%d"
set ylabel "locs"
set grid
plot "loccount.txt" using 1:2 with lines title "loc evolution"
EOG

