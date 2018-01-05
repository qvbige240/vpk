/*
 * History:
 * ================================================================
 * 2017-04-02 qing.zou created
 *
 */
#ifndef VPK_ACTION_H
#define VPK_ACTION_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

typedef enum _VpkActionType
{
	VPK_ACTION_START_TAG = 0,				// without use
	VPK_ACTION_GPSINFO = 1,
	VPK_ACTION_ACCEINFO,
	VPK_ACTION_SNAPSHOT,
	VPK_ACTION_MOVIEREC,
	VPK_ACTION_MOVIELEN,
	VPK_ACTION_RECSTATE,
	VPK_ACTION_DEVINFO,
	VPK_ACTION_VIDEOREC,
	VPK_ACTION_GETFILENAME,
	VPK_ACTION_UNBINDEVICE,

	// menu get
	VPK_ACTION_MENU_PICSIZEGET,			// photo_resolution	
	VPK_ACTION_MENU_RECSIZEGET,			// video_resolution
	VPK_ACTION_MENU_CYCRECGET,			// video_time
	VPK_ACTION_MENU_GSENSORGET,			// vibration_sensor
	VPK_ACTION_MENU_PARKGSENSORGET,		// park_sensor
	VPK_ACTION_MENU_PARKMONITORGET,		// parking_mode
	VPK_ACTION_MENU_POWEROFFVOLTGET,	// off_voltage
	VPK_ACTION_MENU_AUDIOGET,			// record_sound
	VPK_ACTION_MENU_HDRGET,				// HDR
	VPK_ACTION_MENU_TIMESTAMPGET,		// time_watermark
	VPK_ACTION_MENU_UPDATEGET,			// upgrade			
	VPK_ACTION_MENU_POWERSTATEGET,		// switch_machine
	VPK_ACTION_MENU_CRASHSTATGET,		// car_warning
	VPK_ACTION_MENU_DRIVEBEHAVIORGET,	// driving
	VPK_ACTION_MENU_DEVSTATGET,			// abnormal_equipment
	VPK_ACTION_MENU_FORMATSD_NOP,		// format_tf		//
	VPK_ACTION_MENU_FACTORYSET_NOP,		// factory_reset	//
	VPK_ACTION_MENU_VERSIONGET,			// version			//
	VPK_ACTION_MENU_WIFIPHRASEGET,		// wifiphrase
	// menu set
	VPK_ACTION_MENU_PICSIZESET,			// photo_resolution	
	VPK_ACTION_MENU_RECSIZESET,			// video_resolution
	VPK_ACTION_MENU_CYCRECSET,			// video_time
	VPK_ACTION_MENU_GSENSORSET,			// vibration_sensor
	VPK_ACTION_MENU_PARKGSENSORSET,		// park_sensor
	VPK_ACTION_MENU_PARKMONITORSET,		// parking_mode
	VPK_ACTION_MENU_POWEROFFVOLTSET,	// off_voltage
	VPK_ACTION_MENU_AUDIOSET,			// record_sound
	VPK_ACTION_MENU_HDRSET,				// HDR
	VPK_ACTION_MENU_TIMESTAMPSET,		// time_watermark
	VPK_ACTION_MENU_UPDATESET,			// upgrade			
	VPK_ACTION_MENU_POWERSTATESET,		// switch_machine
	VPK_ACTION_MENU_CRASHSTATSET,		// car_warning
	VPK_ACTION_MENU_DRIVEBEHAVIORSET,	// driving
	VPK_ACTION_MENU_DEVSTATSET,			// abnormal_equipment
	VPK_ACTION_MENU_FORMATSD,			// format_tf		//
	VPK_ACTION_MENU_FACTORYSET,			// factory_reset	//
	VPK_ACTION_MENU_VERSIONGET_NOP,		// version			//
	VPK_ACTION_MENU_WIFIPHRASESET,		// wifiphrase

	VPK_ACTION_QRCODE,
//	VPK_ACTION_UPDATE_FREQGET,
	VPK_ACTION_UPDATE_WHETHER_DOWNLOAD,
	VPK_ACTION_UPDATE_WHETHER_UPGRADE,

	// tencent iot 
	VPK_ACTION_IOTPIDGET,
	VPK_ACTION_IOTIDSET,
	VPK_ACTION_IOTLICENCESET,

	VPK_ACTION_POWER_OFF,

	VPK_ACTION_MAX,
} VpkActionType;

typedef struct _VpkAction
{
	VpkActionType	type;
	char*			param;
	char*			recvbuf;
	uint32_t		recvsize;
	char			priv[ZERO_LEN_ARRAY];
}VpkAction;

typedef int (*VpkActionCallback)(void *ctx, void *data);

/* 256 buffer */
VPKAPI VpkAction* vpk_action_create(VpkActionType type, void *param, void *recvbuf, uint32_t recvsize);
VPKAPI int vpk_action_param_set(VpkAction* thiz, VpkActionType type, void *param);
VPKAPI int vpk_action_exec(VpkAction* thiz, VpkActionCallback callback, void *ctx);
VPKAPI int vpk_action_destroy(VpkAction* thiz);
// 
// /**
//  * Create a serial object.
//  *
//  * @param tty		 The char device.
//  * @param baud_speed The serial baud speed, 2400, 4800, 9600...
//  * @param data_bits  The data bit value: 5, 6, 7, 8
//  * @param parity_bit The parity bit value, 'N': no parity, 'O': odd parity, 'E': even parity
//  * @param stop_bit   The stop bit: 1, 2.
//  *
//  * @return The serial object.
//  */
// VPKAPI vpk_serial_t* vpk_serial_create(const char* tty, uint32_t baud_speed,
// 									   uint32_t data_bits, uint8_t parity_bit, uint32_t stop_bit);
// /**
//  * Open a serial object.
//  *
//  * @param thiz	The target serial device.
//  *
//  * @return 0 success, -1 fail.
//  */
// VPKAPI int vpk_serial_open(vpk_serial_t* thiz);
// 
// /* Return 0 means to be successful, breaking out of the 'vpk_serial_read'. */
// typedef int (*SerialVisitFunc)(void* ctx, void* data);
// /**
//  * Read data from serial device.
//  *
//  * @param thiz		The serial device object.
//  * @param buf		The buffer, receive the data from serial.
//  * @param nbytes	The data size, need to read from serial and the buffer size is max size.
//  * @param timout	The timeout waiting time (s).
//  * @param visit		A callback function which should be NULL or Not is to register a func for  
//  *					returning when it returns 0, the type of visit is 'SerialVisitFunc'.
//  *
//  * @return The data size, read from serial.
//  */
// VPKAPI int vpk_serial_read(vpk_serial_t* thiz, void *buf, size_t nbytes, int timout, SerialVisitFunc visit);
// 
// /**
//  * Write data to a serial device.
//  *
//  * @param thiz		The serial device object.
//  * @param buf		The buffer, write the target data to serial.
//  * @param nbytes	The data size, need write to the serial.
//  * @param timout	value 0.
//  *
//  * @return The written data size.
//  */
// VPKAPI int vpk_serial_write(vpk_serial_t* thiz, void* buf, ssize_t nbytes, int timout);
// 
// /**
//  * Close a serial object.
//  *
//  * @param thiz	The target serial device.
//  *
//  * @return 0 success, -1 fail.
//  */
// VPKAPI int vpk_serial_close(vpk_serial_t* thiz);
// 
// /**
//  * Set a serial object config.
//  *
//  * @param thiz		 The serial device object.
//  * @param baud_speed The serial baud speed, 2400, 4800, 9600...
//  * @param data_bits  The data bit value: 5, 6, 7, 8
//  * @param parity_bit The parity bit value, 'N': no parity, 'O': odd parity, 'E': even parity
//  * @param stop_bit   The stop bit: 1, 2.
//  *
//  * @return 0 success, -1 fail.
//  */
// VPKAPI int vpk_serial_opt_set(vpk_serial_t* thiz, unsigned int baud_speed, 
// 							  unsigned int data_bits, unsigned char parity_bit, unsigned int stop_bit);
// 
// /**
//  * Destroy a serial object.
//  *
//  * @param thiz	The target serial device.
//  *
//  * @return 0 success, -1 fail.
//  */
// VPKAPI int vpk_serial_destroy(vpk_serial_t* thiz);

TIMA_END_DELS

#endif //VPK_ACTION_H
