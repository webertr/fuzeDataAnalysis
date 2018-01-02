#!/usr/bin/env gnuplot

set palette rgb 33,13,10

set ylabel "radians" font ",16" offset -1,0

set term x11 0
#set terminal png
#set output 'data/phaseColSlice.png'

set label "1 radian = 6.7E20/m^2" at graph 0.8,0.9 center font ",14"

plot 'data/phaseCol.txt' using ($0):1 title "Phase Col Slice"

set term x11 1
#set terminal png
#set output 'data/unwrappedColSlice.png'

set label "1 radian = 6.7E20/m^2" at graph 0.8,0.9 center font ",14"

plot 'data/phaseUnwrapCol.txt' using ($0):1 title "Unwrapped Phase Col Slice"

set term x11 2
#set terminal png
#set output 'data/phaseRowSlice.png'

set label "1 radian = 6.7E20/m^2" at graph 0.8,0.9 center font ",14"

plot 'data/phaseRow.txt' using ($0):1 title "Phase Row 61 Slice"

set term x11 3
#set terminal png
#set output 'data/unwrappedRowSlice.png'

set label "1 radian = 6.7E20/m^2" at graph 0.8,0.9 center font ",14"

plot 'data/phaseUnwrapRow.txt' using ($0):1 title "Unwrapped Phase Row 61 Slice"

set term x11 4
#set terminal png
#set output 'data/unwrappedPhase.png'
set title "Unwrapped Phase"
show title
set size ratio -1

plot 'data/lineIntegratedUnWrappedPhase.dat' binary matrix with image title ""

set term x11 5
#set terminal png
#set output 'data/wrappedPhase.png'
set title "Wrapped Phase"
show title
set size ratio -1

plot 'data/lineIntegratedWrappedPhase.dat' binary matrix with image title ""

pause -1
