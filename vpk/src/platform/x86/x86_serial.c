/*
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "vpk_logging.h"
#include "x86_serial.h"

static int x86_serial_opt_set(vpk_session_t *session);
static int rs232_opt_ex(int fd, unsigned int baud_speed, unsigned int data_bits, 
						unsigned char parity_bit, unsigned int stop_bit);

// 
// ssize_t tread(int fd, void *buf, size_t nbytes, unsigned int timout)
// {
// 	int                         nfds;
// 	fd_set                   readfds;
// 	struct timeval  tv;
// 
// 	tv.tv_sec = timout;
// 	tv.tv_usec = 0;
// 	FD_ZERO(&readfds);
// 	FD_SET(fd, &readfds);
// 	nfds = select(fd+1, &readfds, NULL, NULL, &tv);
// 	if (nfds <= 0) {
// 		if (nfds == 0)
// 			errno = ETIME;
// 		return(-1);
// 	}
// 	return(read(fd, buf, nbytes));
// }
// 
// ssize_t treadn(int fd, void *buf, size_t nbytes, unsigned int timout)
// {
// 	size_t      nleft;
// 	ssize_t     nread;
// 
// 	nleft = nbytes;
// 	while (nleft > 0) {
// 		if ((nread = tread(fd, buf, nleft, timout)) < 0) {
// 			if (nleft == nbytes)
// 				return(-1); /* error, return -1 */
// 			else
// 				break;      /* error, return amount read so far */
// 		} else if (nread == 0) {
// 			break;          /* EOF */
// 		}
// 		nleft -= nread;
// 		buf += nread;
// 	}
// 	return(nbytes - nleft);      /* return >= 0 */
// }

/**
 * Set serial port parameters
 *
 * @param fd The file description.
 * @param baud_speed The serial baud speed, 2400, 4800, 9600...
 * @param data_bits  The data bit value: 5, 6, 7, 8
 * @param parity_bit The parity bit value, 'N': no parity, 'O': odd parity, 'E': even parity
 * @param stop_bit   The stop bit: 1, 2.
 *
 * @return 0 success, -1 failure
 */
static int rs232_opt_ex(int fd, unsigned int baud_speed, unsigned int data_bits, 
						unsigned char parity_bit, unsigned int stop_bit)
{

	struct termios newtio, oldtio;

	if(tcgetattr(fd, &oldtio) != 0)
	{
		return -1;
	}

	//bzero((char *)&newtio, sizeof(newtio));
	memset(&newtio, 0x0, sizeof(newtio));
	newtio.c_cflag |= (CLOCAL | CREAD);
	newtio.c_cflag &= ~CSIZE;

	//设置波特率
	switch( baud_speed )
	{
	case 2400:		//2400
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case 4800:		//4800
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:		//9600
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 19200:		//19200
		cfsetispeed(&newtio, B19200);
		cfsetospeed(&newtio, B19200);
		break;
	case 38400:		//38400
		cfsetispeed(&newtio, B38400);
		cfsetospeed(&newtio, B38400);
		break;
	case 57600:		//57600
		cfsetispeed(&newtio, B57600);
		cfsetospeed(&newtio, B57600);
		break;
	case 115200:	//115200
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	case 230400:	//230400
		cfsetispeed(&newtio, B230400);
		cfsetospeed(&newtio, B230400);
		break;
	case 460800:	//460800
		cfsetispeed(&newtio, B460800);
		cfsetospeed(&newtio, B460800);
		break;
	case 576000:	//576000
		cfsetispeed(&newtio, B576000);
		cfsetospeed(&newtio, B576000);
		break;
	case 921600:	//921600
		cfsetispeed(&newtio, B921600);
		cfsetospeed(&newtio, B921600);
		break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}

	//设置数据位
	switch( data_bits )
	{
	case 5:		//5
		newtio.c_cflag |= CS5;
		break;
	case 6:		//6
		newtio.c_cflag |= CS6;
		break;
	case 7:		//7
		newtio.c_cflag |= CS7;
		break;
	case 8:		//8
		newtio.c_cflag |= CS8;
		break;
	default:	//
		newtio.c_cflag |= CS8;
		break;
	}

	//设置校验模式
	switch( parity_bit )
	{
	case 'o':
	case 'O':	//奇校验
		newtio.c_cflag |= PARENB;	//
		newtio.c_cflag |= PARODD;	//
		newtio.c_iflag |= INPCK;		//
		break;
	case 'e':
	case 'E':	//偶校验
		newtio.c_cflag |= PARENB;	//
		newtio.c_cflag &= ~PARODD;	//
		newtio.c_iflag |= INPCK;		//
		break;
	case 'n':
	case 'N':	//无校验
		newtio.c_cflag &= ~PARENB;	//
		break;
	default:	//
		newtio.c_cflag &= ~PARENB;
		break;	
	}

	//设置停止位
	switch(stop_bit)
	{
	case 1:		//
		newtio.c_cflag &= ~CSTOPB;
		break;
	case 2:		//
		newtio.c_cflag |= CSTOPB;
		break;
	default:	//
		newtio.c_cflag &= ~CSTOPB;
		break;
	}

	//Timeouts are ignored in canonical input mode or when the NDELAY option is set on the file via open or fcntl.
	newtio.c_cc[VTIME] = 100;	//超时10秒
	newtio.c_cc[VMIN] = 1;
	//	newtio.c_cc[VQUIT] = 0;
	//	newtio.c_cc[VSTART] = 0;

	tcflush(fd, TCIFLUSH);	//清空输入
	//Raw Mode
	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Input */
	newtio.c_oflag &= ~OPOST;   /*Output*/
	//Similarly, to disable hardware flow control:
	//newtio.c_cflag &= ~CNEW_RTSCTS;
	newtio.c_cflag &= ~CRTSCTS;
	//To disable software flow control simply mask those bits:
	newtio.c_iflag &= ~(IXON | IXOFF | IXANY);

	if((tcsetattr(fd, TCSANOW, &newtio)) != 0)
	{
		return (-1);
	}

	return 0;
}

static int com_open(const char *tty)
{
	int fd;
	return_val_if_fail(tty != NULL, -1);

	fd = open(tty, O_RDWR | O_NOCTTY | O_NONBLOCK);
	return_val_if_fail(fd != -1, -1);

	if (fcntl(fd, F_SETFL, 0) < 0)	/** 重新回到阻塞模式（blocking） **/
	{
		if (fcntl(fd, F_SETFL, 0) < 0) {
			LOG_W("can\'t block port.\n");
			close(fd);
			return -1;
		}
		else {
			LOG_D("block port is OK finally.");
		}
	}

	LOG_I("serial \'%s\' open success with fd = %d!", tty, fd);
	// 	if(isatty(STDIN_FILENO) == 0)
	// 		printf("standard input is not a terminal device\n");
	// 	else
	// 		printf("isatty success!\n");
	return fd;
}

static int x86_serial_destruct(void *session)
{
	vpk_serial_t* thiz = (vpk_serial_t*)session;
	if (thiz)
	{
		if (thiz->fd >= 0)
		{
			LOG_I("serial \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
			close(thiz->fd);
			thiz->fd = -1;
		}
		thiz->s_ops = NULL;
		TIMA_FREE(thiz);
		thiz = NULL;
	}
	return 0;
}

static vpk_session_t* x86_serial_load(void *tty)
{
	vpk_serial_t* thiz = NULL;
	tty = tty;

	thiz = (vpk_serial_t*)TIMA_MALLOC(sizeof(vpk_serial_t));
	if (thiz)
	{
		thiz->destruct   = (vpk_destruct_func)x86_serial_destruct;
		thiz->baud_speed = 9600;
		thiz->data_bits  = 8;
		thiz->parity_bit = 'n';
		thiz->stop_bit   = 1;
		thiz->fd		 = -1;
	}

	return (vpk_session_t*)thiz;
}

static int x86_serial_open(vpk_session_t *session, void *tty)
{
	int ret = -1;
	vpk_serial_t* thiz = (vpk_serial_t*)session;
	return_val_if_fail(thiz != NULL, -1);

// 	thiz->fd = open(thiz->name, O_RDWR | O_NOCTTY | O_NONBLOCK);
// 	return_val_if_fail(fd != -1, -1);
// 	
// 	if (fcntl(thiz->fd, F_SETFL, 0) < 0)	/** 重新回到阻塞模式（blocking） **/
// 	{
// 		if (fcntl(thiz->fd, F_SETFL, 0) < 0) {
// 			LOG_W("can't block port.\n");
// 			close(thiz->fd);
// 			return -1;
// 		}
// 		else {
// 			LOG_D("block port is OK finally.");
// 		}
// 	}
	thiz->fd = com_open(thiz->name);
	if (thiz->fd >= 0)
	{
		ret = rs232_opt_ex(thiz->fd, thiz->baud_speed, thiz->data_bits, thiz->parity_bit, thiz->stop_bit);
		if (ret == -1)
		{
			close(thiz->fd);
			thiz->fd = -1;
			LOG_E("serial \'%s\' option set failed!", thiz->name);
		}
		return ret;
	}
	else
	{
		LOG_E("serial port \'%s\' communication open failed!", thiz->name);
		return thiz->fd;
	}
}

//
static int x86_serial_read(vpk_session_t *session, void *buf, size_t nbytes, int timout, void* visit)
{
	int ret = 0, delay_time, fd;
	int count = 0, n = 0, total = nbytes;
	unsigned char *ptr = (unsigned char *)buf;
	fd_set fd_read;
	struct timeval tv;
	vpk_serial_t* thiz = (vpk_serial_t *)session;
	return_val_if_fail(thiz != NULL && thiz->fd >= 0 && buf != NULL, -1);
	
	fd = thiz->fd;
	delay_time = timout > 0 ? 1 : 0;

	do 
	{
		if (count >= nbytes)
		{
			count = nbytes;
			break;
		}

		memset(&tv, 0, sizeof(struct timeval));
		tv.tv_sec  = delay_time;
		tv.tv_usec = 0;
		FD_ZERO(&fd_read);
		FD_SET(fd, &fd_read);
		ret = select(fd+1, &fd_read, NULL, NULL, &tv);
		if (ret < 0)
		{
			LOG_E("(ret < 0)at timout = %d, select error occurred!", timout);
			return -1;
		}
		else if (ret == 0)
		{
			LOG_D("(ret == 0)serial \'%s\' read select time out at = %d!", thiz->name, timout);
			if (visit)
			{
				ret = ((SerialVisitFunc)visit)(thiz, buf);
				LOG_D("find end str and return(ret = %d, timout = %d)", ret, timout);
				if (ret == 0)
					return count;
			}
			continue;
		}
		else if (FD_ISSET(fd, &fd_read))
		{
			n = read(fd, ptr, total);
			if (n > 0)
			{
				count += n;
				ptr   += n;
				total -= n;
			}

			if (visit)
			{
				ret = ((SerialVisitFunc)visit)(thiz, buf);
				LOG_D("find end str and return(ret = %d, timout = %d)", ret, timout);
				if (ret == 0)
				{
					return count;
				}
			}
		}
		//usleep(500000);
	}while(--timout > 0);

	return count;
}

static int x86_serial_write(vpk_session_t *session, void *buf, size_t nbytes, int timout)
{
	int count = 0, total = 0, n = 0;
	char *ptr = (char *)buf;
	vpk_serial_t* thiz = (vpk_serial_t *)session;
	return_val_if_fail(thiz != NULL && thiz->fd >= 0, -1);
	timout = timout;
	
	if (buf && nbytes > 0)
	{
		total = strlen(ptr);
		total = total < nbytes ? total : nbytes;

		while (count < total)
		{
			n = write(thiz->fd, ptr, total);
			if (n <= 0)
				break;

			count += n;
			total -= n;
			ptr += n;
		}
	}

	return count;
}

static int x86_serial_close(vpk_session_t *session)
{
	vpk_serial_t* thiz = (vpk_serial_t*)session;
	if (thiz && thiz->fd >= 0)
	{
		LOG_I("serial \'%s\' close with fd = %d!\n", thiz->name, thiz->fd);
		close(thiz->fd);
		thiz->fd = -1;
	}
	else
	{
		LOG_W("serial(fd) has been closed!");
	}

	return 0;
}

static int x86_serial_opt_set(vpk_session_t *session)
{
	int ret = -1;
	vpk_serial_t* thiz = (vpk_serial_t *)session;
	return_val_if_fail(thiz != NULL && thiz->fd >= 0, -1);

	ret = rs232_opt_ex(thiz->fd, thiz->baud_speed, thiz->data_bits, thiz->parity_bit, thiz->stop_bit);

	return ret;
}

/* The serial operations */
static const session_ops x86_serial_ops =
{
	x86_serial_load,
	x86_serial_open,
	x86_serial_read,
	x86_serial_write,
	x86_serial_close,
	x86_serial_opt_set,
};

const session_ops* get_x86_serial_ops(void)
{
	return (const session_ops*)&x86_serial_ops;
}
