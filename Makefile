CPP=g++
CPPFLAGS=-g -std=c++0x 
#CPPFLAGS=-g -fopenmp -std=c++0x 
#CPPFLAGS=-O9 -std=c++0x
  
OBJS=basics.o delta.o utils.o partialSums.o Dualsorted.o omptest.o#parse_invlist.o
LIBCDS=libs/libcds_extended
LIB=$(LIBCDS)/lib/libcds.a -lboost_serialization
INCLUDES=-I$(LIBCDS)/includes/
SRC=src/
DEBUG=Debug/

all: $(OBJS) #dslib
	$(CPP) $(CPPFLAGS) $(INCLUDES) -o  $(DEBUG)test  $(DEBUG)omptest.o $(LIB)
	
basics.o : $(SRC)basics.c $(SRC)basics.h
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $(SRC)basics.c -o $(DEBUG)basics.o
	
delta.o : $(SRC)delta.c $(SRC)delta.h $(SRC)basics.h
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $(SRC)delta.c -o $(DEBUG)delta.o
	
utils.o : $(SRC)utils.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $(SRC)utils.cpp -o $(DEBUG)utils.o
	
partialSums.o : $(SRC)partialSums.cpp $(SRC)delta.c
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $(SRC)partialSums.cpp -o $(DEBUG)partialSums.o
	
#parse_invlist.o : $(SRC)parse_invlist.cpp $(SRC)utils.cpp
#	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $(SRC)parse_invlist.cpp -o $(DEBUG)parse_invlist.o
	
Dualsorted.o : $(SRC)Dualsorted.cpp $(SRC)Dualsorted.h $(SRC)partialSums.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $(SRC)Dualsorted.cpp -o $(DEBUG)Dualsorted.o
omptest.o : $(SRC)Dualsorted.cpp $(SRC)Dualsorted.h $(SRC)partialSums.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $(SRC)omptest.cpp -o $(DEBUG)omptest.o
#dslib: Dualsorted.o
#	ar -rc $(DEBUG)Dualsorted.a $(DEBUG)Dualsorted.o $(LIB)
clean:
	    rm -rf $(DEBUG)* $(DEBUG)test
