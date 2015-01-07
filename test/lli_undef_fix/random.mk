

random: random.cpp
	g++ -std=c++11 \
	    `llvm-config --cxxflags --ldflags --libs ` \
	    random.cpp -o random

#	clang++ `llvm-config --cxxflags` -c random.cpp
#	clang++ `llvm-config --ldflags` \
#	    -stdlib=libc++ \
#	    `llvm-config --libs` -o random random.o
