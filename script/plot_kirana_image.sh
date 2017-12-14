#!/usr/bin/env gnuplot

set palette rgb 33,13,10

set term x11 0
#set terminal png
#set output 'data/lineIntegrated.png'
set title "Kirana Image"
show title

plot 'data/kiranaImage.dat' binary matrix with image title ""

pause -1
