/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */

#include "vpk_system.h"
#include "vpk_ieventq.h"
#include "vpk_logging.h"

const eventq_ops* eops = NULL;

int vpk_eventq_init(void)
{
	vpk_system_t* sys = vpk_system();
	return_val_if_fail(sys != NULL, -1);

	eops = sys->sys_kset->get_eventq_ops();
	return_val_if_fail(eops != NULL, -1);

// 	eventq = eops->init();
// 	return_val_if_fail(eventq != NULL, -1);

	return 0;
}

vpk_eventq_t* vpk_eventq_open(const char* name, const char* mode)
{
	vpk_eventq_t* eventq = NULL;
	if (eops)
	{
		eventq = eops->init(name, mode);
		return_val_if_fail(eventq != NULL, NULL);

		eventq->e_ops = eops;
	}
	else
		LOG_E("NULL pointer at eventq ops!");

	return eventq;
}

int vpk_eventq_empty(const vpk_eventq_t* queue)
{
	return 0;
}

int vpk_eventq_recv(vpk_eventq_t *queue, vpk_event_t* e)
{
	return_val_if_fail(queue != NULL && e != NULL, -1);

	return queue->e_ops->recv(queue, e);
}

int vpk_eventq_post(vpk_eventq_t *queue, vpk_event_t* e)
{
	return_val_if_fail(queue != NULL && e != NULL, -1);

	return queue->e_ops->post(queue, e);
}

int vpk_eventq_close(vpk_eventq_t *queue)
{
	return_val_if_fail(queue != NULL, -1);

	return queue->e_ops->close(queue);
}

int vpk_eventq_destroy(vpk_eventq_t *queue)
{
	if (queue && queue->destruct)
	{
		return queue->destruct(queue);
	}

	return 0;
}
