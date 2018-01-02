#!/usr/bin/env gnuplot

set palette rgb 33,13,10

set term x11 0

plot 'data/b_n95_000_sm_plot.txt' using 1:2 title "N95" ps 2,\
     'data/b_n95_000_sm_plot.txt' using 1:2 title "N95" ps 1

pause -1
