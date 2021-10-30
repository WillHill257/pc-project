p1 = serial
p2 = parallel

${p1}: ${p1}.cpp
	@g++ ${p1}.cpp -o ${p1}

${p2}: ${p2}.cpp
	@mpicxx ${p2}.cpp -o ${p2}

clean:
	rm -rf ${p1} ${p2}