#include "client.h"

int main(int argc, char *argv[])
{	
	check(argc != 2, 2, "USAGE ./clr HOSTNAME");
	struct addrinfo hints, *res;
	int status, sockfd;
	pthread_t thread;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	hints.ai_socktype = SOCK_STREAM;

	// check argv[1]
	check(argv[1][strlen(argv[1])] != '\0', 2, "invalid host");
	status = getaddrinfo(argv[1], PORT_NUM, &hints, &res);
	check(status != 0, 2, "getaddrinfo: %s\n", gai_strerror(status));
	
	sockfd = createAndBind(res, 0);

	char welcomeMsg[MAX_LENGTH], msg[MAX_LENGTH];
	int bytesRec;
	check((bytesRec = recv(sockfd, welcomeMsg, MAX_LENGTH-1, 0)) == -1, 2, "failed to receive message");
	welcomeMsg[bytesRec] = '\0';
	debug("message received: %s", welcomeMsg);
	if (strcmp(WELCOME_MESSAGE, welcomeMsg) == 0) {
		printf("Please enter a nickname first\n");
		if (pthread_create(&thread, NULL, receiveMsgs, (void *) sockfd)) {
			perror("failed to open thread");
		}
		while (fgets(msg, MAX_LENGTH, stdin) != NULL) {
			debug("sent %s", msg);
			if (send(sockfd, msg, MAX_LENGTH - 1, 0) == -1) {
				perror("message failed");
				break;
			}
		}
	}
	pthread_exit(NULL);
	return 0;
}

void *receiveMsgs(void *sockfd)
{
	char msg[MAX_LENGTH];
	memset(msg, 0, MAX_LENGTH);
	int nbytes;
	for (;;) {
		if ((nbytes = recv((int) sockfd, msg, MAX_LENGTH - 1, 0)) <= 0) {
			if (nbytes != 0)
				perror("error receiving.");
		} else {
			if (strlen(msg) > 0)
				printf("USER: %s\n", msg);
			fflush(stdout); // so our buffer will show
			continue;
		}
		break;
	}
	return NULL;
}
