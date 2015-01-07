

random: random.cpp
	clang++ -stdlib=libc++ -std=c++11 -lc++ random.cpp -o random

#	clang++ `llvm-config --cxxflags` -c random.cpp
#	clang++ `llvm-config --ldflags` \
#	    -stdlib=libc++ \
#	    `llvm-config --libs` -o random random.o
