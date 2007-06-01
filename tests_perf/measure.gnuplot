# GNUPLOT script to plot data from measure program
# To use: Run gnuplot, then type "load 'measure.gnuplot'"
# To get X11 output instead of PNG, use "set terminal x11" and "replot"
# Useful command to set y-range to eliminate peaks: "set yrange [0:1000]"
set ylabel 'latency (microseconds)'
set xlabel 'message number'
set grid
set grid noxtics
set terminal png medium size 640,480
set output 'latency.png'
plot 'latency.dat' t 'Latency' with lines
set ylabel 'throughput (messages/second)'
set xlabel 'time (seconds)'
set output 'throughput.png'
plot 'throughput.dat' index 0 t 'sender' with lines, 'throughput.dat' index 1 t 'receiver' with lines
