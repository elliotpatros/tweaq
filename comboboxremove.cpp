#include "comboboxremove.h"

ComboBoxRemove::ComboBoxRemove(QWidget * const parent) :
    ComboBoxTweaq(parent)
{
    // custom connection rules
    connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(doAction(QString)));
}

// sets
void ComboBoxRemove::addNamedAction(const QString name, TweaqAction action)
{
    addItem(name);
    actionMap[name] = action;
}

// slots
void ComboBoxRemove::doAction(QString name)
{
    if (currentIndex() >= _nTitleItems)
    {
        actionMap[name]();
        setCurrentIndex(0);
    }
}
