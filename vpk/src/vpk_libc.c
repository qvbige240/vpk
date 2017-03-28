/**
 * History:
 * ================================================================
 * 2017-03-24 qing.zou created
 *
 */
#include "vpk_libc.h"

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

static int mysystem(char *cmdstring, char *buf, int size);
static int SystemEx(const char *cmdstring, unsigned int timeout);

VPKAPI int vpk_system_ex(const char *cmd, unsigned int timout)
{
	return SystemEx(cmd, timout);
}

VPKAPI int vpk_system_exval(char* cmd, char* buf, int size)
{
	return mysystem(cmd, buf, size);
}


//system函数扩展，加入超时值(0表示永久等待)
//超时时返回-2，其他情况返回不变。
static int SystemEx(const char *cmdstring, unsigned int timeout)   /* with appropriate signal handling */
{
	pid_t               pid;
	int                 status;
	struct sigaction    ignore, saveintr, savequit;
	sigset_t            chldmask, savemask;

	//精度换成十分之一秒
	timeout *= 10;
	if (timeout == 0)
		timeout = 0xFFFFFFFF;

	if (cmdstring == NULL)
		return(1);      /* always a command processor with UNIX */

	ignore.sa_handler = SIG_IGN;    /* ignore SIGINT and SIGQUIT */
	sigemptyset(&ignore.sa_mask);
	ignore.sa_flags = 0;
	if (sigaction(SIGINT, &ignore, &saveintr) < 0)
		return(-1);
	if (sigaction(SIGQUIT, &ignore, &savequit) < 0)
		return(-1);
	sigemptyset(&chldmask);         /* now block SIGCHLD */
	sigaddset(&chldmask, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
		return(-1);

	if ((pid = fork()) < 0) {
		return -1;    /* probably out of processes */
	} else if (pid == 0) {          /* child */
		/* restore previous signal actions & reset signal mask */
		sigaction(SIGINT, &saveintr, NULL);
		sigaction(SIGQUIT, &savequit, NULL);
		sigprocmask(SIG_SETMASK, &savemask, NULL);

		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		_exit(127);     /* exec error */
	}

	/* parent */
	int ret = 0;
	while (timeout-- > 0 &&
		(ret = waitpid(pid, &status, WNOHANG)) == 0)
		usleep(100*1000);

	/* restore previous signal actions & reset signal mask */
	if (sigaction(SIGINT, &saveintr, NULL) < 0)
		return(-1);
	if (sigaction(SIGQUIT, &savequit, NULL) < 0)
		return(-1);
	if (sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
		return(-1);

	if (ret < 0)
		return -1;

	if (ret > 0)
		return status;

	kill(pid, SIGKILL);
	waitpid(pid, &status, 0);
	return -2;
}

/**
*@brief          命令执行结果函数
*@param[in]      
*@param[out]     
*@return         >0 :命令执行结果字节数 ; -1: Failed
*@remark         
*@author         lky
*@version        V2.1.1
*@date           2014.02.12  18:20:15
*@note     	
*/
static int mysystem(char *cmdstring, char *buf, int size)
{
	char cmd_string[200] = {0};
	char tmpfile[100] = {0};
	char tmp_buf[100] = {0};
	int fd;
	int tmp_fd;
	int nbytes;

	if((cmdstring == NULL) ||
		(strlen(cmdstring) > (sizeof(tmpfile) + 8)) ||
		((strlen(cmdstring) + strlen(tmpfile) + 5) > sizeof(cmd_string))){
			printf("cmd is too long or NULL!\n");
			return -1;
	}
	sscanf(cmdstring, "%[a-Z]", tmp_buf);
	sprintf(tmpfile, "/tmp/%s-XXXXXX", tmp_buf);

	tmp_fd = mkstemp(tmpfile);
	if(tmp_fd < 0){
		printf("mkstemp failed\n");
		return -1;
	}
	close(tmp_fd);

	sprintf(cmd_string, "%s > %s", cmdstring, tmpfile);

	//if(system(cmd_string) < 0){
	if(SystemEx(cmd_string, 20) < 0){
		printf("run \"%s\" failed!\n", cmd_string);
		nbytes = -1;
	}
	else{
		fd = open(tmpfile, O_RDONLY);
		if(fd < 0){
			printf("open %s failed!\n", tmpfile);
			nbytes = -1;
		}
		else{
			nbytes = read(fd, buf, size);
			close(fd);
		}
	}
	memset(cmd_string, 0, sizeof(cmd_string));
	sprintf(cmd_string, "rm -rf /tmp/%s-*", tmp_buf);
	//system(cmd_string);
	SystemEx(cmd_string, 20);

	return nbytes;
}
