# targets
so: change_gain.so mix_to_mono.so normalize.so split_to_mono.so \
	strip_silence_at_end.so strip_silence_at_start.so fade_in.so

# the compiler
CC = gcc

# compiler flags
CFLAGS = -c -fPIC -Wall -I../../
LIBFLAGS = -L../../StaticLibs/libsndfile/lib -lsndfile

# recipe
%.so: %.cpp
	$(CC) $(CFLAGS) ../../m_tweaq.cpp
	$(CC) $(CFLAGS) $*.cpp
	$(CC) -shared -fPIC -o ../lib$*.so $(LIBFLAGS) m_tweaq.o $*.o
	rm $*.o m_tweaq.o

# clean recipe
clean: ;    rm ../lib*.so
