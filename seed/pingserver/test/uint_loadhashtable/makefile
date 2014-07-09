FRAMECOMMON=../../../../framecommon
SRVFRAME=../../../../srvframe
COMMON=../../../common

INCLUDE=-I/usr/include/mysql/ -I$(FRAMECOMMON)/include 
LIBS=-L$(FRAMECOMMON)/lib -lframecommon /usr/lib64/mysql/libmysqlclient_r.a -lz -pthread -lssl -luuid 

BINARY=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
FLAG=-g -Wall
TARGET=loadhash
all:$(TARGET)

$(TARGET):$(BINARY)
	g++ $(FLAG) $^ -o $@ $(LIBS) 

%.o:%.cpp
	g++ $(FLAG) $< -o $@ -c $(INCLUDE) 

clean:
	rm -f $(TARGET)
	rm -f *.o
