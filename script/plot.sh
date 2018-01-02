#!/usr/bin/env gnuplot

# Plot the mode data

#set terminal png enhanced
#set style line 1 lt 3 lc rgb "black" lw 5
#set style line 2 lt 3 lc rgb "red" lw 3 dt 2
#set style line 3 lt 3 lc rgb "blue" lw 3 dt 2
#set style line 4 lc rgb 'black' pt 5   # square
#set terminal x11 enhanced 1
#set ticslevel 0
#set arrow from 25, graph 0 to 25, graph 1 nohead ls 3
#set xlabel "{/Symbol m}sec" font ",20"
#set xlabel "nm" font ",14" offset 0,0
#set ylabel "Amplitude (arb.)" font ",14" offset 0,0
#set xrange[200:300]
#set yrange[0:]
#set grid
#set label "S" at graph 0.5,0.5 center font "Symbol,24"
#set key at graph .9,0.95 font ",18"
#set key left top font ",14"
#set xtics font ",14"
#set ytics font ",14"
#show grid
#set output "data/carbon_shear_one_line.png"
#set output "data/cd_line.png"
#set output "data/test.png"
#set title "Pulse 170405013"
#set arrow from 229.6871, graph 0 to 229.6871, graph 1 nohead ls 3
#set arrow from 228.80225, graph 0 to 228.80225, graph 1 nohead ls 3
#set arrow from 226.502, graph 0 to 226.502, graph 1 nohead ls 3
#set arrow from 467.81493, graph 0 to 467.81493, graph 1 nohead ls 3
#set arrow from 466.23520, graph 0 to 466.23520, graph 1 nohead ls 3
#set arrow from 468.57, graph 0 to 468.57, graph 1 nohead ls 3
#set xrange[0:1023]
#set yrange[0:19]
#set xrange[229.5:229.8]
#set xrange[228.5:229.1]
#set xrange[466.8:467.5]
#set xrange[467:470]
#set xrange[650:1024]
#set yrange[0:3000]
#set arrow from 229.7, graph 0 to 229.7, graph 1 nohead ls 3
#unset key
#plot 'data/test.txt' using 1:2 with line ls 1 title "light"
#plot 'data/data_710.txt' using 1:2 with line ls 1 title "365",\
#     'data/data_710.txt' using 1:2 with line ls 2 title "710"
#plot 'data/pic_calib.dat' binary endian=little array=(1024,1024) format="%float" with image,\
#     'data/data_365.txt' using 1:2 with line ls 1 title "365"
#plot 'data/pic.dat' binary endian=little array=(1024,20) format="%float" with image
#plot 'data/test1.txt' using 1:2 with line ls 1 title "365",\
#     228.835 notitle ls 2
#plot 'data/pic.dat' binary endian=little array=(1024,1024) format="%double" transpose with image
#plot 'data/test_plot.dat' binary format="%double%double"
#plot 'data/test_plot.txt'
#plot 'data/test_image.dat' binary endian=little array=(1024,1024) format="%double" with image
#plot 'data/test_image_fiber.dat' binary endian=little array=(1024,20) format="%double" with image
#plot 'data/cd_calib_226_228nm/plot_cd_1.txt' using ($2/10):1 with line ls 1 title "Cd Lamp-low",\
#     'data/cd_calib_226_228nm/plot_cd_10.txt' using ($2/10):1 with line ls 2 title "Cd Lamp-med",\
#     'data/cd_calib_226_228nm/plot_cd_15.txt' using ($2/10):1 with line ls 3 title "Cd Lamp-high"
#plot 'data/test_plot.txt' using 1:2 with line ls 1 title "Cd Lamp"
#plot 'data/wv.txt' using 0:1 with line ls 1 title "Cd Lamp",\
#     'data/wv_corr.txt' using 0:($1/10) with line ls 2 title "Cd Lamp",\
#plot 'data/mean.txt' using 0:1 with line ls 1 title "365"
#plot 'data/image.dat' binary endian=little array=(1024,1024) format="%double" with image
set xrange[0:6016]
set yrange[0:4000]
#set palette gray
set palette rgb 33,13,10
set cbrange [-3:3]

#set xrange[0:4000]
#set yrange[0:6016]

# 400 Width x 6016 Height
#plot 'data/pic.dat' binary endian=little array=(800,600) format="%double" with image title ""
plot 'data/fphase.dat' binary endian=little array=(6016,4000) format="%double" with image title ""
#plot 'data/pic.dat' binary endian=little array=(4000,6016) format="%double" with image title ""

#plot 'data/test.txt' using 0:1 with line ls 1 title "7"

#set term x11 0
#set xrange[(7-9)*1.24:(11-9)*1.24]
#set term x11 0
#plot 'data/test.txt' using ($0-9)*1.24:1 with points ls 4 title "7"


#set term x11 1
#set xrange[0:1024]
#plot 'data/plot_cd_7.txt' using 0:1 with line ls 1 title "7",\
#     'data/plot_cd_9.txt' using 0:1 with line ls 2 title "9",\
#     'data/plot_cd_12.txt' using 0:1 with line ls 3 title "12"

#set arrow from 467.81493, graph 0 to 467.81493, graph 1 nohead ls 3
#set arrow from 466.23520, graph 0 to 466.23520, graph 1 nohead ls 3
#set arrow from 468.57, graph 0 to 468.57, graph 1 nohead ls 3

#set xrange[467.7:468.3]
#set xrange[465:469]
#set term x11 1
#plot 'data/plot_cd_6.txt' using ($2):1 with line ls 1 title "7",\
#     'data/plot_cd_9.txt' using ($2):1 with line ls 2 title "9",\
#     'data/plot_cd_13.txt' using ($2):1 with line ls 3 title "12"

#set term x11 1
#plot 'data/plot_cd_7.txt' using 0:1 with line ls 1 title "7",\
#     'data/plot_cd_10.txt' using 0:1 with line ls 2 title "9",\
#     'data/plot_cd_14.txt' using 0:1 with line ls 3 title "12"

#plot 'data/170405013/plot_carbon_5.txt' using ($2/10):($1/1500) with line ls 1 title "7th chord"

#plot 'data/170405015/plot_carbon_7.txt' using ($2/10):($1/1400) with line ls 1 title "7th chord",\
#     'data/170405015/plot_carbon_12.txt' using ($2/10):($1/1400) with line ls 2 title "12th chord",\

# set term x11 0
# plot 'data/plot_cd_1.txt' using ($2):1 with line ls 1 title "1",\
#      'data/plot_cd_2.txt' using ($2):1 with line ls 2 title "2",\
#      'data/plot_cd_3.txt' using ($2):1 with line ls 3 title "3"

# set term x11 1
# plot 'data/plot_cd_4.txt' using ($2):1 with line ls 1 title "4",\
#      'data/plot_cd_5.txt' using ($2):1 with line ls 2 title "5",\
#      'data/plot_cd_6.txt' using ($2):1 with line ls 3 title "6"

# set term x11 2
# plot 'data/plot_cd_7.txt' using ($2):1 with line ls 1 title "7",\
#      'data/plot_cd_8.txt' using ($2):1 with line ls 2 title "8",\
#      'data/plot_cd_9.txt' using ($2):1 with line ls 3 title "9"

# set term x11 3
# plot 'data/plot_cd_10.txt' using ($2):1 with line ls 1 title "10",\
#      'data/plot_cd_11.txt' using ($2):1 with line ls 2 title "11",\
#      'data/plot_cd_12.txt' using ($2):1 with line ls 3 title "12"

# set term x11 4
# plot 'data/plot_cd_13.txt' using ($2):1 with line ls 1 title "13",\
#      'data/plot_cd_14.txt' using ($2):1 with line ls 2 title "14",\
#      'data/plot_cd_15.txt' using ($2):1 with line ls 3 title "15"

# set term x11 5
# plot 'data/plot_cd_16.txt' using ($2):1 with line ls 1 title "16",\
#      'data/plot_cd_17.txt' using ($2):1 with line ls 2 title "17",\
#      'data/plot_cd_18.txt' using ($2):1 with line ls 3 title "18"


#set term x11 0
#plot 'data/temp.dat' binary endian=little array=(4000,6016) format="%double" with image title ""
#plot 'data/temp.dat' binary matrix with image title "Line"

#set term x11 0
#plot 'data/holChirpMatlab.dat' binary endian=little array=(4000,6016) format="%double" with image title ""

#set term x11 1
#plot 'data/holChirpC.dat' binary endian=little array=(4000,6016) format="%double" with image title ""

#set term x11 0
#plot 'data/lineIntegrated.dat' binary matrix with image title "Line Integrated Density (170420026 vs. 28)"


pause -1
