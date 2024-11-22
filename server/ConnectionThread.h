#include <QThread>
#include <QLabel>
#include <iostream>
#include <vector>
#include "ThreadSafeQueue.h"
#include <QGridLayout>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
extern "C" {
#include <libavcodec/avcodec.h>
}
#include <QCoreApplication>
#include "ReceptionWorker.h"
#include "DecodingWorker.h"
#include "RenderingWorker.h"
#include "ThreadSafeQueue.h"
#include <QScreen>
#include <QApplication>


class ConnectionThread : public QObject {
    Q_OBJECT

public:
    ConnectionThread(QWidget* parentWindow) : 
        window(parentWindow), 
          layout(static_cast<QGridLayout*>(parentWindow->layout())) {
    }
     ~ConnectionThread() {
        stopConnections();

        for (auto label : labels) {
            delete label;
        }

        for (auto thread : workerThreads) {
            if (thread->isRunning()) {
                thread->quit();
                thread->wait();
            }
            delete thread;
        }

        for (auto queue : frameQueues) {
            delete queue;
        }

        for (auto codecContext : codecContexts) {
            avcodec_free_context(&codecContext);
        }

        delete frame_size;
    }

    void startAcceptingConnections(int server_fd) {
        acceptThread = std::thread([this, server_fd]() {
            while (!stopAccepting) {
                struct sockaddr_in address;
                int addrlen = sizeof(address);
                int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                
                if (new_socket < 0) {
                    std::cerr << "Accept failed" << std::endl;
                    continue;
                }

                QMetaObject::invokeMethod(this, "setupClientConnection", 
                    Qt::QueuedConnection, 
                    Q_ARG(int, new_socket));
            }
        });
    }

    void stopConnections() {
        stopAccepting = true;
        if (acceptThread.joinable()) {
            acceptThread.join();
        }
    }
    std::pair<int, int> calculateSlotSize(int numLabels, int windowWidth, int windowHeight) {
    if (numLabels <= 0) {
        return {0, 0}; 
    }
    int rows = std::ceil(std::sqrt(numLabels));
    int cols = std::ceil(double(numLabels) / rows);
    int slotWidth = windowWidth / cols;
    int slotHeight = windowHeight / rows;
    return {slotWidth, slotHeight};
}

signals:
    void clientConnectionReady(int socket);

private slots:
    void setupClientConnection(int new_socket) {
        const AVCodec* codec= new AVCodec;
        codec = avcodec_find_decoder(AV_CODEC_ID_H264);
        if (!codec) {
            std::cerr << "Codec not found!" << std::endl;
            return;
        }

       AVCodecContext* codec_context = avcodec_alloc_context3(codec);       
       if (!codec_context) {
            std::cerr << "Could not allocate codec context!" << std::endl;
            return;
        }

        if (avcodec_open2(codec_context, codec, nullptr) < 0) {
            std::cerr << "Could not open codec!" << std::endl;
            avcodec_free_context(&codec_context);
            return;
        }

        QLabel *label = new QLabel();
        labels.push_back(label);
        *frame_size=calculateSlotSize(labels.size(), screenWidth, screenHeight);
        
        layout->addWidget(label, labels.size() / 3, labels.size() % 3);
        ThreadSafeQueue<QImage>* frameQueue = new ThreadSafeQueue<QImage>();
        int targetFPS = 20;

        QThread* processingThread = new QThread();
        ReceptionWorker* receptionWorker = new ReceptionWorker(new_socket);
        DecodingWorker* decodingWorker = new DecodingWorker(codec_context, *frameQueue);
        RenderingWorker* renderingWorker = new RenderingWorker( frame_size, label, *frameQueue, targetFPS);

        receptionWorker->moveToThread(processingThread);
        decodingWorker->moveToThread(processingThread);

        QObject::connect(processingThread, &QThread::started, receptionWorker, &ReceptionWorker::getPacket);
        QObject::connect(receptionWorker, &ReceptionWorker::packetReady, decodingWorker, &DecodingWorker::getFrame);
        
        QObject::connect(renderingWorker, &RenderingWorker::finished, [this, renderingWorker, processingThread, label, frameQueue, codec_context]() {
                processingThread->quit();
                processingThread->wait();

                layout->removeWidget(label);
                workerThreads.erase(std::remove(workerThreads.begin(), workerThreads.end(), processingThread), workerThreads.end());
                frameQueues.erase(std::remove(frameQueues.begin(), frameQueues.end(), frameQueue), frameQueues.end());
                codecContexts.erase(std::remove(codecContexts.begin(), codecContexts.end(), codec_context), codecContexts.end());
                labels.erase(std::remove(labels.begin(), labels.end(), label), labels.end());
                delete label;
                delete frameQueue;
                processingThread->deleteLater();  
                renderingWorker->deleteLater(); 

                std::cout << "Thread has been properly cleaned up" << std::endl;
            });

        processingThread->start();
        renderingWorker->start();

        workerThreads.push_back(processingThread);
        frameQueues.push_back(frameQueue);
        codecContexts.push_back(codec_context);

    }

private:
    QWidget* window;
    QGridLayout* layout;
    std::thread acceptThread;
    std::atomic<bool> stopAccepting{false};

    std::vector<QLabel*> labels;
    std::vector<QThread*> workerThreads;
    std::vector<ThreadSafeQueue<QImage>*> frameQueues;
    std::vector<AVCodecContext*> codecContexts;
    int clientIndex = 0;
    std::pair<int, int>* frame_size=new std::pair<int, int>;
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
};
