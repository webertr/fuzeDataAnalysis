#!/usr/bin/env gnuplot

set term x11 0

set ylabel "I (kA)" font ",16" offset -1,0
set xlabel "time (usec)" font ",16" offset 0,0

plot 'ic1_convert.txt' using ($1*1E6):2 title "I 1 Total",\
     'ic1_convert.txt' using ($1*1E6):2 title "I C1",\
     'ic1_convert.txt' using ($1*1E6):2 title "I D1",\
     'ic1_convert.txt' using ($1*1E6):2 title "I P"

set term x11 1

set ylabel "V_{Gap} (kV)" font ",16" offset -1,0
set xlabel "time (usec)" font ",16" offset 0,0

plot 'ic1_convert.txt' using ($1*1E6):2 title "V_{GapR}",\
     'ic1_convert.txt' using ($1*1E6):2 title "V_{GapL}"

pause -1
