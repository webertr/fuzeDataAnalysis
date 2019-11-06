gcc main.c
rm *.txt
ngspice -bo log.txt ignitronBank.cir
./a.out
./gnuplot_script.sh
more log.txt
