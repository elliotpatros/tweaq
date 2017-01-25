# targets
so: change_gain.so mix_to_mono.so normalize.so deinterleave.so fade_in.so

# the compiler
CC = clang

# compiler flags
CFLAGS = -c -O2 -flto -fPIC -Wall -I../../
LIBFLAGS = -O2 -flto -L../../StaticLibs/libsndfile/lib -lsndfile

# recipe
%.so: %.cpp
	$(CC) $(CFLAGS) ../../tweaqapi.cpp
	$(CC) $(CFLAGS) $*.cpp
	$(CC) -shared -O2 -flto -fPIC -o ../lib$*.so $(LIBFLAGS) tweaqapi.o $*.o
	rm $*.o tweaqapi.o

# clean recipe
clean: ;    rm ../lib*.so
