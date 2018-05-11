#!/usr/bin/env gnuplot

set ylabel "I (kA)" font ",16" offset -1,0
set xlabel "time (usec)" font ",16" offset 0,0

plot 'it1_convert.txt' using ($1*1E6):2 title "I 1 Total",\
     'ic1_convert.txt' using ($1*1E6):2 title "I C1",\
     'id1_convert.txt' using ($1*1E6):2 title "I D1",\
     'ip_convert.txt' using ($1*1E6):2 title "I P"
pause -1
