/**
 * History:
 * ================================================================
 * 2019-12-24 qing.zou created
 *
 */

#include <unistd.h>
#include <sys/msg.h>

#include "vpk_object.h"
#include "vpk_logging.h"
#include "vpk_ipc.h"
//#include "vpk_mqueue.h"

// #define VPK_MQ_MSG_NUM_MAX	    10
// #define VPK_MQ_MSG_LEN_MAX	    512

#define MQUEUE_BUFFER_SIZE      VPK_MQ_MSG_LEN_MAX

struct vpk_mqueue
{
    extends_object();
    int     fd;
    char    name[MAX_PATH_SIZE];
    char    recv_buff[VPK_MQ_MSG_LEN_MAX];
};

static int msg_queue_init(const char *fname, int id);
static int msg_queue_is_empty(int qid);
static int msg_queue_post(int qid, void *msg_value);
static int msg_queue_recv(int qid, void *msg_value);
static int msg_queue_rmid(int qid);

static int msg_queue_destruct(void *queue)
{
    vpk_mqueue_t *thiz = queue;
    if (thiz)
    {
        if (thiz->fd >= 0)
        {
            int ret = msg_queue_rmid(thiz->fd);
            if (ret >= 0)
                LOG_I("msg queue \'%s\' close with id %d!\n", thiz->name, thiz->fd);
            thiz->fd = -1;
        }
        free(thiz);
        thiz = NULL;
    }
    return 0;
}

vpk_mqueue_t *vpk_mqueue_open(const char *name, int id)
{
    vpk_mqueue_t *thiz = calloc(1, sizeof(vpk_mqueue_t));
    if (thiz)
    {
        thiz->destruct  = msg_queue_destruct;
        thiz->fd        = msg_queue_init(name, id);
        strncpy(thiz->name, name, sizeof(thiz->name));
        if (thiz->fd < 0)
        {
            LOG_E("msg queue open failed, at %s [%d]", name, id);
            free(thiz);
            return NULL;
        }
        LOG_D("msg queue(name = \'%s\') opened, id %d", thiz->name, thiz->fd);
    }

    return thiz;
}

int vpk_mqueue_empty(vpk_mqueue_t *queue)
{
    if (!queue)
    {
        LOG_E("queue null pointer");
        return -1;
    }
    return msg_queue_is_empty(queue->fd);
}

int vpk_mqueue_recv(vpk_mqueue_t *thiz, vpk_msg_t *m)
{
    return_val_if_fail(thiz && m, -1);

    memset(thiz->recv_buff, 0x00, sizeof(thiz->recv_buff));
    int ret = msg_queue_recv(thiz->fd, thiz->recv_buff);
    if (ret < 0)
    {
        LOG_E("msg queue recv error, name = \'%s\' id %d", thiz->name, thiz->fd);
        return -1;
    }

    m->data = thiz->recv_buff;

    return ret;
}

int vpk_mqueue_post(vpk_mqueue_t *thiz, vpk_msg_t *m)
{
    return_val_if_fail(thiz && m, -1);

    int ret = msg_queue_post(thiz->fd, m->data);
    if (ret < 0)
    {
        LOG_E("msg queue post error, name = \'%s\' id %d", thiz->name, thiz->fd);
        return -1;
    }
    return 0;
}

int vpk_mqueue_close(vpk_mqueue_t *thiz)
{
    int ret = -1;
    if (thiz && thiz->destruct)
    {
        ret = thiz->destruct(thiz);
    }
    return ret;
}


/* xsi ipc, message queue */
typedef struct queue_message
{
    long        msg_type;
    char        msg_value[MQUEUE_BUFFER_SIZE];
} queue_message_t;

static int msg_queue_init(const char *fname, int id)
{
    int qid;
    key_t key;

    //if ((key = ftok(".", 'a')) == -1)
    if ((key = ftok(fname, id)) == -1)
    {
        LOG_E("ftok error!\n");
        return -1;
    }

    if ((qid = msgget(key, IPC_CREAT | 0666)) == -1)
    {
        LOG_E("msgget error!\n");
        return -1;
    }

    LOG_I("queue %d opened\n", qid);

    return qid;
}

static int msg_queue_is_empty(int qid)
{
    int ret = 0;
    struct msqid_ds buf;

    if ((msgctl(qid, IPC_STAT, &buf)) != 0)
    {
        LOG_E("msgctl error");
    }

    if (buf.msg_qnum != 0)
        ret = 0;
    else
        ret = 1;

    return ret;
}

static int msg_queue_post(int qid, void *msg_value)
{
    queue_message_t message = {0};

    message.msg_type = (long)getpid();
    memcpy(message.msg_value, msg_value, sizeof(message.msg_value));

    if (msgsnd(qid, &message, MQUEUE_BUFFER_SIZE, IPC_NOWAIT) < 0)
    {
        LOG_E("msgsnd ERROR!\n");
        return -1;
    }

    return 0;
}

static int msg_queue_recv(int qid, void *msg_value)
{
    queue_message_t message = {0};

    if (msgrcv(qid, (void *)&message, MQUEUE_BUFFER_SIZE, 0, 0) < 0)
    {
        LOG_E("msgrcv ERROR!\n");
        return -1;
    }
    memcpy(msg_value, message.msg_value, MQUEUE_BUFFER_SIZE);

    return 0;
}

static int msg_queue_rmid(int qid)
{
    int ret = msgctl(qid, IPC_RMID, NULL);
    if (ret < 0)
        LOG_E("msg queue rm failed, at [%d]", qid);
    return ret;
}
