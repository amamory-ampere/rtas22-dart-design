appname = sobel
#fred_dir = /fredsys/fred_lib
fred_dir = /user
srcs = $(wildcard ./src/*.c)
# cpp compilation is not working yet
cxx_srcs += $(wildcard ./src/*.cpp)
objs = $(srcs:.c=.o) $(cxx_srcs:.cpp=.o)
deps = $(objs:.o=.d)

CFLAGS += -I $(fred_dir) -I $(fred_dir)/include -g  # -Wall  -Werror
CXXFLAGS += $(CFLAGS)
LDFLAGS += -L $(fred_dir) -L $(fred_dir)/lib -lfred

$(appname): $(objs)
	#make -C $(fred_dir) CC=$(CC)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	#make -C $(fred_dir) clean
	rm -f $(objs) $(deps) $(appname)
