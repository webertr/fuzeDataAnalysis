#!/usr/bin/env gnuplot

set palette rgb 33,13,10

set term x11 0
#set terminal png
#set output 'data/lineIntegrated180308018.png'
set title "Pulse #180308018 (m^{-3})"
set xlabel "z (m)" font ",20" offset 0,0
set ylabel "b (m)" font ",20" offset 1,0
#set cblabel "m^(-3)"

show title

plot 'data/lineIntegrated.dat' binary matrix with image title ""

pause -1
