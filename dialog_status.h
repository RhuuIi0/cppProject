#ifndef DIALOG_STATUS_H
#define DIALOG_STATUS_H

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class Dialog_Status;
}

class Dialog_Status : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Status(QWidget *parent = nullptr);
    explicit Dialog_Status(QString str, QWidget *parent = nullptr);
    ~Dialog_Status();

signals:
    void sendAddData(QString strStatu);

private slots:
    void on_btn_Entry_clicked();

private:
    Ui::Dialog_Status *ui;
    QString m_strData;
    QButtonGroup* m_buttonGroup;
};

#endif // DIALOG_STATUS_H
