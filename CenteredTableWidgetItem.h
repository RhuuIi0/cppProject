#ifndef CENTEREDTABLEWIDGETITEM_H
#define CENTEREDTABLEWIDGETITEM_H

#endif // CENTEREDTABLEWIDGETITEM_H

#include <QTableWidgetItem>

class CenteredTableWidgetItem : public QTableWidgetItem
{
public:
    CenteredTableWidgetItem(const QString &text = QString()) : QTableWidgetItem(text)
    {
        setTextAlignment(Qt::AlignCenter);
    }
};
