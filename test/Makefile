.SUFFIXES: .cpp 
#CXX=/usr/local/gcc4/bin/g++
#CXX=/usr/local/gcc411/bin/g++
#CXX=/usr/local/gcc412/bin/g++
CXX=g++

CXX_FLAGS=-Wall -DBOOST_GIL_USE_CONCEPT_CHECK #-DNDEBUG 
CXX_CHECKSUM_FLAGS=-Wall -DBOOST_GIL_NO_IO -DBOOST_GIL_USE_CONCEPT_CHECK #-DNDEBUG 

BOOST_INCLUDE_PATH=-I../../.. -I../../../../boost_libraries
LIBJPEG_INCLUDE_PATH=-I../../../../lib/libjpeg
LIBJPEG_LIB_PATH=-L../../../../lib/libjpeg
LIBTIFF_INCLUDE_PATH=-I../../../../lib/libtiff
LIBTIFF_LIB_PATH=-L../../../../lib/libtiff
LIBPNG_INCLUDE_PATH=-I../../../../lib/libpng
LIBPNG_LIB_PATH=-L../../../../lib/libpng
LIBZ_LIB_PATH=-L../../../../lib/zlib
ALL_OBJECTS=main.o channel.o pixel.o pixel_iterator.o image.o image_io.o sample_image.o

all: performance checksum
.cpp.o:
#	${CXX} ${CXX_FLAGS} ${BOOST_INCLUDE_PATH} ${LIBJPEG_INCLUDE_PATH} ${LIBTIFF_INCLUDE_PATH} ${LIBPNG_INCLUDE_PATH} -c $<
	${CXX} ${CXX_CHECKSUM_FLAGS} ${BOOST_INCLUDE_PATH} -c $<
clean:
	-rm -f *.o *.exe 
performance:	performance.o
	${CXX} -o performance ${CXX_FLAGS} performance.o
test:	${ALL_OBJECTS}
	${CXX} -o test ${CXX_FLAGS} ${ALL_OBJECTS} ${LIBJPEG_LIB_PATH} -ljpeg ${LIBTIFF_LIB_PATH} -ltiff ${LIBPNG_LIB_PATH} -lpng ${LIBZ_LIB_PATH} -lz
checksum:	${ALL_OBJECTS}
	${CXX} -o test ${CXX_CHECKSUM_FLAGS} ${ALL_OBJECTS} 
