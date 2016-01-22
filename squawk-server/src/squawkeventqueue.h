#ifndef SQUAWKEVENTQUEUE_H
#define SQUAWKEVENTQUEUE_H

#include <boost/lockfree/spsc_queue.hpp>

class SquawkEventQueue {
public:
    static SquawkEventQueue instance() {
        static SquawkEventQueue event_queue();
        return event_queue;
    }

private:
    SquawkEventQueue();
};
#endif // SQUAWKEVENTQUEUE_H
