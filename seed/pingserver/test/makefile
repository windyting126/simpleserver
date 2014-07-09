#FRAMECOMMON=../framecommon
#SRVFRAME=../srvframe
INCLUDE=-I/usr/include/mysql/ -I$(FRAMECOMMON)/include
LIBS=-L$(FRAMECOMMON)/lib -lframecommon /usr/lib64/mysql/libmysqlclient_r.a -lz -pthread -lssl -luuid 
CFLAGS = -g -Wall -fPIC
BINARY = $(patsubst %.cpp,%,$(wildcard *.cpp))
all:$(BINARY)

%:%.cpp
	$(CXX) $< -o $@  $(INCLUDE) $(LIBS) $(CFLAGS) 

clean:
	@rm -rf ${BINARY}

