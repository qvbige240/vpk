
#LDADD = $(top_builddir)/vpk/.libs/libvpk.a $(STATIC_LIBS)
LDADD = $(top_builddir)/vpk/libvpk.la $(STATIC_LIBS)
#AM_LDFLAGS = $(EXTRA_LDFLAGS) -L$(top_builddir)/vpk/.libs
AM_LDFLAGS = -L$(top_builddir)/vpk/.libs
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/vpk/inc -I$(top_srcdir)/vpk/src

noinst_PROGRAMS = test mqrecv evioevent evmsgevent net ttl
LIBS = $(DYNAMIC_LIBS)

test_SOURCES =\
sample.c tqueue_demo.c sample_jasson.c sample_download.c \
sample_string.c sample_md5.c md5.c sample_mq_send.c \
sample_filesys.c sample_consumer.c sample_timer.c sample_sort.c \
sample_sqlite.c sample_minheap.c sample_socket.c sample_epoll.c \
sample_actions.c sample_condition.c sample_mock.c sample_des.c \
sample_aes.c
mqrecv_SOURCES = sample_mq_recv.c
evioevent_SOURCES = sample_evioevent.c
evmsgevent_SOURCES = sample_evmsgevent.c
net_SOURCES = sample_net.c
ttl_SOURCES = sample_ttl.c

#sql_LDADD = $(SQLITE3_LIBS) $(LDADD)
if HAVE_SQLITE3
    noinst_PROGRAMS += sql
    sql_CFLAGS = $(SQLITE3_CFLAGS)
    sql_SOURCES = sample_sql.c
    
    test_CFLAGS = $(SQLITE3_CFLAGS)
    
    LDADD += $(SQLITE3_LIBS)
    AM_CPPFLAGS += -I$(top_srcdir)/vpk/src/database 
endif











