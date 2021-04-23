CXX        = g++
CXXFLAGS   = -Wall -Wpedantic -Wextra -std=c++14 -g
INCLUDES   = -I.
LIBS       = 

SRCS =	\
	src/skein.cpp	\
	src/combinator.cpp	\
	src/main.cpp

.PHONY: skein
skein:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o skein $(LIBS)
