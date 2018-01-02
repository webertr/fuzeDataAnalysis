#!/usr/bin/env gnuplot

set palette rgb 33,13,10

set size ratio -1

set term x11 0

plot 'data/rawHologram.dat' binary matrix with image title "Raw Hologram"

set term x11 1

set size ratio .75

plot 'data/rawHologramCol.txt' using 0:1 title "Col From Raw Hologram"

set term x11 2

plot 'data/rawHologramRow.txt' using 0:1 title "Row From Raw Hologram"

pause -1
