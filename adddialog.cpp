#include "adddialog.h"
#include "ui_adddialog.h"
#include <QMessageBox>

AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog)
{
    ui->setupUi(this);
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::on_btn_Entry_clicked()
{
    QString str = ui->edt_Name->text();

    if(!str.isEmpty())
    {
        // 发送信号
        QString strData = ui->edt_Name->text() + " " + ui->edt_Date->text();
        sendAddData(strData);
    }
    else
    {
        QMessageBox::information(this, "error", "姓名不能为空", "OK");
    }

    this->close();
    return ;
}
