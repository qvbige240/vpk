
CFLAGS = -g
DEFINED = #-D _VERSION
LIBS = 
CC = g++
INCLUDES = -I./
OBJS= main.o flyweight.o flyweight_factory.o
TARGET= main
all:$(TARGET)
 
$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
 
.SUFFIXES:.o .h
.SUFFIXES:.cpp .o
.cpp.o:
	$(CC) $(DEFINED) -c $(CFLAGS) -o $@ $<
 
ok:
	./$(TARGET)
clean:
	rm -f $(OBJS) $(TARGET) core *.log
	