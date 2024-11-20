#! /usr/bin/env gnuplot

set terminal pngcairo size 1000,600 enhanced font 'Verdana,9'
set output "time.png"

set xlabel "N"
set ylabel "time"

set logscale x 10
set logscale y 10

set key top left
set xrange [25:1500000]
set yrange [1e-4:1e2]

plot "valarray.txt" u 1:3 w lp lt 4 lc "#badc58" title "std::valarray : container", \
                 "" u 1:5 w lp lt 8 lc "#6ab04c" title "std::valarray : expression", \
     "vectorXd.txt" u 1:3 w lp lt 4 lc "#7ed6df" title "Eigen::VectorXd : container", \
                 "" u 1:5 w lp lt 8 lc "#22a6b3" title "Eigen::VectorXd : expression", \
     "xarray.txt"   u 1:3 w lp lt 4 lc "#ffbe76" title "xt::xarray : container", \
                 "" u 1:5 w lp lt 8 lc "#f0932b" title "xt::xarray : expression", \
     0.0000025*x w l dt 2 lc "#130f40" title "O(N)"
