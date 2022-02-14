set terminal postscript enhanced color
set size 1.2, 1.2
#set size 1.5, 1.5

set key right bottom

set xlabel "Number of ONUs"
set xrange [20:1200]
set ylabel "Downstream Network Load (Mbps)"
set yrange [80:12000]

set logscale x
set logscale y
set logscale z


#set ticslevel 0
#set ztics 1

set data styl line

#set hidden3d
set pm3d

#set view 60,15 
#set view 60,30

#set title "Consumed CPU (second)"
set zrange [1:200]
set output "sim-udp-cpu.eps"
splot "sim-udp-cpu.txt"  notitle 

