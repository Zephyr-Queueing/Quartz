#include <iostream>
#include <string>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include <Server.h>

#define BUF_SIZE 1024
#define SERVER_HOST "localhost"

using namespace std;

void Server::operator()() {
    int sfd;                                    // server socket
    struct sockaddr_storage peer_addr;          // peer addr info
    socklen_t peer_addr_len;                    // peer addr length
    char peer_host[NI_MAXHOST],                 // peer host/service information
         peer_service[NI_MAXSERV]; 
    
    sfd = Bind();
    PrintInfo(sfd);
    
    string data;
    int req_batch_size;
    while (true) {
        data = Receive(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len, peer_host, peer_service);
        req_batch_size = Parse(data);
        SendBatch(req_batch_size,  (struct sockaddr *) &peer_addr);
    }
}

void Server::SendBatch(int req_batch_size, struct sockaddr *peer_addr) {
    cout << "Sending batch of size: " << req_batch_size << "..." << endl;


    // TODO
}

int Server::Parse(string data) {
    cout << "Parsing data: " << data << endl;
    // TODO
    return 15;
}

string Server::Receive(int sfd, struct sockaddr *peer_addr, socklen_t *peer_addr_len,
                      char *host, char *service) {
    ssize_t nread;
    char buf[BUF_SIZE];

    nread = recvfrom(sfd, buf, BUF_SIZE, MSG_WAITALL,
                     peer_addr, peer_addr_len);
    buf[nread] = '\0';
    if (nread == -1)
        return nullptr;

    getnameinfo(peer_addr, *peer_addr_len, host, NI_MAXHOST,
                service, NI_MAXSERV, NI_NUMERICSERV);

    return string(buf);
}

void Server::PrintInfo(int sfd) {
    struct sockaddr_in serv_addr;
    socklen_t serv_addr_len = sizeof(serv_addr);
    if (getsockname(sfd, (struct sockaddr*) &serv_addr, &serv_addr_len) == -1) {
        perror("getsockname");
        return;
    }
    cout << "Server listening on port: " << ntohs(serv_addr.sin_port) << endl;
}

int Server::Bind() {
    int sfd, s;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = 0;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(SERVER_HOST, service.c_str(), &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(sfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    return sfd;
}
