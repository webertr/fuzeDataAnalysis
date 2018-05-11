gcc main.c
rm *.txt
ngspice -bo log.txt thyristorBank.cir
./a.out
./gnuplot_script.sh
more log.txt
