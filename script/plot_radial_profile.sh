#!/usr/bin/env gnuplot

set palette rgb 33,13,10

set term x11 0
#set terminal png
#set output 'data/radialProfile.png'
set title "Radial Profile 171005023 Column 8 of 50 (m^{-3})"
show title

plot 'data/leftRadialProfile.txt' using ($1*1000):2 title "Left Profile",\
     'data/rightRadialProfile.txt' using ($1*1000):2 title "Right Profile"

set term x11 1
#set output 'data/lineIntegratedSlice.png'
#set title "Line Integrated Slice 171005023, Column 8 of 50 (m^{-3})"
show title

plot 'data/lineIntegratedSlice.txt' using 0:1 title "Line Integrated Cross Section"

pause -1
