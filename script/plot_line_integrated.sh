#!/usr/bin/env gnuplot

set palette rgb 33,13,10

set term x11 0
#set terminal png
#set output 'data/lineIntegrated.png'
set title "Line Integrated Density 171005023 (m^{-3})"
show title

plot 'data/lineIntegrated.dat' binary matrix with image title ""

pause -1
