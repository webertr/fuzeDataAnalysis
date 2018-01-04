#!/usr/bin/env gnuplot

set palette rgb 33,13,10

#set term x11 0
#set terminal png
#set output 'data/sceneBeamPhaseNoFlame.png'

#set title "Radial Profile 171005023 Column 8 (m^{-3})"
#show title
#set xrange[-3.4:3.4]
#set yrange[-2.2:2.2]
#set xtics -3E-3, 1.5E-3, 3E-3
#set ytics -3E-3, 1E-3, 3E-3
#set format x "%.1e"
#set format y "%.1e"
#set size ratio -1
#set yrange[0:18e23]
#set xrange[0:6.2]
#set yrange[0:18e23]
#set xtics font ",16"
#set ytics font ",16"
#set ytics 0, 5e23, 20e23
#set ylabel "{/Symbol m}sec" font ",20"
#set xlabel "r (mm)" font ",20" offset 0,0
#set xrange[-2.7:2.6]
#set yrange[-6.6:6.7]
#set linetype 7 lw 4 lc rgb "black" pointtype 7
#set xrange[2869:3978]
#set yrange[1242:4977]

#set xlabel "r (m)" font ",16"
#set ylabel "y (m)" font ",16"

#set cbrange [-4:0]

#plot 'data/rightRadialProfile.txt' using ($1*1000):2 lc rgb "black" pt 5 ps 1 title ""
#plot 'data/leftRadialProfile.txt' using ($1*1000):2 lt 7 title ""
#plot 'data/flameHologram.dat' binary matrix with image title "Flame Hologram"
#plot 'data/lineIntegrated.dat' binary matrix with image title ""

set xrange[0:4000]
set yrange[0:6016]

set size ratio -1
set terminal png
#set term x11 0
set output 'data/hologramPreviewd42.png'

plot 'data/hologramPreview.dat' binary matrix with image title ""
