INCLUDE=-I/usr/include/mysql/
LIBS=-L$/usr/local/mysql/lib 
CFLAGS = -g -Wall -fPIC
TARGET=../lib/libframecommon.a
		
BINARY = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all:$(TARGET)
	@cp *.h ../include/framecommon

%.o:%.cpp
	$(CXX) $< -o $@ -c $(INCLUDE) $(CFLAGS) 

clean:
	@rm -rf ${BINARY} ${TARGET}
	@rm -f ../include/framecommon/*

$(TARGET):$(BINARY)
	ar rcs $@ $^
