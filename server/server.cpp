#include <iostream>
#include <cstring>
#include <thread>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <QApplication>
#include <QLabel>
#include <QGridLayout>
#include <QWidget>
#include <QThread>
#include <QTimer>
#include <vector>
#include <mutex>
#include <QScreen>

#include "ConnectionThread.h"


#define PORT 8080


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    qRegisterMetaType<AVPacket>("AVPacket");

    // Socket setup
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;
    QWidget window;
    QGridLayout* layout = new QGridLayout(&window);
    window.setLayout(layout);
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();

        window.setGeometry(screenGeometry);  
        window.setFixedSize(screenWidth, screenHeight); 
        std::cout<<"the screenWidth is :"<< screenWidth<<std::endl;
        std::cout<<"the screenheight is :"<< screenHeight<<std::endl;
    }
    else{
    window.setFixedSize(1000, 1000);
    }
    window.show();

    ConnectionThread clientManager(&window);
    clientManager.startAcceptingConnections(server_fd);

    return app.exec();
}

