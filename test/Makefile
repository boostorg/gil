.SUFFIXES: .cpp 
#CXX=/usr/local/gcc4/bin/g++
#CXX=/usr/local/gcc411/bin/g++
#CXX=/usr/local/gcc412/bin/g++
CXX=g++

CXX_FLAGS=-Wall -DBOOST_GIL_USE_CONCEPT_CHECK #-DNDEBUG 
CXX_CHECKSUM_FLAGS=-Wall -DBOOST_GIL_NO_IO -DBOOST_GIL_USE_CONCEPT_CHECK #-DNDEBUG 

BOOST_INCLUDE_PATH=-I../../..
ALL_OBJECTS=main.o channel.o pixel.o pixel_iterator.o image.o image_io.o sample_image.o

all: performance checksum
.cpp.o:
	${CXX} ${CXX_CHECKSUM_FLAGS} ${BOOST_INCLUDE_PATH} -c $<
clean:
	-rm -f *.o *.exe 
performance:	performance.o
	${CXX} -o performance ${CXX_FLAGS} performance.o
checksum:	${ALL_OBJECTS}
	${CXX} -o test ${CXX_CHECKSUM_FLAGS} ${ALL_OBJECTS} 
