#include "sys/types.h"
#include "sys/socket.h"
#include "netdb.h"
#include "netinet/in.h"
#include "pthread.h"
#include <arpa/inet.h>

#define MAX_USERS 5
#define MAX_LENGTH 255
#define WELCOME_MESSAGE "Hello there"
#define PORT_NUM "4444"

int createAndBind(struct addrinfo *res, int isBind);
void *get_in_addr(struct sockaddr *sa);
