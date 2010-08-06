CPP      = g++
CPPFLAGS = -fPIC -O0 -g -Wall -W
INCLUDES = 
SRCS     = logo.cpp LogoDetect.cpp
OBJS     = $(SRCS:.cpp=.o)
LIBS     = ./libsiftfast.a ./libANN.a -lhighgui

all: $(OBJS)
	g++ -shared -o logo.so $(OBJS) $(LIBS)

clean:
	rm -f *.so *.o

.cpp.o:
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $< -o $@
