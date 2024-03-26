#include "deldialog.h"
#include "ui_deldialog.h"
#include <QMessageBox>

DelDialog::DelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DelDialog)
{
    ui->setupUi(this);
}

DelDialog::~DelDialog()
{
    delete ui;
}

void DelDialog::on_pushButton_clicked()
{
    QString str = ui->edt_DelName->text();

    if(!str.isEmpty())
    {
        // 发送信号
        sendDelData(str);
    }
    else
    {
        QMessageBox::information(this, "error", "姓名不能为空", "OK");
    }

    this->close();
    return ;
}

