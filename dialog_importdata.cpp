#include "dialog_importdata.h"
#include "ui_dialog_importdata.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>

Dialog_ImportData::Dialog_ImportData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ImportData)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);

}

Dialog_ImportData::~Dialog_ImportData()
{
    delete ui;
}

void Dialog_ImportData::on_btn_selectFile_clicked()
{
    QString selectedFile = QFileDialog::getOpenFileName(nullptr, tr("选择文件"), "", tr("所有文件 (*.*);;文本文件 (*.txt)"));

    if (!selectedFile.isEmpty()) {
        // 如果用户选择了文件，显示文件路径
        ui->edt_filePath->setText(selectedFile);
    }
    else{
        QMessageBox::information(nullptr, "选择的文件", "请选择文件");
        return ;
    }

    // 读取数据到list中
    QFile file(selectedFile);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::information(this, "error", "opne file error");
        return ;
    }

    while(!file.atEnd()){
        QByteArray aryLine = file.readLine();
        QStringList strList = QString(aryLine).split(" ");
        strList[1].chop(1);                         // 去除行尾的换行
    }

}

