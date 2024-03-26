#include "addnianjiadialog.h"
#include "ui_addnianjiadialog.h"
#include <QMessageBox>

AddNianjiaDialog::AddNianjiaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNianjiaDialog)
{
    ui->setupUi(this);

    // 设置只允许输入浮点数 0-5 保留一位小数
    QDoubleValidator *validator = new QDoubleValidator(0, 5, 1, this);
    ui->edt_days->setValidator(validator);
}

AddNianjiaDialog::~AddNianjiaDialog()
{
    delete ui;
}

void AddNianjiaDialog::on_pushButton_clicked()
{
    QString strDate = ui->edt_date->text();
    QString strDays = ui->edt_days->text();

    if(!strDays.isEmpty())
    {
        // 发送信号
        QString strData = strDate + " " + strDays;
        sendAddData(strData);
    }
    else
    {
        QMessageBox::information(this, "error", "天数不能为空", "OK");
    }

    this->close();
    return ;
}
