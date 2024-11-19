//
// Created by chiendd on 14/08/2024.
//

#ifndef QUERYRESULTVIEW_H
#define QUERYRESULTVIEW_H
#include <ostream>

#include "view.h"

class QueryResultView : public View {
public:
    QueryResultView(size_t aRowCount, std::string query, bool successful = true)
        : numRowsAffected(aRowCount), query(query), successful(successful) {
    }

    bool show(std::ostream &aStream) {
        if (successful) {
            aStream << "Query [" << query << "\t] OK, " << numRowsAffected << " rows affected\n";
        } else {
            aStream << "Query failed. Cause: " << query << '\n';
        }
        return true;
    }

protected:
    size_t numRowsAffected;
    std::string query;
    bool successful;
};


#endif //QUERYRESULTVIEW_H
