p1 = serial
p2 = parallel

all: ${p1} ${p2}

${p1}: ${p1}.cpp
	@g++ -std=c++11 ${p1}.cpp -o ${p1}

${p2}: ${p2}.cpp
	@mpicxx -std=c++11 ${p2}.cpp -o ${p2}

clean:
	@rm -rf ${p1} ${p2} ${p2}.dSYM