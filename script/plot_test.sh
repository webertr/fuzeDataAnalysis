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

#set output 'data/baseLine.png'

#set xrange[2869:3978]
#set yrange[1242:4977]

#set cbrange [-0.5:0.5]

#plot 'data/baseLineVsBaseLine.dat' binary matrix with image title "Baseline"

#set title "Scene Beam Phase No Flame" font ",20"
#show title

#plot 'data/lineIntegrated.dat' binary matrix with image title ""
#plot 'data/hologramPreview.dat' binary matrix with image title "Hologram Preview"

#plot 'data/radialProfile.txt' using 0:1 title "Radial Profile"

#set term x11 0

#plot 'data/rawHologram.dat' binary matrix with image title ""

#set term x11 1

#plot 'data/rawHologramCol.txt' using 1 title "Column from Raw Hologram"

#set term x11 2

#plot 'data/rawHologramRow.txt' using 1 title "Row from Raw Hologram",\
#     'data/fitHologramRow.txt' using 1 w lines ls 1 lc 2 title "Fit"

#set term x11 3

#plot 'data/fitHologram.dat' binary matrix with image title ""

#set terminal png
#set output 'data/wrappedPhaseFlame.png'
#set xrange[0:1203]
#set yrange[0:638]

#plot 'data/hologramPreview.dat' binary matrix with image title ""
#plot 'data/lineIntegratedWrappedPhase.dat' binary matrix with image title ""


#set term x11 0

#plot 'data/radialProfile0.txt' using 1 title "Initial Radial Density Profile Col. 0",\
#     'data/radialProfile99.txt' using 1 title "Initial Radial Density Profile Col. 99"

# set term x11 1

# plot 'data/leftRadialProfile0.txt' using 1 title "Reconstructed Left Radial Profile 0",\
#     'data/rightRadialProfile0.txt' using 1 title "Reconstructed Right Radial Profile 0"

# set term x11 2

# plot 'data/leftRadialProfile99.txt' using 1 title "Reconstructed Left Radial Profile 99",\
#     'data/rightRadialProfile99.txt' using 1 title "Reconstructed Right Radial Profile 99"

# set term x11 0
# # #set terminal png
# # #set output 'data/radialProfile.png'
# set title "Forward Projected Line Integrated Data (m^{-2})" font ",20"
# show title
# set ylabel "y (pixel)" font ",20"
# set xlabel "x (pixel)" font ",20" offset 0,0
# # #set xrange[0:100]
# # #set yrange[0:100]

# plot 'data/lineIntegrated.dat' binary matrix with image title ""

# #set term x11 4

# #plot 'data/test.txt' using 1 title "Right Slice Before Inversion"

# set term x11 1

# set title "Reconstructed Radial Density Profile (m^{-3})" font ",20"
# show title

# set ylabel "Number Density (Arb.)" font ",20"
# set xlabel "Radius (pixel)" font ",20" offset 0,0

# plot 'data/leftRadialProfile0.txt' using 1 title "Reconstructed Left Radial Profile 0" ps 2,\
#      'data/rightRadialProfile0.txt' using 1 title "Reconstructed Right Radial Profile 0" ps 2,\
#      'data/radialProfile0.txt' using 1 title "Initial Radial Density Profile"

# set term x11 2
# #set terminal png
# #set output 'data/wrappedPhaseCandle1.png'

# #set size ratio -1

# #set xrange[0:1562]
# #set yrange[0:710]

# set title "Reconstructed Radial Density Profile (m^{-3})" font ",20"
# #set title "Unwrapped Phase of No Flame vs. No Flame" font ",20"
# show title

# #set ylabel "y (pixel)" font ",16"
# #set xlabel "x (pixel)" font ",16" offset 0,0
# set ylabel "Number Density (Arb.)" font ",20"
# set xlabel "Radius (pixel)" font ",20" offset 0,0

# #plot 'data/lineIntegratedWrappedPhase.dat' binary matrix with image title ""
# plot 'data/leftRadialProfile99.txt' using 1 title "Reconstructed Left Radial Profile 99" ps 2,\
#      'data/rightRadialProfile99.txt' using 1 title "Reconstructed Right Radial Profile 99" ps 2,\
#      'data/radialProfile0.txt' using 1 title "Initial Radial Density Profile"

# set term x11 3
# #set terminal png
# #set output 'data/radialProfile.png'
# set title "Line Integrated Data Slices (m^{-2})" font ",20"
# show title
# set ylabel "Line integrated n(r) (Arb.)" font ",20"
# set xlabel "Radius (pixel)" font ",20" offset 0,0

# plot 'data/lineIntegratedSlice0.txt' using 1 title "Line Integrated Slice 0",\
#      'data/lineIntegratedSlice99.txt' using 1 title "Line Integrated Slice 99"


# set term x11 0

# set size ratio -1

# plot 'data/plasmaImage.dat' binary matrix with image title "2D Real Density"

# set term x11 1

# set size ratio -1

# plot 'data/plasmaImageProject.dat' binary matrix with image title "Line Integrated Image"

# set term x11 2

# set size ratio 1

# plot 'data/forwardProjectImage.txt' using 1 title "Line Integrated Slice"

# set term x11 3

# plot 'data/leftRadialProfile.txt' using 1 title "Reconstructed Left Radial Profile",\
#      'data/rightRadialProfile.txt' using 1 title "Reconstructed Right Radial Profile"

# set term x11 4

# set size ratio -1

# plot 'data/plasmaImageRecon.dat' binary matrix with image title "2D Reconstructed Density"

#set term x11 0
set terminal png
set output 'data/lineIntegratedDensity171213010.png'
set title "Line Integrated Density 171213010"
show title
set size ratio -1
set yrange[-0.009:0.009]
set xrange[-0.0053:0.0053]

set ylabel "y (m)" font ",16"
set xlabel "z (m)" font ",16" offset 0,0

plot 'data/lineIntegrated.dat' binary matrix with image title ""

pause -1
