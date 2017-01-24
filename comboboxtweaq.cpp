#include "comboboxtweaq.h"

// static members
const int ComboBoxTweaq::_nTitleItems = 2;

// constructor
ComboBoxTweaq::ComboBoxTweaq(QWidget* const parent) :
    QComboBox(parent)
{
    // setup connection rules
    disconnect();

    // add a title
    addItem(" ");
    setItemData(0, false, Qt::UserRole - 1);
    insertSeparator(1);
}


// sets
void ComboBoxTweaq::setTitle(const QString title)
{
    setItemText(0, title);
}
