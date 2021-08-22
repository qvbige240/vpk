
CFLAGS = -g
DEFINED = #-D _VERSION
LIBS = -lpthread
CC = g++
INCLUDES = -I./
#OBJS= plugin_main.o plugin_dl.o
OBJS= file_protocol.o \
file_serializer.o \
main.o \
operation_file.o \
operation_file_impl.o


TARGET= main
all:$(TARGET)
 
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
 
.SUFFIXES:.o .h
.SUFFIXES:.c .o
.c.o:
	$(CC) $(DEFINED) -c $(CFLAGS) -o $@ $<
 
ok:
	./$(TARGET)
clean:
	rm -f $(OBJS) $(TARGET) core *.log
