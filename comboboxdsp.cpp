#include "comboboxdsp.h"

ComboBoxDSP::ComboBoxDSP(QWidget* const parent) :
    ComboBox(parent) {}

bool ComboBoxDSP::processHasBeenChosen() const
{
    return 0 <= processIndex();
}

int ComboBoxDSP::processIndex() const
{
    return currentIndex() - _nTitleItems;
}
