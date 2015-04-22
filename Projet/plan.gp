#!/usr/bin/gnuplot

set terminal png
set output "Plan.png"
set boxwidth 0.75 absolute
set xtic auto # set xtics automatically
set ytic auto # set ytics automatically
set title "Plan"
set xlabel "X"
set ylabel "Y"
set xr [0:8]
set yr [0.:8]
set object 1 rect from 0,4 to 4,8 fc rgb "cyan"
set object 2 rect from 4,0 to 6,4 fc rgb "green"
set object 3 rect from 4,4 to 8,8 fc rgb "yellow"
set object 4 rect from 0,0 to 4,4 fc rgb "red"
set object 5 rect from 6,0 to 8,4 fc rgb "white"
set offset 1,1,1,1
plot 'dot.dat' using 1:2:(sprintf("(%d, %d)", $1, $2)) with labels point  pt 7 offset char 1,1 notitle
