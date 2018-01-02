#!/usr/bin/env gnuplot

# Plot the mode data

#xdim = 800
#ydim = 600
#xdim = 4000
#ydim = 6016
#xdim = 828
#ydim = 1808
#set terminal png enhanced
#set terminal x11 enhanced 0
#set xrange[0:xdim]
#set yrange[0:ydim]
set style line 1 lt 3 lc rgb "black" lw 3

#set output "data/test.png"
#set palette gray
set palette rgb 33,13,10
#set cbrange [-3:3]

set term x11 0
set title "Pulse 170420028 (vs. 170420026) \nLine Integrated Electron Density" font ",14"
set arrow from 5, graph 0 to 5, graph 1 nohead front ls 1
plot 'data/lineIntegrated.dat' binary matrix with image title ""

#replot

# set term x11 1
# set title "Pulse 170420028 (vs. 170420026) \nLeft vs. Right Radial Profiles" font ",14"

# set xtics font ",16"
# set ytics font ",16"

# set ylabel "n_{e} (m^{-3})" font ",16" offset -1,0
# set xlabel "r (mm) " font ",16" offset 0,0

# set label "5th Column" at graph 0.8,0.2 center font ",14"

set term x11 1
plot 'data/leftRadialProfile.txt' using ($1*1000):2 title "Left Profile",\
     'data/rightRadialProfile.txt' using ($1*1000):2 title "Right Profile"

set term x11 2
plot 'data/test.txt' using 0:1 title "Corn"

set term x11 3
plot 'data/ellie5thColLID.txt' using 0:2 title "Ellie"

pause -1
