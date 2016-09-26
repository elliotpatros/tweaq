#include "tqcombobox.h"
//==============================================================================
// static members
//==============================================================================
const t_int TqComboBox::_nTitleItems = 2;


//==============================================================================
// constructor
//==============================================================================
TqComboBox::TqComboBox(QWidget *parent) :
    QComboBox(parent)
{
    // set a title (disabled) and a separator
    addItem(QStringLiteral("title"));
    setItemData(0, false, Qt::UserRole - 1);
    insertSeparator(1);
}


//==============================================================================
// gets
//==============================================================================
t_int TqComboBox::processIndex(void) const
{
    return currentIndex() - _nTitleItems;
}


//==============================================================================
// sets
//==============================================================================
void TqComboBox::setTitle(const QString title)
{
    setItemText(0, title);
}

void TqComboBox::setSelectionToTitleIndex(void)
{
    setCurrentIndex(0);
}
