p1 = graphGenerator
p2 = serial
p3 = mpi
p4 = omp

os := "$(shell uname -s)"
ifeq ($(os), "Darwin")
 cc=g++-11
else
  cc=g++
endif

all: ${p1} ${p2} ${p3} ${p4}

${p1}: ${p1}.cpp
	@g++ -std=c++11 ${p1}.cpp -o ${p1}

${p2}: ${p2}.cpp
	@g++ -std=c++11 ${p2}.cpp -o ${p2}

${p3}: ${p3}.cpp
	@mpicxx -std=c++11 ${p3}.cpp -o ${p3}

${p4}: ${p4}.cpp
	${cc} -std=c++11 -fopenmp ${p4}.cpp -o ${p4}

clean:
	@rm -rf ${p1} ${p2} ${p3} ${p3}.dSYM ${p4} ${p4}.dSYM