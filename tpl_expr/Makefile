INCLUDE_EIGEN   = $(CONDA_PREFIX)/include/eigen3
INCLUDE_XTENSOR = $(CONDA_PREFIX)/include
OPTI_LEVEL = -O2

all: main
	./main

main: main.cc
	$(CXX) $(OPTI_LEVEL) -I $(INCLUDE_EIGEN) -I $(INCLUDE_XTENSOR) $< -o $@

clean:
	rm -f main