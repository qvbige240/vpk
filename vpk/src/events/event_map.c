/**
 * History:
 * ================================================================
 * 2017-11-21 qing.zou created
 *
 */

//#include <time.h>
//#include <sys/time.h>

#include "event.h"
#include "event_map.h"

typedef struct evmap_io {
	struct vpk_event_queue	events;
	unsigned short			nread;
	unsigned short			nwrite;
} evmap_io;

typedef struct evmap_notice {
	struct vpk_event_queue	events;
} evmap_notice;

static int evmap_make_space(struct event_io_map *map, int slot, int msize)
{
	if (map->nentries <= slot) {
		int nentries = map->nentries ? map->nentries : 32;
		void **tmp;

		while (nentries <= slot)
			nentries <<= 1;

		tmp = (void **)VPK_REALLOC(map->entries, nentries * msize);
		if (tmp == NULL)
			return (-1);

		memset(&tmp[map->nentries], 0, (nentries - map->nentries) * msize);

		map->nentries = nentries;
		map->entries = tmp;
	}

	return (0);
}

void evmap_iomap_init(struct event_io_map *iomap)
{
	iomap->nentries = 0;
	iomap->entries = NULL;
}

void evmap_iomap_clear(struct event_io_map *iomap)
{
	if (iomap->entries != NULL) {
		int i;
		for (i = 0; i < iomap->nentries; i++) {
			if (iomap->entries[i] != NULL)
				VPK_FREE(iomap->entries[i]);
		}
		VPK_FREE(iomap->entries);
		iomap->entries = NULL;
	}
	iomap->nentries = 0;
}

static void evmap_io_init(evmap_io *entry)
{
	TAILQ_INIT(&entry->events);
	entry->nread = 0;
	entry->nwrite = 0;
}

int evmap_io_add(vpk_evbase_t *evbase, int fd, vpk_events *ev)
{
	const struct eventop *evsel = evbase->evsel;
	struct event_io_map *iomap = &evbase->iomap;
	struct evmap_io *ctx = NULL;
	int nread, nwrite, retval = 0;
	short res = 0, old = 0;
	//vpk_events *old_ev;

	if (fd < 0)
		return 0;

	if (fd >= iomap->nentries) {
		if (evmap_make_space(iomap, fd, sizeof(struct evmap_io *)) == -1)
			return -1;
	}

	if (iomap->entries[fd] == NULL) {
		iomap->entries[fd] = VPK_CALLOC(1, sizeof(struct evmap_io)+evsel->fdinfo_len);
		if (iomap->entries[fd] == NULL)
			return -1;
		evmap_io_init((struct evmap_io *)iomap->entries[fd]);
	}
	ctx = (struct evmap_io *)iomap->entries[fd];

	nread = ctx->nread;
	nwrite = ctx->nwrite;

	if (nread)
		old |= VPK_EV_READ;
	if (nwrite)
		old |= VPK_EV_WRITE;

	if (ev->ev_events & VPK_EV_READ) {
		if (++nread == 1)
			res |= VPK_EV_READ;
	}
	if (ev->ev_events & VPK_EV_WRITE) {
		if (++nwrite == 1)
			res |= VPK_EV_WRITE;
	}
	if (nread > 0xffff || nwrite > 0xffff) {
		EVENT_LOGW("Too many events reading or writing on fd %d", (int)fd);
		return -1;
	}

	if (res) {
		void *extra = ((char*)ctx) + sizeof(struct evmap_io);
		if (evsel->add(evbase, ev->ev_fd, old, (ev->ev_events & VPK_EV_ET) | res, extra) == -1)
			return -1;
		retval = 1;
	}

	ctx->nread = (unsigned short)nread;
	ctx->nwrite = (unsigned short)nwrite;
	TAILQ_INSERT_TAIL(&ctx->events, ev, ev_io_next);

	return retval;
}

int evmap_io_del(vpk_evbase_t *evbase, int fd, vpk_events *ev)
{
	const struct eventop *evsel = evbase->evsel;
	struct event_io_map *iomap = &evbase->iomap;
	struct evmap_io *ctx = NULL;
	int nread, nwrite, retval = 0;
	short res = 0, old = 0;

	if (fd < 0)
		return 0;

	if (fd >= iomap->nentries)
		return -1;

	ctx = (struct evmap_io *)iomap->entries[fd];

	nread = ctx->nread;
	nwrite = ctx->nwrite;
	if (nread)
		old |= VPK_EV_READ;
	if (nwrite)
		old |= VPK_EV_WRITE;

	if (ev->ev_events & VPK_EV_READ) {
		if (--nread == 0)
			res |= VPK_EV_READ;
	}
	if (ev->ev_events & VPK_EV_WRITE) {
		if (--nwrite == 0)
			res |= VPK_EV_WRITE;
	}

	if (res) {
		void *extra = ((char*)ctx) + sizeof(struct evmap_io);
		if (evsel->del(evbase, ev->ev_fd, old, res, extra) == -1)
			return -1;
		retval = 1;
	}

	ctx->nread = nread;
	ctx->nwrite = nwrite;
	TAILQ_REMOVE(&ctx->events, ev, ev_io_next);

	return retval;
}

int evmap_io_active(vpk_evbase_t *evbase, int fd, short events)
{
	struct event_io_map *iomap = &evbase->iomap;
	struct evmap_io *ctx = NULL;
	vpk_events *ev = NULL;

	ctx = (struct evmap_io *)iomap->entries[fd];

	TAILQ_FOREACH(ev, &ctx->events, ev_io_next) {
		if (ev->ev_events & events)
			event_active_nolock(ev, ev->ev_events & events, 1);
	}

	return 0;
}

/** notice **/
void evmap_noticemap_init(struct event_notice_map *ctx)
{
	evmap_iomap_init(ctx);
}

void evmap_noticemap_clear(struct event_notice_map *ctx)
{
	evmap_iomap_clear(ctx);
}

static void evmap_notice_init(evmap_notice *entry)
{
	TAILQ_INIT(&entry->events);
}

int evmap_notice_add(vpk_evbase_t *base, int msg, vpk_events *ev)
{
	const struct eventop *evsel = base->evmsgsel;
	struct event_notice_map *noticemap = &base->noticemap;
	struct evmap_notice *ctx = NULL;
	int fd = msg;

	if (fd >= noticemap->nentries) {
		if (evmap_make_space(noticemap, fd, sizeof(struct evmap_notice *)) == -1)
			return -1;
	}

	if (noticemap->entries[fd] == NULL) {
		noticemap->entries[fd] = VPK_CALLOC(1, sizeof(struct evmap_notice)+evsel->fdinfo_len);
		if (noticemap->entries[fd] == NULL)
			return -1;
		evmap_notice_init((struct evmap_notice *)noticemap->entries[fd]);
	}
	ctx = (struct evmap_notice *)noticemap->entries[fd];

	if (TAILQ_EMPTY(&ctx->events)) {
		if (evsel->add(base, ev->ev_fd, 0, VPK_EV_NOTICE, NULL) == -1)
			return -1;
	}

	TAILQ_INSERT_TAIL(&ctx->events, ev, ev_notice_next);

	return 1;
}

int evmap_notice_del(vpk_evbase_t *base, int msg, vpk_events *ev)
{
	const struct eventop *evsel = base->evmsgsel;
	struct event_notice_map *noticemap = &base->noticemap;
	struct evmap_notice *ctx = NULL;
	int fd = msg;

	if (fd >= noticemap->nentries)
		return -1;

	ctx = (struct evmap_notice *)noticemap->entries[fd];

	if (TAILQ_FIRST(&ctx->events) == TAILQ_LAST(&ctx->events, vpk_event_queue)) {
		if (evsel->del(base, ev->ev_fd, 0, VPK_EV_NOTICE, NULL) == -1)
			return -1;
	}

	TAILQ_REMOVE(&ctx->events, ev, ev_notice_next);

	return 1;
}

void evmap_notice_active(vpk_evbase_t *base, int msg, int ncalls, void *data, int size)
{
	struct event_notice_map *noticemap = &base->noticemap;
	struct evmap_notice *ctx = NULL;
	vpk_events *ev;
	int fd = msg;

	ctx = (struct evmap_notice *)noticemap->entries[fd];

	TAILQ_FOREACH(ev, &ctx->events, ev_notice_next) {
		if (data && size > 0)
			vpk_event_data_set(ev, data, size);
		event_active_nolock(ev, VPK_EV_NOTICE, ncalls);
	}
}
