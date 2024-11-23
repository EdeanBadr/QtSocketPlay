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
#include "AcceptWorker.h"

class ConnectionThread : public QObject {
    Q_OBJECT

public:
    ConnectionThread(QWidget* parentWindow) :
        window(parentWindow),
        layout(static_cast<QGridLayout*>(parentWindow->layout())),
        frame_size(new std::pair<int, int>),
        screen(QGuiApplication::primaryScreen()),
        screenGeometry(screen->geometry()),
        screenWidth(screenGeometry.width()),
        screenHeight(screenGeometry.height()) {}

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

        if (acceptThread) {
            acceptWorker->stop();
            acceptThread->quit();
            acceptThread->wait();
            delete acceptWorker;
            delete acceptThread;
        }
    }

    void startAcceptingConnections(int server_fd) {
        acceptWorker = new AcceptWorker(server_fd);
        acceptThread = new QThread();

        acceptWorker->moveToThread(acceptThread);

        connect(acceptThread, &QThread::started, acceptWorker, &AcceptWorker::startAccepting);
        connect(acceptWorker, &AcceptWorker::newConnection, this, &ConnectionThread::setupClientConnection);
        connect(this, &ConnectionThread::stopAcceptingConnections, acceptWorker, &AcceptWorker::stop);

        acceptThread->start();
    }

    void stopConnections() {
        emit stopAcceptingConnections();

        if (acceptThread) {
            acceptThread->quit();
            acceptThread->wait();
        }
    }

signals:
    void stopAcceptingConnections();

private slots:
    void setupClientConnection(int new_socket) {
        const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);
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

        QLabel* label = new QLabel();
        labels.push_back(label);
        updateLayout();
        ThreadSafeQueue<QImage>* frameQueue = new ThreadSafeQueue<QImage>();
        int targetFPS = 20;

        QThread* processingThread = new QThread();
        ReceptionWorker* receptionWorker = new ReceptionWorker(new_socket);
        DecodingWorker* decodingWorker = new DecodingWorker(codec_context, *frameQueue);
        RenderingWorker* renderingWorker = new RenderingWorker(frame_size, label, *frameQueue, targetFPS);

        receptionWorker->moveToThread(processingThread);
        decodingWorker->moveToThread(processingThread);

        QObject::connect(processingThread, &QThread::started, receptionWorker, &ReceptionWorker::getPacket);
        QObject::connect(receptionWorker, &ReceptionWorker::packetReady, decodingWorker, &DecodingWorker::getFrame);

        QObject::connect(renderingWorker, &RenderingWorker::finished, [this, renderingWorker, processingThread, label, frameQueue, codec_context]() {
            processingThread->quit();
            processingThread->wait();
            
            layout->removeWidget(label);
            labels.erase(std::remove(labels.begin(), labels.end(), label), labels.end());

            delete label;
            updateLayout();
            workerThreads.erase(std::remove(workerThreads.begin(), workerThreads.end(), processingThread), workerThreads.end());
            frameQueues.erase(std::remove(frameQueues.begin(), frameQueues.end(), frameQueue), frameQueues.end());
            codecContexts.erase(std::remove(codecContexts.begin(), codecContexts.end(), codec_context), codecContexts.end());
            delete frameQueue;
            processingThread->deleteLater();
            std::cout << "Thread has been properly cleaned up" << std::endl;
        });

        processingThread->start();
        renderingWorker->start();

        workerThreads.push_back(processingThread);
        frameQueues.push_back(frameQueue);
        codecContexts.push_back(codec_context);
        std::cout<<" frame queues number is"<<frameQueues.size()<<std::endl;
        std::cout<<" codec contexts number is"<<codecContexts.size()<<std::endl;
        std::cout<<" workerThreads  number is"<<workerThreads.size()<<std::endl;
        


    }

private:
    QWidget* window;
    QGridLayout* layout;
    QThread* acceptThread = nullptr;
    AcceptWorker* acceptWorker = nullptr;
    std::vector<QLabel*> labels;
    std::vector<QThread*> workerThreads;
    std::vector<ThreadSafeQueue<QImage>*> frameQueues;
    std::vector<AVCodecContext*> codecContexts;

    std::pair<int, int>* frame_size;
    QScreen* screen;
    QRect screenGeometry;
    int screenWidth;
    int screenHeight;
    
    std::pair<int, int> calculateSlotSize(int numLabels, int windowWidth, int windowHeight) {
        if (numLabels <= 0) {
            return {0, 0};
        }
        int rows, cols;
        if (numLabels == 1) {
            rows = 1;
            cols = 1;
        } else if (numLabels == 2) {
            rows = 1; 
            cols = 2;
        } else {
            rows = std::ceil(std::sqrt(numLabels));
            cols = std::ceil(double(numLabels) / rows);
        }
        int slotWidth = windowWidth / cols;
        int slotHeight = windowHeight / rows;
        std::cout<<"the width is:"<< slotWidth<<std::endl;
         std::cout<<"the height is:"<< slotHeight<<std::endl;
        return {slotWidth, slotHeight};
    }
   void updateLayout() {
    while (QLayoutItem* item = layout->takeAt(0)) {
        delete item;
    }

    *frame_size = calculateSlotSize(labels.size(), screenWidth, screenHeight);

    int numLabels = labels.size();
    int rows, cols;

    if (numLabels == 1) {
        rows = 1;
        cols = 1;
    } else if (numLabels == 2) {
        rows = 1; 
        cols = 2;
    } else {
        rows = std::ceil(std::sqrt(numLabels));
        cols = std::ceil(double(numLabels) / rows);
    }

    for (int i = 0; i < numLabels; ++i) {
        int row = i / cols;
        int col = i % cols;
        layout->addWidget(labels[i], row, col);
    }
}

};