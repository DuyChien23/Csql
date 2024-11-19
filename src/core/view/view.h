//
// Created by chiendd on 14/08/2024.
//

#ifndef VIEW_H
#define VIEW_H

class View {
    public:
    virtual ~View() = default;
    bool    show(std::ostream &aStream);
};


#endif //VIEW_H
