#define DEBUG 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "insock.h"
#include "dbg.h"

struct chatUser {
	char name[MAX_LENGTH];
	int sockfd;
	pthread_t thread;
	int available;
	char ip[INET6_ADDRSTRLEN];
};

int receiveProccess(struct chatUser *user, char *msg);
void *receiveMsgs(void *userId);

