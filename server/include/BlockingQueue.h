#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
#include <vector>
#include <queue>
#include "base/cvec.h"
#include <mutex>

namespace avt {

    class BlockingQueue {

    public:
        BlockingQueue(int maxSize = 20);
        ~BlockingQueue();

        void send(std::vector<Cvec3f>& points);
        const std::vector<Cvec3f>& consume();

    private:

        bool isFull()  const { return buffer.size() >= MAX_SIZE;}
        bool isEmpty() const { return buffer.size() == 0; }

        std::mutex mu;
        std::condition_variable cond;
        std::queue<std::vector<Cvec3f>> buffer;
        const int MAX_SIZE;
    };
}
#endif /* BLOCKINGQUEUE_H */
