CFLAGS = `pkg-config opencv --cflags`
LIBS = `pkg-config opencv --libs`

LBtool : LBtool.cpp lbt.cpp
	g++ LBtool.cpp lbt.cpp -g -o LBtool $(CFLAGS) $(LIBS)
clean :
	rm LBtool
