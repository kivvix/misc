INCLUDE_EIGEN   = $(CONDA_PREFIX)/include/eigen3
INCLUDE_XTENSOR = $(CONDA_PREFIX)/include
OPTI_LEVEL = -O2


all: main-run

%.exe: %.cc
	$(CXX) $(OPTI_LEVEL) -I $(INCLUDE_EIGEN) -I $(INCLUDE_XTENSOR) $< -o $@

%-run: %.exe
	./$<

clean:
	rm -f *.exe *.txt