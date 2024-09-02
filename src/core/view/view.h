//
// Created by chiendd on 14/08/2024.
//

#ifndef VIEW_H
#define VIEW_H
#include <ostream>

namespace Csql {
    class View {
    public:
        virtual ~View() {};
        bool    show(std::ostream &aStream);
    };
}

#endif //VIEW_H
