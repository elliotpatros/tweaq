#include "parameterdialoglayout.h"

const int ParameterDialogLayout::_colSpan = 2;
const int ParameterDialogLayout::_rowSpan = 1;

ParameterDialogLayout::ParameterDialogLayout(QWidget* parent) :
    QGridLayout(parent),
    _nRows(0)
{
    disconnect();
}

void ParameterDialogLayout::addRow(QWidget* const widget)
{
    if (widget != nullptr) addWidget(widget, _nRows++, 0, _rowSpan, _colSpan);
}

void ParameterDialogLayout::addRow(QWidget* const lhsWidget, QWidget* const rhsWidget)
{
    if (lhsWidget != nullptr && rhsWidget != nullptr)
    {
        int colStart = 0;
        addWidget(lhsWidget, _nRows, colStart++, _rowSpan, 1);
        addWidget(rhsWidget, _nRows, colStart,   _rowSpan, 1);
        _nRows++;
    }
    else
    {
        addRow(lhsWidget);
        addRow(rhsWidget);
    }
}
