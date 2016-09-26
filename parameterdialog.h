#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

// application class
#include "m_tweaq.h"

// base class
#include <QDialog>

// widgets
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

// tweaq data structures
#include "parameterhandle.h"

class ParameterDialog : public QDialog
{
    Q_OBJECT

public:
    // constructor
    explicit ParameterDialog(QWidget* parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());

    // sets
    void appendParameterFields(const vector<ParameterHandle>* parameters);
    t_int makeAndShow(const QString title);

    // gets
    void getEntries(QStringList* entries);


private:
    // child widgets
    QLabel* _label;
    vector<QLineEdit*> _lineEditEntries;
    vector<QComboBox*> _comboBoxEntries;
    QPushButton* _buttonAccept;
    QPushButton* _buttonCancel;
    QGridLayout* _layout;

    // convenience
    t_int lastRow(void) const;

    // static members
    static t_int _nColumns, _lastColumn, _buttonSpan;
    static QString _blankEntry;
};

#endif // PARAMETERDIALOG_H
