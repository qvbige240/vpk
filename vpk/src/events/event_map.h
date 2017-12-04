/**
 * History:
 * ================================================================
 * 2017-11-21 qing.zou created
 *
 */
#ifndef EVENT_MAP_H
#define EVENT_MAP_H

//#include <sys/queue.h>		/* tailq */

#include "vpk_typedef.h"
#include "vpk_logging.h"
//#include "vpk_minheap.h"

TIMA_BEGIN_DELS

void evmap_iomap_init(struct event_io_map *iomap);

void evmap_iomap_clear(struct event_io_map *iomap);

int evmap_io_add(vpk_evbase_t *evbase, int fd, vpk_events *ev);

int evmap_io_del(vpk_evbase_t *evbase, int fd, vpk_events *ev);

int evmap_io_active(vpk_evbase_t *evbase, int fd, short events);

TIMA_END_DELS

#endif // EVENT_MAP_H
