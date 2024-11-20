INCLUDE_EIGEN   = $(CONDA_PREFIX)/include/eigen3
INCLUDE_XTENSOR = $(CONDA_PREFIX)/include
OPTI_LEVEL = -O2


all: main-run

%.out: %.cc
	$(CXX) $(OPTI_LEVEL) -std=c++20 -g -pg -I $(INCLUDE_EIGEN) -I $(INCLUDE_XTENSOR) $< -o $@

%-run: %.out
	./$<

time.txt: time.out
	./$< > $@

time.png: time.txt plot.plt
	gnuplot plot.plt

clean:
	rm -f *.out *.txt
