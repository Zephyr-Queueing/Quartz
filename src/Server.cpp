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
#define SERVER_PORT 51711
#define BUF_SIZE 1024
#define DELIM "*"

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
    pair<int, int> request;
    while (true) {
        data = Receive(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len, peer_host, peer_service);
        request = Parse(data);
        if (request.second) {
            cerr << "Batch failed - recovered with " << request.first << " messages" << endl;
            FlushLastBatch();
        }
        SendBatch(sfd, request.first, (struct sockaddr *) &peer_addr, peer_addr_len);
    }
}

void Server::FlushLastBatch() {
    for (Message const &msg : lastBatch) {
        wpq.enqueueFront(msg);
    }
    lastBatch = list<Message>();
}

int Server::Bind() {
    int sfd, optval = 1;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr;
    
    if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    }

    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0) {
        perror("setsockopt failed"); 
        exit(EXIT_FAILURE); 
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERVER_PORT);
        
    if (bind(sfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }

    return sfd;
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

    return string(buf);
}

std::pair<int, bool> Server::Parse(string data) {
    // cout << "Received... " << data << endl;
    int delimIndex = data.find(DELIM[1]);
    string batchSizeData = data.substr(0, delimIndex);
    string hasProcessedBatchData = data.substr(delimIndex + 1);
    int batchSize;
    bool lastBatchStatus;
    try {
        batchSize = stoi(batchSizeData);
        lastBatchStatus = bool(hasProcessedBatchData == "1");
    } catch (...) {
        cout << "Ignoring invalid data." << endl;
    }
    return pair<int, bool>(batchSize, lastBatchStatus);
}

void Server::SendBatch(int sfd, int req_batch_size, struct sockaddr *peer_addr, socklen_t peer_addr_len) {
    // cout << "Sending batch of size: " << req_batch_size << "..." << endl;
    list<Message> batch = wpq.dequeueBatch(req_batch_size);
    lastBatch = batch;
    string buf;
    for (Message msg : batch) {
        buf.append(msg.serialize());
    }
    buf.append(DELIM);
    // cout << buf << endl;
    int n = sendto(sfd, buf.c_str(), buf.length(), 0, peer_addr, peer_addr_len);
    if (n < 0) {
        perror("Error on send");
        cerr << "ERROR in sendto" << endl;
    }
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
