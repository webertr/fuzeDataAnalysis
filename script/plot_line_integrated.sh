#!/usr/bin/env gnuplot

set palette rgb 33,13,10

set term x11 0
#set terminal png
#set output 'data/lineIntegrated180215015.png'
#set title "Line Integrated Density 180215015 (m^{-3})"
show title

plot 'data/lineIntegrated.dat' binary matrix with image title ""

pause -1
