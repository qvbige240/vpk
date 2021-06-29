
CFLAGS = -g
DEFINED = #-D _VERSION
LIBS = -ldl
CC = g++
INCLUDES = -I./
OBJS= plugin_main.o plugin_dl.o
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
	