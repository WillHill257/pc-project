p1 = graphGenerator
p2 = serial
p3 = mpi

all: ${p1} ${p2} ${p3}

${p1}: ${p1}.cpp
	@g++ -std=c++11 ${p1}.cpp -o ${p1}

${p2}: ${p2}.cpp
	@g++ -std=c++11 ${p2}.cpp -o ${p2}

${p3}: ${p3}.cpp
	@mpicxx -std=c++11 ${p3}.cpp -o ${p3}

clean:
	@rm -rf ${p1} ${p2} ${p3} ${p3}.dSYM