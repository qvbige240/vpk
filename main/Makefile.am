AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS = test
#bin_PROGRAMS = jansson
#test_LDFLAGS=-static
#DYNAMIC_LIBS= -ltimapi $(DYNAMIC_LIBS
test_LDADD =\
$(project_dir)/vpk/.libs/libvpk.a \
$(STATIC_LIBS)

#$(project_dir)/x86/lib/libtimapi.a \
#$(JANSSON_LIB) $(CURL_LIB) \
#$(project_dir)/vpk/libvpk.a

#$(project_dir)/vpk/nvt/lib/libjansson.a
#LIBS=-lnvtuctrl -lpthread -lm
#test_LDFLAGS=-L$(project_dir)/nvt/lib
LIBS=$(DYNAMIC_LIBS)
test_LDFLAGS=$(EXTRA_LDFLAGS)
test_SOURCES =\
sample.c tqueue_demo.c sample_jasson.c sample_download.c \
sample_string.c sample_md5.c md5.c sample_mq_send.c \
sample_filesys.c sample_consumer.c sample_timer.c sample_sort.c \
sample_sqlite.c sample_minheap.c sample_socket.c sample_epoll.c \
sample_actions.c sample_condition.c sample_mock.c
#sample_download.c
#sample_semaphore.c
#sample_mq_recv.c
#sample_mq_send.c
#sample_mq.c
#sample_jasson.c
#sample_00.c

bin_PROGRAMS += mqrecv
mqrecv_LDADD =\
$(project_dir)/vpk/.libs/libvpk.a \
$(STATIC_LIBS)
LIBS=$(DYNAMIC_LIBS)
mqrecv_LDFLAGS=$(EXTRA_LDFLAGS)
mqrecv_SOURCES =\
sample_mq_recv.c

bin_PROGRAMS += evioevent
evioevent_LDADD =\
$(project_dir)/vpk/.libs/libvpk.a \
$(STATIC_LIBS)
LIBS=$(DYNAMIC_LIBS)
evioevent_LDFLAGS=$(EXTRA_LDFLAGS)
evioevent_SOURCES =\
sample_evioevent.c

bin_PROGRAMS += evmsgevent
evmsgevent_LDADD =\
$(project_dir)/vpk/.libs/libvpk.a \
$(STATIC_LIBS)
LIBS=$(DYNAMIC_LIBS)
evmsgevent_LDFLAGS=$(EXTRA_LDFLAGS)
evmsgevent_SOURCES =\
sample_evmsgevent.c

bin_PROGRAMS += sql
sql_LDADD =\
$(project_dir)/vpk/.libs/libvpk.a \
$(STATIC_LIBS)
LIBS=$(DYNAMIC_LIBS)
sql_LDFLAGS=$(EXTRA_LDFLAGS)
sql_SOURCES =\
sample_sql.c

