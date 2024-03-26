#ifndef ADDNIANJIADIALOG_H
#define ADDNIANJIADIALOG_H

#include <QDialog>

namespace Ui {
class AddNianjiaDialog;
}

class AddNianjiaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNianjiaDialog(QWidget *parent = nullptr);
    ~AddNianjiaDialog();

private slots:
    void on_pushButton_clicked();

signals:
    void sendAddData(QString);      // 定义sendData信号

private:
    Ui::AddNianjiaDialog *ui;
};

#endif // ADDNIANJIADIALOG_H
