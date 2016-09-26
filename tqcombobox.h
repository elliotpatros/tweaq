#ifndef TQCOMBOBOX_H
#define TQCOMBOBOX_H

// application header
#include "m_tweaq.h"

// base class
#include <QComboBox>

class TqComboBox : public QComboBox
{
    Q_OBJECT

public:
    // constructor
    explicit TqComboBox(QWidget* parent = Q_NULLPTR);

    // gets
    t_int processIndex(void) const;

    // sets
    void setTitle(const QString title);
    void setSelectionToTitleIndex(void);


private:
    static const t_int _nTitleItems;
};

#endif // TQCOMBOBOX_H
