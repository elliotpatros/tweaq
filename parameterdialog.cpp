#include "parameterdialog.h"

ParameterDialog::ParameterDialog(const QString title, QWidget* parent) :
    QDialog(parent, Qt::WindowFlags()),
    _layout(new ParameterDialogLayout(this)),
    _label(new QLabel(this)),
    _buttonAccept(new QPushButton(this)),
    _buttonCancel(new QPushButton(this))
{
    // custom connection rules
    disconnect();
    _layout->disconnect();
    _label->disconnect();
    _buttonCancel->disconnect();
    _buttonAccept->disconnect();
    connect(_buttonCancel, SIGNAL(released()), this, SLOT(reject()));
    connect(_buttonAccept, SIGNAL(released()), this, SLOT(accept()));

    // setup layout
    setLayout(_layout);

    // setup label
    _label->setAlignment(Qt::AlignHCenter);
    _label->setText(title);
    _layout->addRow(_label);

    // setup buttons
    _buttonCancel->setText("Cancel");
    _buttonAccept->setText("Ok");
}


// gets
vector<QString> ParameterDialog::entries() const
{
    const size_t nLineEdits  = _lineEdits.size();
    const size_t nComboBoxes = _comboBoxes.size();

    vector<QString> list;
    if (nLineEdits != nComboBoxes) return list;

    list.reserve(nLineEdits + nComboBoxes);
    for (size_t i = 0; i < nLineEdits; i++)
    {
        if (_lineEdits [i] != nullptr) list.emplace_back(_lineEdits [i]->text());
        if (_comboBoxes[i] != nullptr) list.emplace_back(_comboBoxes[i]->currentText());
    }

    return list;
}


// sets
int ParameterDialog::show(vector<ParameterHandle> handles)
{
    // add parameter fields to layout
    for (const auto& handle : handles)
    {   // setup a new line editor and combo box
        setupLineEdit(handle);
        setupComboBox(handle);

        // add the widgets to the layout
        _layout->addRow(_lineEdits.back(), _comboBoxes.back());
    }

    // add cancel and ok buttons to layout
    _layout->addRow(_buttonCancel, _buttonAccept);

    // show modal dialog
    checkForCompletedFields();
    setWindowModality(Qt::WindowModal);
    return exec();
}

// helpers
void ParameterDialog::setupLineEdit(const ParameterHandle& handle)
{
    _lineEdits.push_back(nullptr);
    if (handle.hasDefaultValue())
    {
        _lineEdits.back() = new QLineEdit(this);
        if (_lineEdits.back() == nullptr) return;

        _lineEdits.back()->disconnect();
        _lineEdits.back()->setPlaceholderText(handle.defaultValue());
        _lineEdits.back()->setValidator(new QDoubleValidator(this));
        if (handle.hasDescription()) _lineEdits.back()->setToolTip(handle.description());

        connect(_lineEdits.back(), SIGNAL(textChanged(const QString&)), SLOT(checkForCompletedFields()));
    }
}

void ParameterDialog::setupComboBox(const ParameterHandle& handle)
{
    _comboBoxes.push_back(nullptr);
    if (handle.hasLabels())
    {
        _comboBoxes.back() = new QComboBox(this);
        if (_comboBoxes.back() == nullptr) return;

        _comboBoxes.back()->disconnect();
        _comboBoxes.back()->addItems(handle.labels());
        if (handle.hasDescription()) _comboBoxes.back()->setToolTip(handle.description());
    }
}

void ParameterDialog::checkForCompletedFields() const
{
    const auto allTextFieldsHaveEntries = std::all_of(_lineEdits.begin(),
                                                      _lineEdits.end(),
                                                      [](const QLineEdit* const field)
    {
        return field == nullptr || !field->text().isEmpty();
    });

    _buttonAccept->setEnabled(allTextFieldsHaveEntries);
}
