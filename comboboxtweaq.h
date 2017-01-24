#ifndef COMBOBOXTWEAQ_H
#define COMBOBOXTWEAQ_H

// application header
#include "tweaq.h"

// base class
#include <QComboBox>

class ComboBoxTweaq : public QComboBox
{
    Q_OBJECT

public:
    explicit ComboBoxTweaq(QWidget* const parent = 0);

    // sets
    void setTitle(const QString title);


protected:
    static const int _nTitleItems;
};

#endif // COMBOBOXTWEAQ_H
