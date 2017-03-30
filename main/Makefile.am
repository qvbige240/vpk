AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS = test
#bin_PROGRAMS = jansson
test_LDFLAGS=-static
test_LDADD =\
$(project_dir)/nvt/lib/libjansson.a \
$(project_dir)/vpk/libvpk.a
#$(project_dir)/vpk/nvt/lib/libjansson.a
#$(project_dir)/vpk/nvt/lib/libnvtuctrl.so
LIBS=-lnvtuctrl -lpthread -lm
test_LDFLAGS=-L$(project_dir)/nvt/lib
test_SOURCES =\
sample_jasson.c
#sample_00.c
