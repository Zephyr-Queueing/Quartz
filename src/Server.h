#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include <WeightedPriorityQueue.h>

using namespace std;

class Server {
  public:
    Server(WeightedPriorityQueue &w) : wpq(w) {}
    void operator()();

  private:
    int Bind();
    pair<int, int> Parse(string data);
    void PrintInfo(int sfd);
    void SendBatch(int sfd, int req_batch_size, struct sockaddr *peer_addr, socklen_t peer_addr_len);
    string Receive(int sfd, struct sockaddr *peer_addr, socklen_t *peer_addr_len, char *host, char *service);
    WeightedPriorityQueue &wpq;
};

#endif // SERVER_H
