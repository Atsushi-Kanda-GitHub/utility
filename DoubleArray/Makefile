OBJS    = DoubleArray.o da_test.o
CPPFLAG = -Wall -O3

da:$(OBJS)
	g++-11 -o da $(OBJS)

DoubleArray.o: DoubleArray.cpp
	g++-11 $(CPPFLAG) -c DoubleArray.cpp

da_test.o: da_test.cpp
	g++-11 $(CPPFLAG) -c da_test.cpp

clean:
	rm -f da $(OBJS)

