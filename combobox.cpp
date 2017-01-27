#include "combobox.h"

// static members
const int ComboBox::_nTitleItems = 2;

// constructor
ComboBox::ComboBox(QWidget* const parent) :
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
void ComboBox::setTitle(const QString title)
{
    setItemText(0, title);
}
