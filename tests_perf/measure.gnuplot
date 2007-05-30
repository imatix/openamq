# GNUPLOT script to plot data from measure program
# To use: Run gnuplot, then type "load 'measure.gnuplot'"
# To get X11 output instead of PNG, use "set terminal x11" and "replot"
# Useful command to set y-range to eliminate peaks: "set yrange [0:1000]"
set ylabel 'microseconds'
set xlabel 'message #'
set grid
set grid noxtics
set terminal png medium size 640,480
set output 'measure.png'
plot 'out.csv' t 'Latency' with lines
