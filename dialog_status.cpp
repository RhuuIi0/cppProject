#include "dialog_status.h"
#include "ui_dialog_status.h"

Dialog_Status::Dialog_Status(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Status)
{
    ui->setupUi(this);
}

Dialog_Status:: Dialog_Status(QString str, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Status)
{
    ui->setupUi(this);

    m_strData = str;

    // 初始化group，并将单选框添加到group
    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->addButton(ui->radio_zhengchang);
    m_buttonGroup->addButton(ui->radio_chidao);
    m_buttonGroup->addButton(ui->radio_chidao2);
    m_buttonGroup->addButton(ui->radio_nianjia);
    m_buttonGroup->addButton(ui->radio_shijia);
    m_buttonGroup->addButton(ui->radio_kuanggong);
    m_buttonGroup->addButton(ui->radio_baifang);
}

Dialog_Status::~Dialog_Status()
{
    delete ui;
}

void Dialog_Status::on_btn_Entry_clicked()
{
    QRadioButton* selectRadio = (QRadioButton*)m_buttonGroup->checkedButton();
    if(selectRadio){
        // 获取当前选中项
        QString strSelectRadio = selectRadio->text();
        m_strData.append(strSelectRadio);
        sendAddData(m_strData);
    }
    else{
        // 没有被选中
        return ;
    }

    return ;
}

