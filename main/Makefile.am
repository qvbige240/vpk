AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS = test
#bin_PROGRAMS = jansson
#test_LDFLAGS=-static
test_LDADD =\
$(JANSSON_LIB) $(CURL_LIB) \
$(project_dir)/vpk/libvpk.a
#$(project_dir)/vpk/nvt/lib/libjansson.a
#LIBS=-lnvtuctrl -lpthread -lm
#test_LDFLAGS=-L$(project_dir)/nvt/lib
LIBS=$(DYNAMIC_LIBS)
test_LDFLAGS=$(EXTRA_LDFLAGS)
test_SOURCES =\
sample.c tqueue_demo.c sample_jasson.c sample_download.c \
sample_string.c sample_md5.c md5.c sample_mq_send.c \
sample_filesys.c sample_consumer.c sample_timer.c sample_sort.c
#sample_download.c
#sample_semaphore.c
#sample_mq_recv.c
#sample_mq_send.c
#sample_mq.c
#sample_jasson.c
#sample_00.c
