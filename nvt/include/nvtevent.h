/**

    This header file of NvtEVENT APIs.

    This header file of NvtEVENT APIs for query event status

    @file       nvtevent.h
    Copyright   Novatek Microelectronics Corp. 2014.  All rights reserved.

*/

#ifndef _NVTEVENT_H
#define _NVTEVENT_H
#include <stdio.h>
#include <time.h>
#include <nvtcfg_define.h>
#ifdef __cplusplus
extern "C" {
#endif

#define CMD_MAX_SIZE 256
//#define EVT_NVTSYSTEM_EVT_PORT 8899
#define EVT_SHM_KEY 9988

#define EVT_TRIGGER_ERR  "TRIGGER_ERROR"
#define EVT_ALARM_HIGH "HIGH"
#define EVT_ALARM_LOW  "LOW"
#define EVT_ALARM1_NAME "DI0"    //digital input
#define EVT_MOTION1_NAME "MD0"   //motion detection
#define EVT_NETWORK1_NAME "NL0"  //network loss
#define EVT_ALARM_OPEN "OPEN"
#define EVT_ALARM_GROUNDED "GROUNDED"


#define EVT_NOTIFY "NOTIFY"
#define EVT_NOTIFY_MOTION "NOTIFY_MOTION"
#define EVT_NOTIFY_DIGITAL_INPUT "NOTIFY_DIGITAL_INPUT"
#define EVT_NOTIFY_NETWORK_LOSS "NOTIFY_NETWORK_LOSS"
#define EVT_ALARM "ALARM"
#define EVT_MOTION "MOTION"
#define EVT_EVENT_CFG_PATH "/etc/event.conf"
#define EVT_FTP_CFG_PATH "/etc/event_ftp.conf"
#define EVT_SMTP_CFG_PATH "/etc/event_smtp.conf"
#define EVT_SD_CFG_PATH "/etc/event_sd.conf"
#define EVT_HTTP_CFG_PATH "/etc/event_http.conf"
#define EVT_ISCSI_CFG_PATH "/etc/event_iscsi.conf"
#define EVT_CURL_FTP_SEND_CMD "curl --ftp-create-dirs -T %s %s"
#define EVT_CURL_HTTP_SEND_CMD "curl %s %s \"%s\""

#define EVT_ALARM_OUTPUT_DIRECTION "/sys/class/gpio/gpio10/direction"
#define EVT_ALARM_OUTPUT_VALUE "/sys/class/gpio/gpio10/value"

#define EVT_NOTIFY_EVENT_QUEUE_NAME "/NvtNotifyEventMq"
//#define EVT_TMP_PATH "/mnt/sd/tmp"
#define EVT_TMP_PATH "/tmp"
#define EVT_TMP_PATH_SD "/mnt/sd/tmp"
//#define EVT_SNAPSHOT_TMP_PATH "/mnt/sd/tmp"
//#define EVT_LOG_TMP_PATH "/mnt/sd/tmp"
#define EVT_SD_PATH_ROOT "/mnt/sd/event"
//#define EVT_SMTP_FILE_PATH_ROOT "/mnt/sd/tmp/%s"
#define EVT_SMTP_FILE_NAME "smtp_file"

#define EVT_ISCSI_PATH_ROOT "/mnt/iscsi"
#define EVT_ISCSI_ERROR_LOG_PATH "/tmp/iscsi_error_log"
#define EVT_ISCSI_INFO_LOG_PATH "/tmp/iscsi_info_log"
#define EVT_ISCSI_START_ISCSID "/sbin/iscsid"
#define EVT_ISCSI_DISCOVERY_CMD "iscsiadm -m discovery -t st -p %s 2> %s 1> %s"
#define EVT_ISCSI_CONNECT_CMD "iscsiadm -m node -T %s --portal %s --login 2> %s 1> %s &"
//#define EVT_ISCSI_DISCONNECT_CMD "iscsiadm -m node -T %s --logout 2> %s 1> %s"
#define EVT_ISCSI_DISCONNECT_CMD "iscsiadm -m node --logoutall=all 2> %s 1> %s"
#define EVT_MQ_MSG_NUM_MAX 10
#define EVT_MQ_MSG_LEN_MAX 256

#define EVT_ACTION_SNAPSHOT "snapshot"
#define EVT_ACTION_LOG     "log"
#define EVT_ACTION_VIDEO   "video"

#define EVT_VIDEO_FILE_TYPE "mp4"

#define EVT_UNIX_SOCKET_SESSION "/tmp/unix_socket_event_event"
#define EVT_UNIX_SOCKET_SESSION_RECORD_CMD "/tmp/unix_socket_event_record"

//customed event from uITRON
#define CUSTOM_EVT_PREFIX	"CST,"
#define CUSTOM_EVT_MQ_MSG_NUM_MAX 10
#define CUSTOM_EVT_MQ_MSG_LEN_MAX 12
#define CUSTOM_NOTIFY_EVENT_QUEUE_NAME "/NvtCustomEventMq"

/////get event status (motion, digital input, network loss)
EVENT_ER nvtEvent_get_evt_status(EVENT_INFO *result);
EVENT_ER nvtEvent_register_event(int *sd, REGISTER_TYPE type);
EVENT_ER nvtEvent_unregister_event(int sd);
EVENT_ER nvtEvent_wait_evt_notify(EVENT_INFO *result, int time_out, int sd);//time_out: second
////motion configuration
EVENT_ER nvtEvent_get_motion_cfg(int id, MOTION_CFG *motion_cfg);//now id always =0
EVENT_ER nvtEvent_set_motion_cfg(int id, MOTION_CFG *motion_cfg);//now id always =0
EVENT_ER nvtEvent_get_motion_enable(char *enable);///enable: "on" or "off"
////digital input config
EVENT_ER nvtEvent_set_alarm_input_cfg(EVENT_ALARMINPUT_CFG alarm_cfg);
EVENT_ER nvtEvent_get_alarm_input_cfg(EVENT_ALARMINPUT_CFG *alarm_cfg);
///digital output
EVENT_ER nvtEvent_get_alarm_output_cfg(ALARM_OUTPUT_STATUS *cfg);  
EVENT_ER nvtEvent_set_alarm_output_cfg(ALARM_OUTPUT_STATUS cfg); 
EVENT_ER nvtEvent_get_alarm_output_status(ALARM_OUTPUT_STATUS *status);
EVENT_ER nvtEevnt_set_alarm_output_grounded(void);
EVENT_ER nvtEevnt_set_alarm_output_open(void);
///event configuration
EVENT_ER nvtEevnt_get_event_cfg(char *name, EVENT_CFG * event_cfg);
EVENT_ER nvtEevnt_get_event_cfg_list( EVENT_CFG  *event_cfg, int *event_num);
EVENT_ER nvtEvent_add_event_cfg(EVENT_CFG *event_cfg);
EVENT_ER nvtEvent_remove_event_cfg(char *name);
EVENT_ER nvtEvent_modify_event_cfg(EVENT_CFG *event_cfg);
//event ftp configuration
EVENT_ER nvtEvent_get_ftp_cfg_list(EVENT_FTP_CFG* ftp_cfg, int *ftp_num);
EVENT_ER nvtEvent_get_ftp_cfg(char *name, EVENT_FTP_CFG *ftp_cfg);
EVENT_ER nvtEvent_add_ftp_cfg(EVENT_FTP_CFG *ftp_cfg);
EVENT_ER nvtEvent_remove_ftp_cfg(char *name);
EVENT_ER nvtEvent_modify_ftp_cfg(EVENT_FTP_CFG *ftp_cfg);
//event smtp configuration
EVENT_ER nvtEvent_get_smtp_cfg_list(EVENT_SMTP_CFG* smtp_cfg, int *smtp_num);
EVENT_ER nvtEvent_get_smtp_cfg(char *name, EVENT_SMTP_CFG *smtp_cfg);
EVENT_ER nvtEvent_add_smtp_cfg(EVENT_SMTP_CFG *smtp_cfg);
EVENT_ER nvtEvent_remove_smtp_cfg(char *name);
EVENT_ER nvtEvent_modify_smtp_cfg(EVENT_SMTP_CFG *smtp_cfg);
//event sd configuration
EVENT_ER nvtEvent_get_sd_cfg_list(EVENT_SD_CFG* sd_cfg, int *sd_num);
EVENT_ER nvtEvent_get_sd_cfg(char *name, EVENT_SD_CFG *sd_cfg);
EVENT_ER nvtEvent_add_sd_cfg(EVENT_SD_CFG *sd_cfg);
EVENT_ER nvtEvent_remove_sd_cfg(char *name);
EVENT_ER nvtEvent_modify_sd_cfg(EVENT_SD_CFG *sd_cfg);
//event http configuration
EVENT_ER nvtEvent_get_http_cfg_list(EVENT_HTTP_CFG* http_cfg, int *http_num);
EVENT_ER nvtEvent_get_http_cfg(char *name, EVENT_HTTP_CFG *http_cfg);
EVENT_ER nvtEvent_add_http_cfg(EVENT_HTTP_CFG *http_cfg);
EVENT_ER nvtEvent_remove_http_cfg(char *name);
EVENT_ER nvtEvent_modify_http_cfg(EVENT_HTTP_CFG *http_cfg);
//event iscsi configuration
EVENT_ER nvtEvent_get_iscsi_cfg_list(EVENT_ISCSI_CFG* iscsi_cfg, int *iscsi_num);
EVENT_ER nvtEvent_get_iscsi_cfg(char *name, EVENT_ISCSI_CFG *iscsi_cfg);
EVENT_ER nvtEvent_add_iscsi_cfg(EVENT_ISCSI_CFG *iscsi_cfg);
EVENT_ER nvtEvent_remove_iscsi_cfg(char *name);
EVENT_ER nvtEvent_modify_iscsi_cfg(EVENT_ISCSI_CFG *iscsi_cfg);
EVENT_ER nvtEvent_connect_iscsi_target(EVENT_ISCSI_CFG *iscsi_cfg);
EVENT_ER nvtEvent_disconnect_iscsi_target(EVENT_ISCSI_CFG *iscsi_cfg);
//event action
EVENT_ER nvtEvent_snapshot_to_file(char *path, int ID,EVENT_CFG *event_cfg ,char *output_file_name);////now ID always = 0;
EVENT_ER nvtEvent_log_to_file(char *path, int ID,EVENT_CFG *event_cfg ,char *output_file_name);////now ID always = 0;
EVENT_ER nvtEvent_remove_file(char *file);
EVENT_ER nvtEvent_ftp_send_file(char *file_path, EVENT_CFG *event_cfg);
EVENT_ER nvtEvent_sd_send_file(char *file_path, EVENT_CFG *event_cfg);
EVENT_ER nvtEvent_http_send_file(char *file_path, EVENT_CFG *event_cfg);
EVENT_ER nvtEvent_iscsi_send_file(char *file_path, EVENT_CFG *event_cfg);
EVENT_ER nvtEvent_smtp_send_file(char *file_path, EVENT_CFG *event_cfg);
//save config to file
EVENT_ER nvtEvent_Pstore_to_file(void);
//for testing event server (ex: ftp server, iscsi server, sd server, smtp server ,http server)
EVENT_ER create_test_event(EVENT_CFG *test_event);
EVENT_ER nvtEvent_smtp_send_file_test(char *file_path, EVENT_SMTP_CFG *smtp_cfg);
EVENT_ER nvtEvent_iscsi_send_file_test(char *file_path, EVENT_ISCSI_CFG *iscsi_cfg);
EVENT_ER nvtEvent_http_send_file_test(char *file_path, EVENT_HTTP_CFG *http_cfg);
EVENT_ER nvtEvent_sd_send_file_test(char *file_path, EVENT_SD_CFG *sd_cfg);
EVENT_ER nvtEvent_ftp_send_file_test(char *file_path, EVENT_FTP_CFG *ftp_cfg);

////check SD card exist
EVENT_ER nvtEvent_check_SD_exist(int *bExist);


/////for onvif live555 rtsp metadata status
EVENT_ER nvtEvent_get_onvif_pull_msg(char *result ,unsigned int max_len, int b_init); ////result: output about event info , max_len: the result max buffer size   b_init: first send event msg (1: initial 0:not inital)
#ifdef __cplusplus
}
#endif
/* ----------------------------------------------------------------- */
#endif /* _NVTEVENT_H  */
