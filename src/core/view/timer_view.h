//
// Created by chiendd on 14/08/2024.
//

#ifndef TIMERVIEW_H
#define TIMERVIEW_H
#include <chrono>
#include "view.h"

class Timer {
protected:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<clock_t>  started;
public:

    Timer() : started(clock_t::now()) {}

    void reset() {
        started = clock_t::now();
    }

    double elapsed() const {
        return std::chrono::duration_cast<second_t>(clock_t::now() - started).count();
    }
};

namespace Csql {
    class TimerView : public View {
    public:
        TimerView() {}

        bool show(std::ostream& anOutput) {
            static Timer theTimer;
            anOutput << " (" << std::fixed << theTimer.elapsed() << " sec)" << std::endl;
            return true;
        }
    };
}
#endif //TIMERVIEW_H
