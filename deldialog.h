#ifndef DELDIALOG_H
#define DELDIALOG_H

#include <QDialog>

namespace Ui {
class DelDialog;
}

class DelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DelDialog(QWidget *parent = nullptr);
    ~DelDialog();

signals:
    void sendDelData(QString);      // 定义sendData信号

private slots:
    void on_pushButton_clicked();

private:
    Ui::DelDialog *ui;
};

#endif // DELDIALOG_H
