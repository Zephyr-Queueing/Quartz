#ifndef SERVER_H
#define SERVER_H

class Server {
  public:
    Server(int port) : port{1} {}
    void operator()();

  private:
    int port;
};

#endif // SERVER_H
