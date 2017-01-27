#ifndef COMBOBOXTWEAQ_H
#define COMBOBOXTWEAQ_H

// application header
#include "tweaq.h"

// base class
#include <QComboBox>

class ComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit ComboBox(QWidget* const parent = nullptr);

    // sets
    void setTitle(const QString title);


protected:
    static const int _nTitleItems;
};

#endif // COMBOBOXTWEAQ_H
