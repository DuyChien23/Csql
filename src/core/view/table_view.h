//
// Created by chiendd on 14/08/2024.
//

#ifndef TABLEVIEW_H
#define TABLEVIEW_H
#include <sstream>
#include <vector>

#include "view.h"

namespace Csql {
    class TableView : public View {
    public:
        template <typename ... Args>
        TableView(Args && ... tableHeaderArgs) {
            (addHeaderElement(tableHeaderArgs), ...);
            numCols = data.size();
        }

        TableView(std::vector<std::string> aTableHeader) {
            for(auto headerElement:aTableHeader){
                addHeaderElement(headerElement);
            }
            numCols = data.size();
        }

        template <typename ... Args>
        void addData(Args && ... dataArgs) {
            size_t theIndex = 0;

            ([&](auto& aElement) {
                std::stringstream theStream;
                theStream << aElement;
                data.push_back(theStream.str());

                size_t &columnWidth = columnWidths.at(theIndex++ % numCols);
                columnWidth = std::max(columnWidth, theStream.str().size());
            }(dataArgs), ...);
        }

        void addData(std::vector<std::string> aData) {
            size_t theIndex = 0;
            for (auto value : aData) {
                data.push_back(value);
                size_t &columnWidth = columnWidths.at(theIndex++ % numCols);
                columnWidth = std::max(columnWidth, value.size());
            }
        }

        bool show(std::ostream& aStream);
    protected:
        std::vector<std::string> data;
        std::vector<size_t> columnWidths;
        size_t numCols;

        const static char cornerChar = '+';
        const static char rowChar = '-';
        const static char colChar = '|';
        const static size_t kExtraPadding = 3;

        template<typename T>
        void addHeaderElement(T aElement) {
            std::stringstream theStream;
            theStream << aElement;
            data.push_back(theStream.str());
            columnWidths.push_back(theStream.str().size());
        }

        void drawRowSeparator(std::ostream& aStream) const;
        void drawRow(std::ostream& aStream, size_t aStartIndex) const;
    };
}


#endif //TABLEVIEW_H
