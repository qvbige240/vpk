AUTOMAKE_OPTIONS=foreign
platform_nt966x_sources = \
src/platform/nt966x/nt966x_eventq.c \
src/platform/nt966x/nt966x_eventq.h \
src/platform/nt966x/nt966x_log.c \
src/platform/nt966x/nt966x_log.h \
src/platform/nt966x/nt966x_nvtuctrl.c \
src/platform/nt966x/nt966x_nvtuctrl.h \
src/platform/nt966x/nt966x_serial.c \
src/platform/nt966x/nt966x_serial.h \
src/platform/nt966x/nt966x_session.c \
src/platform/nt966x/nt966x_session.h \
src/platform/nt966x/nt966x_system.c \
src/platform/nt966x/nt966x_system.h
platform_x86_sources = \
src/platform/x86/hi35xx_actuator.c \
src/platform/x86/hi35xx_actuator.h \
src/platform/x86/x86_eventq.c \
src/platform/x86/x86_eventq.h \
src/platform/x86/x86_log.c \
src/platform/x86/x86_log.h \
src/platform/x86/x86_serial.c \
src/platform/x86/x86_serial.h \
src/platform/x86/x86_session.c \
src/platform/x86/x86_session.h \
src/platform/x86/x86_system.c \
src/platform/x86/x86_system.h

#src/platform/x86/x86_actuator.c 
#src/platform/x86/x86_actuator.h 

if X8666
platform_sources = $(platform_x86_sources)
else
platform_sources = $(platform_nt966x_sources)
endif
vpk_all_sources = \
inc/vpk_action.h \
inc/vpk_constants.h \
inc/vpk_event.h \
inc/vpk_filesys.h \
inc/vpk.h \
inc/vpk_ipc.h \
inc/vpk_libc.h \
inc/vpk_logging.h \
inc/vpk_md5.h \
inc/vpk_mmap.h \
inc/vpk_nvtuctrlc.h \
inc/vpk_queues.h \
inc/vpk_serialc.h \
inc/vpk_timer.h \
inc/vpk_typedef.h \
inc/vpk_util.h \
src/vpk_actions.c \
src/vpk_actions.h \
src/vpk_actuator.c \
src/vpk_actuator.h \
src/vpk_cdevice.c \
src/vpk_cdevice.h \
src/vpk_ieventq.c \
src/vpk_ieventq.h \
src/vpk_ifilesys.c \
src/vpk_ifilesys.h \
src/vpk_ipc.c \
src/vpk_ipc_sem.c \
src/vpk_ipc_sem.h \
src/vpk_ipc_shm.c \
src/vpk_ipc_shm.h \
src/vpk_libc.c \
src/vpk_logger.c \
src/vpk_logger.h \
src/vpk_md5.c \
src/vpk_mmap.c \
src/vpk_nvtuctrl.c \
src/vpk_nvtuctrl.h \
src/vpk_object.c \
src/vpk_object.h \
src/vpk_serial.c \
src/vpk_serial.h \
src/vpk_session.c \
src/vpk_session.h \
src/vpk_system.c \
src/vpk_system.h \
src/vpk_tqueue.c \
src/vpk_tqueue.h \
src/vpk_util.c \
src/events/epoll.c \
src/events/event.c \
src/events/event.h \
src/events/event_map.c \
src/events/event_map.h \
src/events/event_msg.c \
src/events/event_msg.h \
src/events/event_thread.c \
src/events/event_thread.h \
src/database/vpk_sql.h \
src/database/vpk_sql.c \
src/database/vpk_iterator.h \
src/database/vpk_iterator.c \
src/database/sql_value.h \
src/database/sql_value.c \
src/database/sql/sql_sqlite3.h \
src/database/sql/sql_sqlite3.c \
src/database/sql/sql_impl.h \
$(platform_sources)
ipc_all_sources =\
inc/vpk_filesys.h \
inc/vpk_ipc.h \
src/vpk_ifilesys.c \
src/vpk_ifilesys.h \
src/vpk_ipc.c \
src/vpk_ipc_sem.c \
src/vpk_ipc_sem.h \
src/vpk_ipc_shm.c \
src/vpk_ipc_shm.h \
src/vpk_libc.c

lib_LTLIBRARIES=libvpk.la
libvpk_la_LDFLAGS = -fPIC
#libvpk_la_LIBADD =\
#$(project_dir)/x86/lib/libjansson.a
libvpk_la_SOURCES=$(vpk_all_sources)
lib_LTLIBRARIES += libipc.la
libipc_la_SOURCES=$(ipc_all_sources)

#if USE_SHARED
#lib_LTLIBRARIES=libvpk.la
#libvpk_la_SOURCES=$(vpk_all_sources)
#lib_LTLIBRARIES += libipc.la
#libipc_la_SOURCES=$(ipc_all_sources)
#else
#noinst_LIBRARIES=libvpk.a
#libvpk_a_SOURCES=$(vpk_all_sources)
#noinst_LIBRARIES += libipc.a
#libipc_a_SOURCES=$(ipc_all_sources)
#endif

include_HEADERS =\
inc/macro_event.h \
inc/vpk_action.h \
inc/vpk_constants.h \
inc/vpk_event.h \
inc/vpk_events.h \
inc/vpk_filesys.h \
inc/vpk.h \
inc/vpk_heap.h \
inc/vpk_ipc.h \
inc/vpk_libc.h \
inc/vpk_list.h \
inc/vpk_logging.h \
inc/vpk_md5.h \
inc/vpk_minheap.h \
inc/vpk_mmap.h \
inc/vpk_nvtuctrlc.h \
inc/vpk_queues.h \
inc/vpk_serialc.h \
inc/vpk_typedef.h \
inc/vpk_util.h \
src/database/vpk_sql.h \
src/database/vpk_iterator.h \
src/database/sql_value.h \
src/database/sql/sql_impl.h
