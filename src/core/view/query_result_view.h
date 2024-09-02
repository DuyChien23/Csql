//
// Created by chiendd on 14/08/2024.
//

#ifndef QUERYRESULTVIEW_H
#define QUERYRESULTVIEW_H
#include <ostream>

#include "timer_view.h"
#include "view.h"

namespace Csql {
    class QueryResultView : public View {
    public:
        QueryResultView(size_t aRowCount, std::string query): numRowsAffected(aRowCount), query(query) {}

        bool show(std::ostream& aStream) {
            aStream << "Query [" << query << "\t] OK, " << numRowsAffected << " rows affected";
            TimerView().show(aStream);
            return true;
        }

    protected:
        size_t numRowsAffected;
        std::string query;
    };
}

#endif //QUERYRESULTVIEW_H
