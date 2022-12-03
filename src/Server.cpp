#include <iostream>
#include <string>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <Server.h>
#include <Message.h>

#define SERVER_HOST "127.0.0.1"
#define SERVER_SERVICE "51711"
#define BUF_SIZE 1024
#define BATCH_DELIM "*"

using namespace std;

void Server::operator()() {
    int sfd;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);                    
    char peer_host[NI_MAXHOST],                
         peer_service[NI_MAXSERV]; 
    
    sfd = Bind();
    // PrintInfo(sfd);
    
    string data;
    int req_batch_size;
    while (true) {
        data = Receive(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len, peer_host, peer_service);
        req_batch_size = Parse(data);
        SendBatch(sfd, req_batch_size, (struct sockaddr *) &peer_addr, peer_addr_len);
    }
}

void Server::SendBatch(int sfd, int req_batch_size, struct sockaddr *peer_addr, socklen_t peer_addr_len) {
    // cout << "Sending batch of size: " << req_batch_size << "..." << endl;
    list<Message> batch = wpq.dequeueBatch(req_batch_size);
    string buf;
    for (Message msg : batch) {
        buf.append(msg.serialize());
    }
    buf.append(BATCH_DELIM);
    // cout << buf << endl;
    int n = sendto(sfd, buf.c_str(), buf.length(), 0, peer_addr, peer_addr_len);
    if (n < 0) {
        perror("yee");
        cerr << "ERROR in sendto" << endl;
    }
}

int Server::Parse(string data) {
    // cout << "Received... " << data << endl;
    int batchSize;
    try {
        batchSize = stoi(data);
    } catch (...) {
        cout << "Ignoring invalid data." << endl;
    }
    return batchSize;
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

// void Server::PrintInfo(int sfd) {
//     struct sockaddr_in6 serv_addr;
//     socklen_t serv_addr_len = sizeof(serv_addr);
//     if (getsockname(sfd, (struct sockaddr*) &serv_addr, &serv_addr_len) == -1) {
//         perror("getsockname");
//         return;
//     }
//     cout << "Server listening on port: " << ntohs(serv_addr.sin6_port) << endl;
// }

int Server::Bind() {
    int sfd, s, optval = 1;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(SERVER_HOST, SERVER_SERVICE, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            cout << "binding" << endl;
            char s[INET_ADDRSTRLEN] = "\0";
            inet_ntop(AF_INET, &(rp->ai_addr), s, INET_ADDRSTRLEN);
            printf("IP address: %s\n", s);
            break;
        }

        close(sfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    return sfd;
}
