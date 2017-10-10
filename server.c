#include "server.h"

static struct chatUser users[MAX_USERS];

int main(int argc, char *argv[])
{
	check(argc != 2, 2, "USAGE ./csr HOSTNAME");
	struct addrinfo hints, *res;
	int status, sockfd;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	hints.ai_socktype = SOCK_STREAM;

	for (int i = 0; i < MAX_USERS; i++) {
		users[i].available = 1;
	}
	int i = 0;
	// check argv[1]
	check(argv[1][strlen(argv[1])] != '\0', 2, "invalid host");
	status = getaddrinfo(argv[1], PORT_NUM, &hints, &res);
	check(status != 0, 2, "getaddrinfo: %s\n", gai_strerror(status));

	sockfd = createAndBind(res, 1);

	check(listen(sockfd, MAX_USERS) == -1, 2, "listen failed");

	printf("Server: Waiting for connections...\n");
	addr_size = sizeof their_addr;
	while (i <= MAX_USERS) {
		if (users[i].available != 1) {
			// but maybe there are other available spaces
			if (i == MAX_USERS)
				i = 0;
			continue;
		}
		if ((users[i].sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) == -1) {
			perror("unable to accept connection.");
			continue;
		}
		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
		       	users[i].ip, sizeof(users[i].ip));

		printf("server: got connection from %s\n", users[i].ip);
		users[i].available = 0;
		if (send(users[i].sockfd, WELCOME_MESSAGE, sizeof(WELCOME_MESSAGE), 0) == -1) {
			perror("welcome message failed.");
			continue;
		}

		if (pthread_create(&(users[i].thread), NULL, receiveMsgs, (void *) i)) {
			perror("failed to open thread");
			continue;
		}
		i++;
	}
	i = 0;

	for (; i < MAX_USERS; i++) {
		shutdown(users[i].sockfd, SHUT_RDWR);
		close(users[i].sockfd);
	}
	close(sockfd);
	pthread_exit(NULL);
	return 0;
}
int receiveProccess(struct chatUser *user, char *msg)
{
	int nbytes;
	memset(msg, 0, MAX_LENGTH);
	if ((nbytes = recv(user->sockfd, msg, MAX_LENGTH - 1, 0)) <= 0)
	{
		user->available = 1;
		return 0;
	} else {
		msg[nbytes] = '\0';
		char *pos = strchr(msg, '\n');
		*pos = '\0';
		if (strlen(msg) <= 0)
			return 0;
		return 1;
	}
}

void *receiveMsgs(void *userId)
{
	struct chatUser *user = &users[(int) userId];
	int flag = 1;
	char msg[MAX_LENGTH];
	check(!receiveProccess(user, user->name), 0, "Bye Bye");

	for (;;) {
		if (flag > 1) { // more than one empty msg? bye bye
			fprintf(stderr, "Bye Bye %s", user->name);
			user->available = 1;
			break;
		}
		for (int x = 0; x < MAX_USERS; x++) {
			if (x == (int) userId || users[x].available == 1)
				continue;
			if (send(users[x].sockfd, msg, MAX_LENGTH - 1, 0) == -1) {
				perror("message failed.");
			}
		}
		if (!receiveProccess(user, msg)) {
			++flag;
		} else {
			printf("%s: %s\n", user->name, msg);
		}
	}
	return NULL;
}
