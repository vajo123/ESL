CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lavcodec -lavformat -lavutil -lswscale -lsfml-audio -lsfml-system `pkg-config --cflags --libs opencv4`

SRCS = algoritam.cpp readsrt.cpp puttext.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = algoritam

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I /usr/include/opencv4 -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)


