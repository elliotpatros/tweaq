#include "parameterdialog.h"

//==============================================================================
// static members
//==============================================================================
t_int ParameterDialog::_nColumns = 2;
t_int ParameterDialog::_lastColumn = ParameterDialog::_nColumns - 1;
t_int ParameterDialog::_buttonSpan = ParameterDialog::_nColumns / 2;
QString ParameterDialog::_blankEntry = QStringLiteral("_BLANK_");


//==============================================================================
// constructor
//==============================================================================
ParameterDialog::ParameterDialog(QWidget *parent, Qt::WindowFlags flags) :
    QDialog(parent, flags)
{
    // make child widgets
    _label = new QLabel(this);
    _layout = new QGridLayout(this);
    _buttonAccept = new QPushButton(this);
    _buttonCancel = new QPushButton(this);

    // setup...
    // ...main layout
    setLayout(_layout);

    // ...window title
    _label->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    _layout->addWidget(_label, 0, 0, 1, _nColumns);

    // ...confirm & cancel button row (but wait to add them until last)
    _buttonCancel->setText(QStringLiteral("Cancel"));
    _buttonCancel->disconnect();
    connect(_buttonCancel, SIGNAL(released()), this, SLOT(reject()));

    _buttonAccept->setText(QStringLiteral("Ok"));
    _buttonAccept->disconnect();
    connect(_buttonAccept, SIGNAL(released()), this, SLOT(accept()));
}


//==============================================================================
// sets
//==============================================================================
void ParameterDialog::appendParameterFields(const vector<ParameterHandle>* parameters)
{
    const t_uint nParameters = parameters->size();
    for (t_uint pIndex = 0; pIndex < nParameters; ++pIndex)
    {
        // get pointer to parameter handle
        const ParameterHandle* handle = &parameters->at(pIndex);

        // add a line edit
        _lineEditEntries.emplace_back(new QLineEdit(this));
        QLineEdit* editor = _lineEditEntries.back();

        // should we use the line editor?
        if (handle->get().shouldUseDefault == TQ_TRUE)
        {
            editor->setPlaceholderText(QString::fromUtf8(handle->get().name));
            editor->setText(QString::number(handle->get().defaultValue));
            _layout->addWidget(editor, lastRow(), 0, 1, _lastColumn);
        }
        else
        {
            editor->setText(_blankEntry);
        }

        // add label(s)
        _comboBoxEntries.emplace_back(new QComboBox(this));
        QComboBox* labels = _comboBoxEntries.back();

        // should we use a combo box or a label?
        const t_uint nLabels = handle->get().nLabels;
        if (nLabels > 1)
        {   // add a combo box
            for (t_uint i = 0; i < nLabels; ++i)
            {
                labels->addItem(QString::fromUtf8(handle->get().labels[i]));
            }

            _layout->addWidget(labels, lastRow() - 1, _lastColumn, 1, 1);
        }
        else // use a static text label
        {
            QLabel* label = new QLabel(this);
            label->setText(handle->get().labels[0]);
            _layout->addWidget(label, lastRow() - 1, 0, 1, _nColumns);

            // and make sure we can tell the combo box wasn't used here
            labels->addItem(_blankEntry);
            labels->setCurrentIndex(0);
        }
    }
}

t_int ParameterDialog::makeAndShow(const QString title)
{
    // set title text
    _label->setText(title);

    // add confirmation button layout to this dialog ("ok" and "cancel")
    _layout->addWidget(_buttonCancel, lastRow(), 0, 1, _buttonSpan);
    _layout->addWidget(_buttonAccept, lastRow() - 1, _buttonSpan, 1, _buttonSpan);

    // show yourself
    setWindowModality(Qt::WindowModal);
    return exec();
}

t_int ParameterDialog::lastRow(void) const
{
    return _layout->rowCount();
}


//==============================================================================
// gets
//==============================================================================
void ParameterDialog::getEntries(QStringList* entries)
{
    // the size of _comboBoxEntries and _lineEditEntries should be the same
    // if one of them wasn't used, it'll have a blank entry as text
    if(_comboBoxEntries.size() != _lineEditEntries.size())
    {
        qDebug() << "start panicking";
    }

    const t_uint nEntries = _comboBoxEntries.size();
    for (t_uint i = 0; i < nEntries; ++i)
    {
        // check the line edit
        QString entry = _lineEditEntries[i]->text();
        if (entry != _blankEntry)
        {
            entries->append(entry);
        }

        // check the combo box
        entry = _comboBoxEntries[i]->currentText();
        if (entry != _blankEntry)
        {
            entries->append(entry);
        }
    }
}
