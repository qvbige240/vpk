AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS = test
test_LDFLAGS=-static
test_LDADD =\
$(project_dir)/vpk/libvpk.a
#$(project_dir)/vpk/nvt/lib/libnvtuctrl.so
LIBS=-lnvtuctrl -lpthread
test_LDFLAGS=-L$(project_dir)/nvt/lib
test_SOURCES =\
sample_00.c
