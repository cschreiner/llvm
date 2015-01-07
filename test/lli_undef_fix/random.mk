

random: random.cpp
	clang `llvm-config --cxxflags --ldflags` \
	    random.cpp \
	    `llvm-config --libs`
