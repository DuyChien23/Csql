//
// Created by chiendd on 14/08/2024.
//

#include "table_view.h"

#include <iomanip>

#include "timer_view.h"

namespace Csql {
    void TableView::drawRowSeparator(std::ostream &aStream) const {
        aStream << cornerChar;
        for (const size_t& theColWidth : columnWidths)
        {
            aStream << std::setfill(rowChar) << std::setw(theColWidth + kExtraPadding) << "" << cornerChar;
        }
        aStream << '\n';
    }

    void TableView::drawRow(std::ostream &aStream, size_t aStartIndex) const {
        aStream << colChar;
        size_t theEndIndex = aStartIndex + numCols;
        for (size_t theItemIndex = aStartIndex; theItemIndex < theEndIndex; ++theItemIndex)
        {
            aStream << std::left << std::setfill(' ')
                << std::setw(columnWidths[theItemIndex % numCols] + kExtraPadding)
                << data[theItemIndex] << colChar;
        }
        aStream << '\n';
        drawRowSeparator(aStream);
    }

    bool TableView::show(std::ostream &aStream) {
        size_t theNumRows = data.size() / numCols;
        drawRowSeparator(aStream);

        for (size_t theRowIndex = 0; theRowIndex < theNumRows; ++theRowIndex)
        {
            drawRow(aStream, theRowIndex * numCols);
        }

        //Subtract 1 for column headers
        aStream << theNumRows - 1 << " rows in set";

        TimerView().show(aStream);

        return true;
    }
}
