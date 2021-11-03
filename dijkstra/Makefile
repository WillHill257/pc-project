p1 = graphGenerator
p2 = serial

all: ${p1} ${p2}

${p1}: ${p1}.cpp
	g++ -std=c++11 ${p1}.cpp -o ${p1}

${p2}: ${p2}.cpp
	g++ -std=c++11 ${p2}.cpp -o ${p2}

clean:
	rm -rf ${p1} ${p2}