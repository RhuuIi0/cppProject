#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDialog(QWidget *parent = nullptr);
    ~AddDialog();

private slots:
    void on_btn_Entry_clicked();

signals:
    void sendAddData(QString);      // 定义sendData信号

private:
    Ui::AddDialog *ui;
};

#endif // ADDDIALOG_H
