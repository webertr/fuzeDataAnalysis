#!/usr/bin/env gnuplot

set term x11 0

set ylabel "I (kA)" font ",16" offset -1,0
set xlabel "time (usec)" font ",16" offset 0,0
set xrange [0:200]

plot 'ip_convert.txt' using ($1*1E6):2 title "I_{P}",\

set term x11 1

set ylabel "I (kA)" font ",16" offset -1,0
set xlabel "time (usec)" font ",16" offset 0,0
set xrange [0:200]

plot 'ic1_convert.txt' using ($1*1E6):2 title "I_{C1}",\
     'iout1_convert.txt' using ($1*1E6):2 title "I_{OUT1}",\
     'icb1_convert.txt' using ($1*1E6):2 title "I_{CB1}"


set term x11 2

set ylabel "V (kV)" font ",16" offset -1,0
set xlabel "time (usec)" font ",16" offset 0,0
set title "Switch triggers at 0 V"

plot 'vigcb_convert.txt' using ($1*1E6):2 title "V_{ICB}"

#plot 'vgap_convert.txt' using ($1*1E6):2 title "V_{Gap}"

pause -1
