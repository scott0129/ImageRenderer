EXENAME = main
OBJS_FILE = main.cpp Hittable.cpp Vector.cpp Objects.cpp objectCollection.cpp Renderer.cpp
OBJS = $(OBJS_FILE:.cpp=.o)

CXX = clang++
WARNINGS = -pedantic -Wall -Werror -Wfatal-errors -Wextra -Wno-unused-parameter -Wno-unused-variable
CXXFLAGS = -std=c++1y -stdlib=libc++ -g -O0 $(WARNINGS) -c
LD = clang++
LDFLAGS = -std=c++1y -stdlib=libc++ -lc++abi -lpthread
ASANFLAGS = -fsanitize=address -fno-omit-frame-pointer

all: $(EXENAME)


$(EXENAME): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXENAME) `libpng-config --ldflags`

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	-rm -f *.o $(EXENAME)

.PHONY: all $(EXENAME) test clean
