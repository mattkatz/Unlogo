CPP      = g++
CPPFLAGS = -fPIC -O0 -g -Wall -W `pkg-config --cflags opencv`
INCLUDES = -I./include
SRCS     = src/logo.cpp src/LogoDetect.cpp
OBJS     = $(SRCS:.cpp=.o)
LIBS     = ./lib/libsiftfast.a ./lib/libANN.a `pkg-config --libs opencv`


all: $(OBJS)
	g++ -shared -o logo.so $(OBJS) $(LIBS)

clean:
	rm -f *.so *.o

.cpp.o:
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $< -o $@
