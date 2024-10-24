INCLUDE_EIGEN   = $(CONDA_PREFIX)/include/eigen3
INCLUDE_XTENSOR = $(CONDA_PREFIX)/include

all: main
	./main

main: main.cc
	$(CXX) -O2 -I $(INCLUDE_EIGEN) -I $(INCLUDE_XTENSOR) $< -o $@

clean:
	rm -f main