perf_evidence: main.o mongoose.o
	clang++ main.o mongoose.o -o perf_evidence -std=c++14 -stdlib=libc++

main.o: main.cpp mongoose/mongoose.h
	clang++ -c main.cpp -o main.o -std=c++11

mongoose.o: mongoose/mongoose.c mongoose/mongoose.h
	clang -c mongoose/mongoose.c -o mongoose.o

