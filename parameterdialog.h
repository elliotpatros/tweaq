#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

// application class
#include "tweaq.h"

// base class
#include <QDialog>

// widgets
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include "parameterdialoglayout.h"

// tweaq classes
#include "parameterhandle.h"

class ParameterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterDialog(const QString title = QString(), QWidget* parent = nullptr);

    // gets
    vector<QString> entries() const;

    // sets
    int show(vector<ParameterHandle> handles);


private slots:
    void checkForCompletedFields() const;


private:
    // child widgets
    ParameterDialogLayout* _layout;
    QLabel* _label;
    vector<QLineEdit*> _lineEdits;
    vector<QComboBox*> _comboBoxes;
    QPushButton* _buttonAccept;
    QPushButton* _buttonCancel;

    // helpers
    void setupLineEdit(const ParameterHandle& handle);
    void setupComboBox(const ParameterHandle& handle);
//    void addParameterFields(vector<ParameterHandle>& handles);
};

#endif // PARAMETERDIALOG_H
