#include <QThread>
#include <QLabel>
#include <QPixmap>
#include <QCoreApplication>
#include <iostream>
#include <chrono>
#include <thread>
#include "ThreadSafeQueue.h"

using namespace std::chrono;
class RenderingWorker : public QThread {
    Q_OBJECT

public:
    RenderingWorker( std::pair<int, int>* frame_size, QLabel* label, ThreadSafeQueue<QImage>& queue, int targetFPS)
        : frame_size(frame_size), label(label), frameQueue(queue), targetFPS(targetFPS) {}

void run() override {
    int frameCount = 0;
    QImage* frame = new QImage;
    QImage* scaledImage = new QImage;
    auto sleepDuration = std::chrono::milliseconds(1000 / targetFPS);
    const int waitTime = 4; 
    auto lastFrameTime = std::chrono::steady_clock::now();
    int targetWidth=0;
    int targetHeight=0;
    while (true) {
    if (!frameQueue.empty()) {
        lastFrameTime = std::chrono::steady_clock::now(); 

        *frame = frameQueue.pop();
        auto [targetWidth, targetHeight]=*frame_size;
        *scaledImage = frame->scaled(targetWidth,targetHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(QPixmap::fromImage(*scaledImage));
        QCoreApplication::processEvents();
        if (sleepDuration.count() > 0) {
            std::this_thread::sleep_for(sleepDuration);
        }
    }
    else {
            auto now = std::chrono::steady_clock::now();
            auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - lastFrameTime).count();
            if (elapsedSeconds >= waitTime) {
                    std::cout<<"the time is up"<<std::endl;
                    std::cout<<"the height and width are"<<targetWidth<<targetHeight<<std::endl;
                    emit finished();
                    break; 
                }
    }
    }
    delete frame;
    delete scaledImage;
}
signals:
    void finished();

private:
    QLabel* label;
    ThreadSafeQueue<QImage>& frameQueue;
    int targetFPS;  
    std::pair<int, int>* frame_size;
};
