#include "BlockingQueue.h"

namespace avt {

    BlockingQueue::BlockingQueue(int maxSize) : MAX_SIZE(maxSize)
    {
    }

    BlockingQueue::~BlockingQueue()
    {
    }
    
    void BlockingQueue::send(std::vector<Cvec3f>& points) {
        std::unique_lock<std::mutex> locker(mu);
        cond.wait(locker, [this](){return !isFull();});

        std::vector<Cvec3f> copy(points);
        buffer.push(copy);

        locker.unlock();
        cond.notify_all();
    }

    const std::vector<Cvec3f>& BlockingQueue::consume() {
        std::unique_lock<std::mutex> locker(mu);
        cond.wait(locker, [this](){return !isEmpty();});
        const std::vector<Cvec3f>& front = buffer.front();

#ifdef DEBUG
        for (int i = 0; i < front.size(); ++i) {
            std::cout << "consume " << front[i][0];
            std::cout << ", " << front[i][1];
            std::cout << ", " << front[i][2] << std::endl;
        }
#endif

        buffer.pop();
        locker.unlock();
        cond.notify_all();
        return front;
    }

}
