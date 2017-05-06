#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
#include <vector>
#include <queue>

class BlockingQueue {
 public:
    BlockingQueue(int maxSize = 20) : MAX_SIZE(maxSize) {
    }

    ~BlockingQueue() {
    }

    void send(std::vector<Cvec3f>& points) {
        std::unique_lock<std::mutex> locker(mu);
        cond.wait(locker, [this](){return !isFull();});

#ifdef DEBUG
        std::cout << "producing"  << std::endl;
#endif

        std::vector<Cvec3f> copy(points);
        buffer.push(copy);

        locker.unlock();
        cond.notify_all();
    }
    const std::vector<Cvec3f>& consume() {
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

 private:

    bool isFull() const {
        return buffer.size() >= MAX_SIZE;
    }

    bool isEmpty() const {
        return buffer.size() == 0;
    }

    std::mutex mu;
    std::condition_variable cond;

    std::queue<std::vector<Cvec3f>> buffer;
    const int MAX_SIZE;
};

#endif /* BLOCKINGQUEUE_H */
