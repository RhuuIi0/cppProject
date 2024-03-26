#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 连接数据库
    ConnectSQL();

    // 初始化table comboBox
    InitTableWidget_info();
    InitTableWidget_nianjia();
    InitComboBox();


    // 初始化当前年份
    m_nSearchYear = QDate::currentDate().year();
    // 更新label标签
    ui->label_Year->setText(QString::number(m_nSearchYear));

    // 初始化对话框
    m_addDialog = new AddDialog(this);
    m_delDialog = new DelDialog(this);
    m_addNianjiaDialog = new AddNianjiaDialog(this);

    // 绑定adddata dialog
    connect(ui->btn_AddData, &QPushButton::clicked, m_addDialog, &AddDialog::open);             // 单击确认打开对话框
    connect(m_addDialog, SIGNAL(sendAddData(QString)), this, SLOT(receiveAddData(QString)));    // 连接接受数据信号

    // 绑定 deldata dialog
    connect(ui->btn_DelData, &QPushButton::clicked, m_delDialog, &DelDialog::open);
    connect(m_delDialog, SIGNAL(sendDelData(QString)), this, SLOT(receiveDelData(QString)));

    // 绑定 addnianjia dialog
    connect(m_addNianjiaDialog, SIGNAL(sendAddData(QString)), this, SLOT(receiveAddData_nianjia(QString)));

    // 创建右键菜单
    CreateRightMenu();

    // 清除过期数据
    ClearData();
}

MainWindow::~MainWindow()
{
    // 关闭数据库连接
    m_db.close();

    delete ui;
}

void MainWindow::ClearData()
{
    // delete from nianjia where year(in_date)=2021;
    QSqlQuery query;
    int nDelDate = QDate::currentDate().year() - 3;
    QString strSqlExec = "delete from nianjia where year(in_date)=" +
                         QString::number(nDelDate) +
                         ";";

    if(!query.exec(strSqlExec))
    {
        QMessageBox::information(this, "error", "ClearData", "OK");
    }

    return ;
}

void MainWindow::InitComboBox()
{
    QStringList strMonthList;
    strMonthList << "1" << "2" << "3" << "4"
                 << "5" << "6" << "7" << "8"
                 << "9" << "10" << "11" << "12";
    ui->comboBox_Month->addItems(strMonthList);
}

void MainWindow::InitTableWidget_nianjia()
{
    QStringList strList;
    strList<< "请假日期"<< "天数";

    InitTableWidget(ui->table_nianjia, strList, 2);

    ui->table_nianjia->setColumnWidth(0, 150);       // 设置列宽
    ui->table_nianjia->setColumnWidth(1, 80);
}

void MainWindow::InitTableWidget_info()
{
    QStringList strList;
    strList<< "姓名"<< "入职时间"<< "是否有年假" << "已休年假" << "剩余年假";

    InitTableWidget(ui->table_Information, strList, 5);

    ui->table_Information->setColumnWidth(0, 80);       // 设置列宽
    ui->table_Information->setColumnWidth(1, 200);
    ui->table_Information->setColumnWidth(2, 150);
    ui->table_Information->setColumnWidth(3, 100);
    ui->table_Information->setColumnWidth(4, 100);
}

void MainWindow::InitTableWidget(QTableWidget* tableWidget, const QStringList& strList, int nColumnCount)
{
    tableWidget->setColumnCount(nColumnCount);           // 设置列数
//    tableWidget->setRowCount(3);              // 设置行数
//    tableWidget->setFocusPolicy(Qt::NoFocus); // 去除选中虚线框
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
                                                        // 设置整行选取
//    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
                                                        // 设置只能选取单行
    tableWidget->setHorizontalHeaderLabels(strList);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
                                                        // 不允许编辑
//    tableWidget->resizeColumnsToContents();   // 自动调整列宽
    tableWidget->setAlternatingRowColors(true);// 开启交替背景色
    tableWidget->setFont(QFont("新宋体", 15));
    tableWidget->verticalHeader()->setDefaultSectionSize(25);
                                                        // 设置默认行高

    // 表头style设置
    QHeaderView *header = tableWidget->horizontalHeader();
                                                        // 获取表头对象
    header->setFixedHeight(40);                         // 设置表头行高
    header->setStyleSheet("QHeaderView::section { "     // 设置表头背景色
                          "background-color: lightyellow; "
                          "color: black; "
                          "font-family: 新宋体; "
                          "font-size: 25; "
                          "border: 1px outset #1E90FF;"
                          "}"
    );
}

void MainWindow::ConnectSQL()
{
    // 连接mysql
    m_db = QSqlDatabase::addDatabase("QODBC");
    m_db.setHostName("192.168.1.130");
    m_db.setPort(3306);
    m_db.setDatabaseName("testConnet");
    m_db.setUserName("shareUser");
    m_db.setPassword("000000");
    bool IsOpen = m_db.open();
    if(IsOpen)
    {
        qDebug()<<"MySQL Connect Success!";
    }
    else
    {
        QSqlError error = m_db.lastError();
        QMessageBox::information(this, "error", error.text());
    }

    return ;
}

/*------------------------------------*/
bool MainWindow::WriteToSQL(QString strTable, QString strData)
{
    // 张三 2022/12/22
    // insert into information values("邸玉婷", "2019/7/1");
    QSqlQuery query;
    QStringList strList = strData.split(" ");               // 空格分隔字符串

    // data -> information(table) 写入数据
    QString strInsert = "insert into " + strTable + " values(\"" + strList[0] + "\", \"" + strList[1] + "\");";
    bool bResult = query.exec(strInsert);

    return bResult;
}

bool MainWindow::DelToSQL(QString strTable, QString strData)
{
    // delete from information where name="dd";
    bool bResult = false;
    QSqlQuery query;
    QString strDelete = "delete from " + strTable + " where name=\"" + strData + "\";";

    bResult = query.exec(strDelete);

    return bResult;
}

/*------------------------------------*/

void MainWindow::on_btn_Import_clicked()
{
    // 批量导入数据
    // 读取文件
    QFile file("Information.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgbox;
        msgbox.setWindowTitle("Tip");
        msgbox.setText("文件打开失败");
        msgbox.exec();
        return ;
    }

    while(!file.atEnd())
    {
        QByteArray aryLine = file.readLine();
        // 向数据库中写入数据
        if(!WriteToSQL("information", QString(aryLine)))
        {
            QMessageBox::information(this, "error", "SQL Write Error", "OK");
            return ;
        }
    }
    return ;
}

void MainWindow::receiveAddData(QString strData)
{
    // 接收子窗口发送的数据
    // 向数据库中写入数据
    if(!WriteToSQL("information", QString(strData)))
    {
        QMessageBox::information(this, "error", "SQL Write Error", "OK");
    }
    else
    {
        QMessageBox::information(this, "Tip", "Success", "OK");
    }

    return ;
}

void MainWindow::receiveDelData(QString strData)
{
    // 槽函数
    // 删除数据库的中的数据
    bool bDel = DelToSQL("nianjia", QString(strData));
    bool bDelInfo = DelToSQL("information", QString(strData));
    if(bDel && bDelInfo)
    {
        QMessageBox::information(this, "Tip", "Success", "OK");
    }
    else
    {
        QMessageBox::information(this, "error", "SQL Write Error", "OK");
    }

    return ;
}

void MainWindow::on_btn_SearchEntry_clicked()
{
    QString strSearchData;
    QSqlQuery query;

    // 清空 QList<INFORNATION>
    m_listInfo.clear();

    // 更新label标签
    ui->label_Year->setText(QString::number(m_nSearchYear));

    if(ui->checkBox_Name->isChecked())
    {
        // 姓名搜索
        strSearchData = ui->edt_Search->text();             // 获取输入的姓名
        if(!strSearchData.isEmpty())
        {
            // select * from information where name="<name>"
            QString strTemp = "select * from information where name=\"" + strSearchData + "\";";
            if(!query.exec(strTemp))
            {
                QMessageBox::question(this, "error", "Search with name", "OK");
                return ;
            }

            // 判断查询结果是否为空
            if(!query.next())
            {
                QMessageBox::question(this, "error", "查无此人", "OK");
                return ;
            }
        }
        else
        {
            QMessageBox::question(this, "error", "姓名不能为空", "OK");
            return ;
        }
    }
    else if(ui->checkBox_Month->isChecked())
    {
        // 月份搜索
        strSearchData = ui->comboBox_Month->currentText();

        // select * from information where month(in_date)=7;
        QString strTemp = "select * from information where month(in_date)=" + strSearchData;
        if(!query.exec(strTemp))
        {
            QMessageBox::question(this, "error", "Search with month", "OK");
            return ;
        }

        // 判断查询结果是否为空
        if(!query.next())
        {
            QMessageBox::question(this, "error", "本月没人入职", "OK");
            return ;
        }
    }
    else
    {
        return ;
    }

    // 通过 information 表查询 nianjia 表详情
//    query.first();                                              // 重置 query 游标
    SearchInformation(query);
}

void MainWindow::SearchInformation(QSqlQuery query)
{
    INFORMATION infoTemp;
    QString strName;
    QString strBeginDate;
    QString strEndDate;
    float fResult;

    do
    {
        // 向结构体写入数据
        infoTemp.strName = query.value(0).toString();           // 姓名
        infoTemp.dateIn = query.value(1).toDate();              // 入职日期
        m_listInfo.append(infoTemp);
//        QMessageBox::information(this, "tip", infoTemp.strName + infoTemp.dateIn.toString(), "OK");
    }while(query.next());

    // 搜索nianjia表，完善数据
    // 是否有年假 已休年假 剩余年假
    QDate nCurrentDate = QDate::currentDate();
    for(auto it = m_listInfo.begin(); it != m_listInfo.end(); ++it)
    {
        if((m_nSearchYear - (*it).dateIn.year() >= 2) && (m_nSearchYear != nCurrentDate.year()))
        {
            (*it).strIsHave = "是";
        }
        else if((m_nSearchYear - (*it).dateIn.year() > 2) && (m_nSearchYear == nCurrentDate.year()))
        {
            (*it).strIsHave = "是";
        }
        else if((m_nSearchYear - (*it).dateIn.year() == 2) && (m_nSearchYear == nCurrentDate.year()))
        {
            int nCurrent = (nCurrentDate.month() * 100) + nCurrentDate.day();       // 当前日期的月份+日期
            int nIn = ((*it).dateIn.month() * 100) + (*it).dateIn.day();            // 入职日期的月份+日期

            if(nCurrent >= nIn)
            {
                (*it).strIsHave = "是";
            }
            else
            {
                (*it).strIsHave = "否";
            }
        }
        else
        {
            (*it).strIsHave = "否";
        }

        // 已休年假 -> name date区间()  (2024/5/3 2025/5/3)
        strName = (*it).strName;
        strBeginDate = QDate(m_nSearchYear, it->dateIn.month(), it->dateIn.day()).toString("yyyy/MM/dd");
        strEndDate = QDate(m_nSearchYear+1, it->dateIn.month(), it->dateIn.day()).toString("yyyy/MM/dd");
        fResult = CalcUsedDays(strName, strBeginDate, strEndDate);
        (*it).fUsedDays = fResult;

        // 剩余年假
        if((*it).strIsHave == "否")
        {
            (*it).fUnused = 0;
        }
        else
        {
            (*it).fUnused = 5.0 - fResult;
        }
    }

    // 显示到表中
    ShowInfo();
}

float MainWindow::CalcUsedDays(const QString& strName, const QString& strBeginDate, const QString& strEndDate)
{
    QSqlQuery query;
    float fUsedDays = 1.0;

    // select sum(days) from nianjia where name="白云" and in_date BETWEEN "2021/6/26" and "2022/6/26" group by name;
    QString strSql = "select sum(days) from nianjia where name=\"" +
                     strName +
                     "\" and in_date BETWEEN \"" +
                     strBeginDate +
                     "\" and \"" +
                     strEndDate +
                     "\" group by name;";

    if(!query.exec(strSql))
    {
        QMessageBox::question(this, "error", "CalcUseDays", "OK");
        return  -1;
    }
    if(!query.next())
    {
        // 如果查询结果为空
        return 0;
    }
    fUsedDays = query.value(0).toFloat();

    return fUsedDays;
}

void MainWindow::ShowInfo()
{
    // QList<INFORMATION>数据显示到表格中
    ui->table_Information->setSortingEnabled(false);

    // 插入行数
    ui->table_Information->setRowCount(m_listInfo.count());

    for(int i = 0; i < m_listInfo.count(); i++)
    {
        ui->table_Information->setItem(i, 0, new CenteredTableWidgetItem(m_listInfo[i].strName));
        ui->table_Information->setItem(i, 1,
                                       new CenteredTableWidgetItem(m_listInfo[i].dateIn.toString("yyyy/MM/dd"))
                                       );
        ui->table_Information->setItem(i, 2, new CenteredTableWidgetItem(m_listInfo[i].strIsHave));
        ui->table_Information->setItem(i, 3,
                                       new CenteredTableWidgetItem(QString::number(m_listInfo[i].fUsedDays, 'f', 1))
                                       );
        ui->table_Information->setItem(i, 4,
                                       new CenteredTableWidgetItem(QString::number(m_listInfo[i].fUnused, 'f', 1))
                                       );
    }

    // 开启自动排序
    ui->table_Information->setSortingEnabled(true);
}


void MainWindow::on_btn_Pre_clicked()
{
    // eg: CurrrntYear = 2024
    // min = 2024-2 = 2022
    int nMinYear = QDate::currentDate().year() - 2;
    if(m_nSearchYear > nMinYear)
    {
        m_nSearchYear--;
        on_btn_SearchEntry_clicked();
    }

    return ;
}


void MainWindow::on_btn_Next_clicked()
{
    if(m_nSearchYear < QDate::currentDate().year())
    {
        m_nSearchYear++;
        on_btn_SearchEntry_clicked();
    }

    return ;
}


void MainWindow::on_table_Information_doubleClicked(const QModelIndex &index)
{
    m_SelectRow = index;
    // 显示label_name 标签
    ui->label_Name->setText(m_listInfo[m_SelectRow.row()].strName);
    // 从 QList<Info> 获取数据
    // 删除所有数据
    while(ui->table_nianjia->rowCount() > 0)
    {
        ui->table_nianjia->removeRow(0);
    }
//    QMessageBox::information(this, "tip", m_listInfo[index.row()].strName, "OK");
    QString strBeginDate;
    QString strEndDate;
    QSqlQuery query;
    QString strSqlExec;

    // select in_date from nianjia where name="白云" and in_date BETWEEN "2021/6/26" and "2022/6/26";
        // 年假区间
    strBeginDate = QDate(
                        m_nSearchYear,
                        m_listInfo[index.row()].dateIn.month(),
                        m_listInfo[index.row()].dateIn.day()
                   ).toString("yyyy/MM/dd");
    strEndDate = QDate(
                     m_nSearchYear+1,
                     m_listInfo[index.row()].dateIn.month(),
                     m_listInfo[index.row()].dateIn.day()
                 ).toString("yyyy/MM/dd");

    strSqlExec = "select in_date, days from nianjia where name=\"" +
                 m_listInfo[index.row()].strName +
                 "\" and in_date BETWEEN \"" +
                 strBeginDate +
                 "\" and \"" +
                 strEndDate +
                 "\";";
    if(!query.exec(strSqlExec))
    {
        // 是否执行成功
        QMessageBox::question(this, "error", "get nianjia data", "OK");
        return ;
    }
    if(!query.next())
    {
        // 结果是否为空
        return ;
    }

    // 将数据显示到 ninajia 表中
    int nRowCount = ui->table_nianjia->rowCount();
    do
    {
        // 插入新行
        ui->table_nianjia->insertRow(nRowCount);

        // 请假日期
        ui->table_nianjia->setItem(nRowCount, 0,
                                   new CenteredTableWidgetItem(query.value(0).toString()));
        // 请假天数
        ui->table_nianjia->setItem(nRowCount, 1,
                                   new CenteredTableWidgetItem(query.value(1).toString()));

        nRowCount++;
    }while(query.next());
}


void MainWindow::CreateRightMenu()
{
    // 创建右键菜单
    m_rightClickMenu = new QMenu(ui->table_nianjia);

    // 添加增加行的动作
    m_addAction = new QAction(tr("增加"), m_rightClickMenu);
    connect(m_addAction, &QAction::triggered, this, &MainWindow::onAddRow);
    m_rightClickMenu->addAction(m_addAction);

    // 添加删除行的动作
    m_deleteAction = new QAction(tr("删除"), m_rightClickMenu);
    m_deleteAction->setEnabled(false);                              // 默认禁用删除行操作
    connect(m_deleteAction, &QAction::triggered, this, &MainWindow::onDeleteRow);
    m_rightClickMenu->addAction(m_deleteAction);

    // 设置QTableWidget的上下文菜单策略
    ui->table_nianjia->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->table_nianjia, &QTableWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    // 检查是否有选中项
    QModelIndex index = ui->table_nianjia->indexAt(pos);
    bool hasSelection = !ui->table_nianjia->selectedItems().isEmpty();

    // 如果有选中项，则启用删除行操作
    m_deleteAction->setEnabled(hasSelection);

    m_rightClickMenu->exec(ui->table_nianjia->viewport()->mapToGlobal(pos));
}

void MainWindow::onAddRow()
{
    // 判断是否有年假
    if(m_listInfo[m_SelectRow.row()].strIsHave == "是")
    {
        m_addNianjiaDialog->open();
    }
    else
    {
        QMessageBox::information(this, "tip", "该成员没有年假", "OK");
    }
}

void MainWindow::receiveAddData_nianjia(QString strData)
{
    // insert into nianjia values("白云", "2022/09/30", 1.5);
    QSqlQuery query;
    QStringList strList = strData.split(" ");               // 空格分隔字符串

    QString strSqlExec = "insert into nianjia values(\"" +
                         m_listInfo[m_SelectRow.row()].strName +
                         "\", \"" +
                         strList[0] +
                         "\", " +
                         strList[1] +
                         ");";

    if(!query.exec(strSqlExec))
    {
        QMessageBox::question(this, "error", "add nianjia", "OK");
    }
    else
    {
        QMessageBox::information(this, "Tip", "Success", "OK");
    }

    // 更新界面
    on_btn_SearchEntry_clicked();
    on_table_Information_doubleClicked(m_SelectRow);

    return ;
}

void MainWindow::onDeleteRow()
{
    QSqlQuery query;
    QList<QTableWidgetItem*> selectItems = ui->table_nianjia->selectedItems();
    if(!selectItems.isEmpty())
    {
        // delete from nianjia where name="白云" and in_date="2021/08/31" and days=1.5;
        QString strSqlExec = "delete from nianjia where name=\"" +
                             m_listInfo[m_SelectRow.row()].strName +
                             "\" and in_date=\"" +
                             selectItems[0]->text() +
                             "\" and days=" +
                             selectItems[1]->text() +
                             ";";

        if(!query.exec(strSqlExec))
        {
            // 是否执行成功
            QMessageBox::question(this, "error", "nianjia delete data", "OK");
            return ;
        }
        else
        {
            // 刷新表
            on_btn_SearchEntry_clicked();
            on_table_Information_doubleClicked(m_SelectRow);
        }
    }

    return ;
}
