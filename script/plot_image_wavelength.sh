#!/usr/bin/env gnuplot

set palette rgb 33,13,10

set term x11 0
#set terminal png
#set output 'data/lineIntegrated.png'
set title "Spectrometer Image"
show title

plot 'data/imageWV.dat' binary matrix with image title ""

pause -1
