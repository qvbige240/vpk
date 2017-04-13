/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */

#include "vpk_logging.h"

#include "vpk_serial.h"
#include "vpk_cdevice.h"

static cdevice_register_t container = {0};

int register_serial_device(const char* standard, const session_ops* sops)
{
	return_val_if_fail(standard != NULL && sops != NULL, -1);

	return vpk_register_cdevice(&container, standard, sops, "session");
}

static vpk_serial_t* vpk_serial_load(const char* tty)	//..whether the tty is being used
{
	vpk_serial_t* serial = NULL;
	const session_ops *s_ops = NULL;
	return_val_if_fail(tty != NULL, NULL);

	s_ops = (const session_ops *)vpk_find_cdevice(&container, "serial");
	if (s_ops)
	{
		serial = (vpk_serial_t*)s_ops->load((void*)tty);
		return_val_if_fail(serial != NULL, NULL);
		serial->s_ops = s_ops;
		memset(serial->name, 0, _countof(serial->name));
		//strlcpy(serial->name, tty, _countof(serial->name));
		memcpy(serial->name, tty, sizeof(serial->name));
		LOG_D("serial->name: %s, tty: %s", serial->name, tty);
	}

	return serial;
}

VPKAPI vpk_serial_t* vpk_serial_create(const char* tty, uint32_t baud_speed,
									   uint32_t data_bits, uint8_t parity_bit, uint32_t stop_bit)
{
	//check, whether the tty is being used
	//if (vpk_tty_used_exist(tty))
	vpk_serial_t* serial = vpk_serial_load(tty);
	return_val_if_fail(serial != NULL, NULL);

	serial->baud_speed = baud_speed;
	serial->data_bits  = data_bits;
	serial->parity_bit = parity_bit;
	serial->stop_bit   = stop_bit;

	return serial;
}

VPKAPI int vpk_serial_open(vpk_serial_t* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL, -1);

	return thiz->s_ops->open((vpk_session_t *)thiz, thiz->name);
}

VPKAPI int vpk_serial_read(vpk_serial_t* thiz, void *buf, size_t nbytes, int timout, SerialVisitFunc visit)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL && buf != NULL, -1);

	if (nbytes > 0)
	{
		return thiz->s_ops->read((vpk_session_t *)thiz, buf, nbytes, timout, (void*)visit);
	}

	return 0;
}

VPKAPI int vpk_serial_write(vpk_serial_t* thiz, void* buf, ssize_t nbytes, int timout)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL, -1);

	return thiz->s_ops->write((vpk_session_t *)thiz, buf, nbytes, timout);
}

VPKAPI int vpk_serial_close(vpk_serial_t* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL, -1);

	return thiz->s_ops->close((vpk_session_t *)thiz);
}

VPKAPI int vpk_serial_opt_set(vpk_serial_t* thiz, unsigned int baud_speed, 
							  unsigned int data_bits, unsigned char parity_bit, unsigned int stop_bit)
{
	return_val_if_fail(thiz != NULL && thiz->s_ops != NULL && thiz->fd >= 0, -1);

	thiz->baud_speed = baud_speed;
	thiz->data_bits  = data_bits;
	thiz->parity_bit = parity_bit;
	thiz->stop_bit   = stop_bit;

	return thiz->s_ops->optset((vpk_session_t *)thiz);
}

VPKAPI int vpk_serial_destroy(vpk_serial_t* thiz)
{
	if (thiz && thiz->destruct)
	{
		return thiz->destruct(thiz);
	}

	return 0;
}
