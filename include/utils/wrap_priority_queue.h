//
// Created by zpx on 2024/06/11.
//

#ifndef TINYWEBSERVER_WRAP_PRIORITY_QUEUE_H
#define TINYWEBSERVER_WRAP_PRIORITY_QUEUE_H

#include <queue>
#include <algorithm>

namespace tiny_web_server {
    template<typename Tp, typename Sequence = std::vector<Tp>, typename Compare  = std::less<typename Sequence::value_type> >
    class WrapPriorityQueue : public std::priority_queue<Tp> {
    public:
        bool remove(const Tp &value) {
            auto it = std::find(this->c.begin(), this->c.end(), value);
            if (it == this->c.end()) {
                return false;
            }
            if (it == this->c.begin()) {
                // deque the top element
                this->pop();
            } else {
                // remove element and re-heap
                this->c.erase(it);
                std::make_heap(this->c.begin(), this->c.end(), this->comp);
            }
            return true;
        }
    };
}
#endif //TINYWEBSERVER_WRAP_PRIORITY_QUEUE_H
