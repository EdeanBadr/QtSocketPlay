#include <QThread>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class AcceptWorker : public QObject {
    Q_OBJECT

public:
    AcceptWorker(int server_fd) : server_fd(server_fd) {}

signals:
    void newConnection(int socket);

public slots:
    void startAccepting() {
        while (!stopAccepting) {
            struct sockaddr_in address;
            int addrlen = sizeof(address);
            int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

            if (new_socket < 0) {
                std::cerr << "Accept failed" << std::endl;
                continue;
            }

            emit newConnection(new_socket);
        }
    }

    void stop() {
        stopAccepting = true;
    }

private:
    int server_fd;
    std::atomic<bool> stopAccepting{false};
};
