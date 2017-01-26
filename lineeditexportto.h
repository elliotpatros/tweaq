#ifndef LINEEDITEXPORTTO_H
#define LINEEDITEXPORTTO_H

// application header
#include "tweaq.h"

// base class
#include <QLineEdit>

// Qt classes
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDir>
#include <QFileInfo>

class LineEditExportTo : public QLineEdit
{
    Q_OBJECT

public:
    // constructor
    explicit LineEditExportTo(QWidget* parent = nullptr);

    // gets
    QString path() const;
    bool currentPathIsValid() const;


protected:
    // drag and drop
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);


private slots:
    // export to directory
    void validateTextForUser();


private:
    // private members
    static const QString _defaultPath;
    QFileInfo _currentPath;
};

#endif // LINEEDITEXPORTTO_H
