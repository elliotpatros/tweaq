#include "parameterhandle.h"

ParameterHandle::ParameterHandle()
{
    init_parameter(p);
}

ParameterHandle::ParameterHandle(const ParameterHandle& other)
{
    copy(other);
}

ParameterHandle& ParameterHandle::operator= (const ParameterHandle& other)
{
    copy(other);
    return *this;
}

ParameterHandle::~ParameterHandle()
{
    free_parameter(p);
}

bool ParameterHandle::hasLabels() const
{
    return p.nLabels > 0;
}

bool ParameterHandle::hasDefaultValue() const
{
    return p.defaultValue != nullptr;
}

bool ParameterHandle::hasDescription() const
{
    return p.description != nullptr;
}

bool ParameterHandle::hasAnyFields() const
{
    return hasLabels() || hasDefaultValue();
}

int ParameterHandle::nFields() const
{
    int n = 0;
    if (hasLabels()) n++;
    if (hasDefaultValue()) n++;

    return n;
}

QString ParameterHandle::description() const
{
    return hasDescription() ? QString(p.description) : QString();
}

QString ParameterHandle::defaultValue() const
{
    return hasDefaultValue() ? QString(p.defaultValue) : QString();
}

int ParameterHandle::nLabels() const
{
    return p.nLabels;
}

QString ParameterHandle::label(const int index) const
{
    return hasLabels() ? QString(p.labels[index]) : QString();
}

QStringList ParameterHandle::labels() const
{
    QStringList list;
    for (int i = 0; i < nLabels(); i++) list << label(i);

    return list;
}

Parameter& ParameterHandle::reference()
{
    return p;
}


// helpers
void ParameterHandle::copy(const ParameterHandle& other)
{
    init_parameter(p);

    if (other.hasDescription())
    {
        set_parameter_description(p, other.p.description);
    }
    if (other.hasDefaultValue())
    {
        set_parameter_default(p, other.p.defaultValue);
    }

    p.nLabels = other.nLabels();
    if (!hasLabels()) return;
    p.labels = (char**)calloc(p.nLabels, sizeof(char*));
    if (p.labels == nullptr) return;

    for (int i = 0; i < p.nLabels; i++)
    {
        set_string(p.labels[i], other.p.labels[i]);
        if (p.labels[i] == nullptr)
        {
            free_string_list(p.labels, p.nLabels);
            return;
        }
    }
}
