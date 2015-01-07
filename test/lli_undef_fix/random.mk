

random: random.cpp
	clang++ -stdlib=libc++ random.cpp -nodefaultlibs -lc++ -lc++abi -lm \
	    -lc -lgcc_s -lgcc

#	clang++ `llvm-config --cxxflags` -c random.cpp
#	clang++ `llvm-config --ldflags` \
#	    -stdlib=libc++ \
#	    `llvm-config --libs` -o random random.o
