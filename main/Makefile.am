AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS = test
#bin_PROGRAMS = jansson
#test_LDFLAGS=-static
test_LDADD =\
$(JANSSON_LIB) \
$(project_dir)/vpk/libvpk.a
#$(project_dir)/vpk/nvt/lib/libjansson.a
#LIBS=-lnvtuctrl -lpthread -lm
#test_LDFLAGS=-L$(project_dir)/nvt/lib
LIBS=$(DYNAMIC_LIBS)
test_LDFLAGS=$(EXTRA_LDFLAGS)
test_SOURCES =\
sample_jasson.c
#sample_00.c
