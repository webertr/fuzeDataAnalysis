#!/usr/bin/env gnuplot

set palette rgb 33,13,10

set term x11 0

set xrange[0:4000]
set yrange[0:6016]
#set xrange[4300:5995]
#set yrange[1863:2755]

set size ratio -1
#set terminal png
#set output 'data/hologramPreview.png'

plot 'data/hologramPreview.dat' binary matrix with image title ""

pause -1
