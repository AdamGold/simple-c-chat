#include "utilities.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int createAndBind(struct addrinfo *res, int isBind)
{
	struct addrinfo *p;
	int sockfd, yes = 1;
	for(p = res; p != NULL; p = p->ai_next) {
		if((sockfd = socket(p->ai_family,
				p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
			perror("setsockopt");
		}
		
		int (*fncToUse)(int, const struct sockaddr*, socklen_t);
		if (isBind)
			fncToUse = &bind;
		else
			fncToUse = &connect;
		if((*fncToUse)(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			if (!isBind)
				close(sockfd);
			perror("bind/connect failed");
			continue;
		}
		break;
	}

	freeaddrinfo(res); // free the linked list
	check(p == NULL, -1, "failed to find server and bind");

	return sockfd;
}
