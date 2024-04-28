#ifndef DIALOG_INITDAYS_H
#define DIALOG_INITDAYS_H

#include <QDialog>
#include <QDate>

namespace Ui {
class Dialog_InitDays;
}

class Dialog_InitDays : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_InitDays(QWidget *parent = nullptr);
    explicit Dialog_InitDays(const QDate& date, QWidget *parent = nullptr);
    ~Dialog_InitDays();

signals:
    void sendAddData(int);

private slots:
    void on_btn_Entry_clicked();

private:
    Ui::Dialog_InitDays *ui;

    QDate m_date;
};

#endif // DIALOG_INITDAYS_H
