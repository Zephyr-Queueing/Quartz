#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include <Server.h>

#define BUF_SIZE 10

using namespace std;

void Server::operator()() {
    cout << "Server started..." << endl;
    
    int sfd, s;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct sockaddr_in serv_addr;
    struct sockaddr_storage peer_addr;
    socklen_t serv_addr_len;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];
    char port[] = "45940";

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = 0;         /* Allow IPv4 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(NULL, "9191", &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            cout << rp->ai_addrlen << endl;
            break;
        }
        close(sfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    serv_addr_len = sizeof(serv_addr);
    if (getsockname(sfd, (struct sockaddr*) &serv_addr, &serv_addr_len) == -1) {
        perror("getsockname");
        return;
    }

    cout << "Listening on " << serv_addr.sin_port  << ", " << serv_addr.sin_addr.s_addr << endl;

    peer_addr_len = sizeof(struct sockaddr_storage);
    for (;;) {
        nread = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
        cout << "here " << nread << endl;
        if (nread == -1)
            continue;

        char host[NI_MAXHOST], service[NI_MAXSERV];

        s = getnameinfo((struct sockaddr *) &peer_addr,
                        peer_addr_len, host, NI_MAXHOST,
                        service, NI_MAXSERV, NI_NUMERICSERV);
        if (s == 0)
            printf("Received %ld bytes from %s:%s\n", (long) nread, host, service);
        else
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

        if (sendto(sfd, buf, nread, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != nread)
            fprintf(stderr, "Error sending response\n");
    }
}