/**
 * History:
 * ================================================================
 * 2017-08-21 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <sys/time.h>

#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>

#include "vpk.h"

#define IPC_SOCKET_PATH		"/tmp/local.ipc"

int server_test()
{
	int rc;
	int opt;
	struct sockaddr_storage ss;
	struct sockaddr_un *un;

	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		LOG_E("socket: %s", strerror(errno));
		return -1;
	}
	opt = 1;
	//rc = setsockopt (fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof (opt));
	opt = fcntl (fd, F_GETFL, 0);
	if (opt == -1)
		opt = 0;
	if (!(opt & O_NONBLOCK))
		rc = fcntl (fd, F_SETFL, opt | O_NONBLOCK);

	memset(&ss, 0, sizeof(ss));
	un = (struct sockaddr_un*) &ss;
	ss.ss_family = AF_UNIX;
	strncpy(un->sun_path, IPC_SOCKET_PATH, sizeof(un->sun_path));

	/*  Allow re-using the address. */
	opt = 1;
	rc = setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));
	LOG_D("rc = %d", rc);
	if (bind(fd, (struct sockaddr*)un, sizeof(struct sockaddr_un)) < 0) {
		LOG_E("bind: %s", strerror(errno));
		return -1;
	}

	if (listen(fd, 10) < 0) {
		LOG_E("listen: %s", strerror(errno));
		return -1;
	}

	int s, len;
	char recvbuf[2048] = {0};
	while (1)
	{
		if ((s = accept(fd, NULL, NULL)) < 0) {
			LOG_E("accept: %s", strerror(errno));
			sleep(1);
			continue;
		}

		len = send(s, "Welcome to server", strlen("Welcome to server"), 0);

		while ( (len = recv(s, recvbuf, 2048, 0)) > 0 ) {
			LOG_D("recv: %s", recvbuf);

			sprintf(recvbuf, "%s %s", recvbuf, " back from server");
			len = send(s, recvbuf, strlen(recvbuf), 0);
			memset(recvbuf, 0, 2048);
		}
	}

	close(s);
	close(fd);

	return 0;
}

int client_test()
{
	int rc;
	int opt;
	struct sockaddr_storage ss;
	struct sockaddr_un *un;

	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		LOG_E("socket: %s", strerror(errno));
		return -1;
	}
	opt = 1;
	//rc = setsockopt (fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof (opt));
	opt = fcntl (fd, F_GETFL, 0);
	if (opt == -1)
		opt = 0;
	if (!(opt & O_NONBLOCK))
		rc = fcntl (fd, F_SETFL, opt | O_NONBLOCK);

	memset(&ss, 0, sizeof(ss));
	un = (struct sockaddr_un*) &ss;
	ss.ss_family = AF_UNIX;
	strncpy(un->sun_path, IPC_SOCKET_PATH, sizeof(un->sun_path));

	rc = connect(fd, (struct sockaddr *)&ss, sizeof(struct sockaddr_un));
	if (rc < 0) {
		LOG_E("connect: %s", strerror(errno));
	}
	usleep(600000);

	int len = 0;
	char buffer[2048] = {0};
	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		len = recv(fd, buffer, 2048, 0);
		LOG_D("recv: %s", buffer);
		LOG_D("Enter message to send:");
		scanf("%s", buffer);
		len = send(fd, buffer, strlen(buffer), 0);
		LOG_D("send[%d]: %s", len, buffer);
		usleep(500000);
	}

	return 0;
}

int socket_main(int argc, char *argv[])
{
// 	int ret = 0;
// 
// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");

	//char* pathname = "./ipc";
	//int proj_id = 0x10;
	char* type = "time";
	//if (argc > 1)
	//{
	//	type = argv[1];
	//}

	int o;
	static const struct option long_options[] = {
		{ "help",			no_argument,			NULL, 'h' },
		{ "version",		no_argument,			NULL, 'V' },
		{ "sample",			required_argument,		NULL, 'd' },
		{ "type",			required_argument,		NULL, 't' },
		{ "keycode",		required_argument,		NULL, 'k' },
		{ "file",			required_argument,		NULL, 'f' },
		{ "url",			required_argument,		NULL, 'u' },
		{ "log",			optional_argument,		NULL, 'l' },
		{ NULL, 0, NULL, 0 }
	};

	optind = 1;
	//LOG_I("22 optind = %d, argc = %d", optind, argc);
	while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0) {
		//printf("opt = %c\n", o);  
		//printf("optarg = %s\n", optarg);  
		//printf("optind = %d\n", optind);  
		//printf("argv[optind - 1] = %s\n",  argv[optind - 1]);
		switch(o) {
			case 't':
				type = optarg;
				break;
			default:
				break;
		}
	}

	LOG_D("type = %s", type);

	double elapsed;
	struct timeval result, prev, next;	

	gettimeofday(&prev, 0);

	if (strcasecmp(type, "server") == 0) {
		server_test();
	} else if (strcasecmp(type, "client") == 0) {
		client_test();
	} else {
		client_test();
	}

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);		//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
