#ifndef PARAMETERDIALOGLAYOUT_H
#define PARAMETERDIALOGLAYOUT_H

// application header
#include "tweaq.h"

// base class
#include <QGridLayout>

class ParameterDialogLayout : public QGridLayout
{
    Q_OBJECT

public:
    explicit ParameterDialogLayout(QWidget* parent = nullptr);

    void addRow(QWidget* const widget);
    void addRow(QWidget* const lhsWidget, QWidget* const rhsWidget);

private:
    int _nRows;
    static const int _colSpan;
    static const int _rowSpan;

    // helpers
    void addValidRows(QWidget* const lhsWidget, QWidget* const rhsWidget);
};

#endif // PARAMETERDIALOGLAYOUT_H
