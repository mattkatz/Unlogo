CPP      = g++
CPPFLAGS = -fPIC -O0 -g -Wall -W `pkg-config --cflags opencv`
INCLUDES = -I./include
SRCS     = src/logo.cpp src/LogoDetect.cpp
OBJS     = $(SRCS:.cpp=.o)
LIBS     = -lsiftfast -lANN `pkg-config --libs opencv`


all: $(OBJS)
	g++ -shared -o logo.so $(OBJS) $(LIBS)

clean:
	rm -f *.so src/*.o

.cpp.o:
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $< -o $@
