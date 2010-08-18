CPP      = g++
CPPFLAGS = -fPIC -O0 -g -Wall -W -L/usr/local/lib 
INCLUDES = -I/usr/local/include -I/usr/local/include/opencv 
SRCS     = unlogo.cpp LogoFilter.cpp
OBJS     = $(SRCS:.cpp=.o)
LIBS     = -lopencv_highgui -lopencv_calib3d -lopencv_core -lopencv_features2d -lopencv_imgproc -lopencv_video


all: $(OBJS)
	g++ -shared -o unlogo.so $(OBJS) $(LIBS)


clean:
	rm -f *.so *.o

.cpp.o:
	$(CPP) -c $(CPPFLAGS) $(INCLUDES) $< -o $@


