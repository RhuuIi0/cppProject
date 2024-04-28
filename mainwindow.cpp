#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 连接数据库
    if(!ConnectSQL()){
        QMessageBox::information(this, "error", "SQL connect error");
    }

    // 初始化映射表（部门 -> tablewidget）
    Init_mapTableAndDepartment();
    // 初始化日期
    Init_labelDate();
    // 初始化表头、Style、姓名
    ui->tabWidget->setCurrentIndex(0);
    for(auto it : m_mapTableAndDepartment){
        Init_tableStyle(it.second);             // 所有table的style初始化
    }
//    Init_tableFormat(ui->table_zhongjian);      // 第一遍调用列宽没改变
    Init_tableFormat(ui->table_zhongjian);      // 第二遍改变了，原因未知
    Init_tableFormat_Name("中健", ui->table_zhongjian);
    // 创建右键菜单
    CreateRightMenu();

    // 连接槽函数，高亮整行整列
    connectSlots();

    // 初始化快捷键，连接快捷键槽函数
    m_shortcutOpenDialog = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(m_shortcutOpenDialog, &QShortcut::activated, this, &MainWindow::OpenSearchDialog);

    // 初始化delete快捷键
    m_shortcutDelete = new QShortcut(QKeySequence("Delete"), this);
    connect(m_shortcutDelete, &QShortcut::activated, this, &MainWindow::onDeleteData);

    // 给所有的tablewidget绑定双击事件，连接 slot function
    Init_tableDoubleClickEvent();

    // 检查当前日期的应出勤天数是否初始化
    InitDays();

    // 显示信息
    ShowInfo_Update();
}

MainWindow::~MainWindow()
{
    m_db.close();

    delete ui;
}

void MainWindow::OutputDebug(QString strDebug){
    qDebug() << strDebug;
}
void MainWindow::OutputDebug_Box(QString strDebug){
    QMessageBox::information(this, "tip", strDebug);
}

bool MainWindow::ConnectSQL(){
    m_db = QSqlDatabase::addDatabase("QODBC");
    m_db.setHostName("RhuuIi0");
    m_db.setPort(3306);
    m_db.setDatabaseName("nameConnect");
    m_db.setUserName("shareUser");
    m_db.setPassword("000000");

    bool IsOpen = m_db.open();
    if(!IsOpen)
    {
        return false;
    }
    else
    {
        return true;
    }

    return false;
}

void MainWindow::CreateRightMenu(){
    // 创建右键菜单
    m_menuRightClick = new QMenu(this);

    // 添加菜单项
    m_actionImport = new QAction(tr("****import****"), this);
    m_actionPersonal = new QAction(tr("增/删员工"), this);
    m_actionImportData = new QAction(tr("导入数据"), this);
    // 连接槽函数
    connect(m_actionImport, &QAction::triggered, this, &MainWindow::ImportData);
    connect(m_actionPersonal, &QAction::triggered, this, &MainWindow::AddAndDelPersonal);
    connect(m_actionImportData, &QAction::triggered, this, &MainWindow::onImportPersonalData);
    // 将菜单项添加到右键菜单
    m_menuRightClick->addAction(m_actionImportData);
    m_menuRightClick->addAction(m_actionPersonal);
    m_menuRightClick->addAction(m_actionImport);

    // 设置右键菜单
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);
}

void MainWindow::showContextMenu(const QPoint &pos){
    m_menuRightClick->exec(mapToGlobal(pos));
}

void MainWindow::ImportData(){
    // 构造SQL语句
    // insert into information_kaoqin(name, department) VALUES("name", "department")
    QSqlQuery* query = new QSqlQuery;

    // 打开文件
    QFile file("Information.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::information(this, "error", "opne file error");
        return ;
    }

    while(!file.atEnd()){
        QByteArray aryLine = file.readLine();
        QStringList strList = QString(aryLine).split(" ");
        strList[1].chop(1);                         // 去除行尾的换行
        QString strSqlExec =
            "insert into information_kaoqin(name, department) VALUES(\"" +
            strList[0] +
            "\", \"" +
            strList[1] +
            "\");";

        if(!query->exec(strSqlExec)){
            QMessageBox::information(this, "error", "[sql.exec]import fail");
            return ;
        }
    }

    QMessageBox::information(this, "success", "import success");
    return ;
}

void MainWindow::Init_tableDoubleClickEvent(){
    connect(ui->table_zhongjian,    &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_quancai,      &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_zhongda,      &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_quanshikuai,  &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_hantai,       &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_xinghe,       &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_huoyan,       &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_rongkang,     &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_guoxin,       &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_yunfeng,      &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_gangxu,       &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_shangjiu,     &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_zhongzheng,   &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
    connect(ui->table_hengdongying, &QTableWidget::cellDoubleClicked, this, &MainWindow::onDoubleClickEvent);
}

void MainWindow::Init_mapTableAndDepartment(){
    // 初始化tablewidget和部门的对应关系
    m_mapTableAndDepartment.insert(std::make_pair("中健",     ui->table_zhongjian));
    m_mapTableAndDepartment.insert(std::make_pair("全彩",     ui->table_quancai));
    m_mapTableAndDepartment.insert(std::make_pair("忠达",     ui->table_zhongda));
    m_mapTableAndDepartment.insert(std::make_pair("全实快",   ui->table_quanshikuai));
    m_mapTableAndDepartment.insert(std::make_pair("涵泰",     ui->table_hantai));
    m_mapTableAndDepartment.insert(std::make_pair("星禾",     ui->table_xinghe));
    m_mapTableAndDepartment.insert(std::make_pair("火眼",     ui->table_huoyan));
    m_mapTableAndDepartment.insert(std::make_pair("荣康",     ui->table_rongkang));
    m_mapTableAndDepartment.insert(std::make_pair("国鑫",     ui->table_guoxin));
    m_mapTableAndDepartment.insert(std::make_pair("云丰",     ui->table_yunfeng));
    m_mapTableAndDepartment.insert(std::make_pair("刚需",     ui->table_gangxu));
    m_mapTableAndDepartment.insert(std::make_pair("上九",     ui->table_shangjiu));
    m_mapTableAndDepartment.insert(std::make_pair("中正",     ui->table_zhongzheng));
    m_mapTableAndDepartment.insert(std::make_pair("恒东瀛",   ui->table_hengdongying));
}

void MainWindow::Init_labelDate(){
    // 获取当前的日期，并显示到 label
    m_dateLabel = QDate::currentDate();

    QFont font;
    font.setPointSize(22);
    font.setFamily("Source Code Pro");
    ui->label_Date->setFont(font);
    ui->label_Date->setText(m_dateLabel.toString("yyyy年MM月"));
}

void MainWindow::Init_tableStyle(QTableWidget* tableWidget){
    // 表头style设置
    QHeaderView *header = tableWidget->horizontalHeader();
                                                                // 获取表头对象
    header->setFixedHeight(80);                                 // 设置表头行高
    header->setHighlightSections(false);                        // 关闭表格被选中时加粗高亮
    header->setFont(QFont("Courier New", 15));
    header->setStyleSheet("QHeaderView::section { "             // 设置表头背景色
                          "background-color: lightyellow; "
                          "color: black; "
                          "border: 1px outset #1E90FF;"
                          "}"
                          );

    // 表格style设置
//    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
                                                                // 设置整行选中
    tableWidget->setAlternatingRowColors(true);                 // 开启交替背景色
    tableWidget->setFont(QFont("宋体", 14));                  // 设置字体
    tableWidget->verticalHeader()->setDefaultSectionSize(35);
                                                                // 设置默认行高
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
                                                                // 不允许编辑
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
                                                                // 禁用自动调整列宽
//    tableWidget->setFocusPolicy(Qt::NoFocus);                   // 去除选中虚线框
}

void MainWindow::Init_tableFormat(QTableWidget* tableWidget){
    tableWidget->clearContents();
    tableWidget->setRowCount(0);
    tableWidget->setColumnCount(0);

    int nColumn = 0;

    // 重置日期到1号
    QDate date = QDate(m_dateLabel.year(), m_dateLabel.month(), 1);
    // 获取当前月份的天数
    int daysInMonth = date.daysInMonth();
    // 设置列数和列宽（0和1）
    tableWidget->setColumnCount(daysInMonth+12);
    tableWidget->setColumnWidth(nColumn++, 70);
    tableWidget->setColumnWidth(nColumn++, 60);
    // 设置表头数据
    QStringList strList;
    strList << "姓名" << "时间段";
    for(int i = 1; i <= daysInMonth; i++){
        QString strTemp = SelectWeek(date);
        strTemp.append(QString::number(i));
        strList << strTemp;
        tableWidget->setColumnWidth(i+1, 40);

        nColumn++;
        date = date.addDays(1);
    }

    tableWidget->setColumnWidth(nColumn++, 50);
    tableWidget->setColumnWidth(nColumn++, 50);
    tableWidget->setColumnWidth(nColumn++, 50);
    tableWidget->setColumnWidth(nColumn++, 50);
    tableWidget->setColumnWidth(nColumn++, 25);
    tableWidget->setColumnWidth(nColumn++, 50);
    tableWidget->setColumnWidth(nColumn++, 50);
    tableWidget->setColumnWidth(nColumn++, 25);
    tableWidget->setColumnWidth(nColumn++, 50);
    tableWidget->setColumnWidth(nColumn++, 50);
    strList << "应\n出\n勤" << "实际\n出勤" << "年\n假" << "事\n假"
            << "病\n假"  << "旷\n工" << "迟\n到" << "早\n退"
            << "签\n到\n分" << "全\n勤\n分";
    tableWidget->setHorizontalHeaderLabels(strList);
}

void MainWindow::Init_tableFormat_Name(QString strDepartment, QTableWidget* tableWidget){
    // 格式化table中的名字
    tableWidget->setSortingEnabled(false);

    // select name from information_kaoqin where department="department";
    QSqlQuery* query = new QSqlQuery;
    QString strSqlExec =
        "select name from information_kaoqin where department=\"" +
        strDepartment +
        "\";";
    // 判断执行是否成功
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]select name SQL exec error");
        return ;
    }
    // 判断结果是否为空
    if(!query->next()){
        OutputDebug("[sql.next]select name error");
        return ;
    }

    // 将name数据写入表格
    int startRow = tableWidget->rowCount();            // 获取当前总行数
    int startColumn = 0;
    int rowSpan = 2;
    int columnSpan = 1;
    int nDaysInMonth = m_dateLabel.daysInMonth();
    do{
        // 插入数据
        tableWidget->insertRow(startRow);
        tableWidget->setItem(startRow, 0, new QTableWidgetItem(query->value(0).toString()));
        tableWidget->insertRow(startRow+1);
        tableWidget->setItem(startRow+1, 0, new QTableWidgetItem(query->value(0).toString()));
        // 合并单元格
        tableWidget->setSpan(startRow, startColumn, rowSpan, columnSpan);   // name
        tableWidget->setSpan(startRow, startColumn+nDaysInMonth+2, rowSpan, columnSpan);   // 应出勤
        tableWidget->setSpan(startRow, startColumn+nDaysInMonth+3, rowSpan, columnSpan);   // 实际出勤
        tableWidget->setSpan(startRow, startColumn+nDaysInMonth+4, rowSpan, columnSpan);   // 年假
        tableWidget->setSpan(startRow, startColumn+nDaysInMonth+5, rowSpan, columnSpan);   // 事假
        tableWidget->setSpan(startRow, startColumn+nDaysInMonth+6, rowSpan, columnSpan);   // 病假
        tableWidget->setSpan(startRow, startColumn+nDaysInMonth+7, rowSpan, columnSpan);   // 旷工
        tableWidget->setSpan(startRow, startColumn+nDaysInMonth+8, rowSpan, columnSpan);   // 迟到
        tableWidget->setSpan(startRow, startColumn+nDaysInMonth+9, rowSpan, columnSpan);   // 早退
        tableWidget->setSpan(startRow, startColumn+nDaysInMonth+10, rowSpan, columnSpan);  // 签到分
        tableWidget->setSpan(startRow, startColumn+nDaysInMonth+11, rowSpan, columnSpan);  // 全勤分

        startRow += 2;
    }while(query->next());

    // 将时间段数据写入表格
    for(int i = 0; i < tableWidget->rowCount(); i+=2){
        tableWidget->setItem(i, 1, new QTableWidgetItem("上午"));
        tableWidget->setItem(i+1, 1, new QTableWidgetItem("下午"));
    }

    // 填充空白数据（为了高亮显示功能）
    for(int row = 0; row < tableWidget->rowCount(); row++){
        for(int column = 0; column < tableWidget->columnCount(); column++){
            QTableWidgetItem *item = tableWidget->item(row, column);
            if (!item) {
                // 没数据，填充数据
                QTableWidgetItem *newItem = new QTableWidgetItem();
                tableWidget->setItem(row, column, newItem); // 恢复默认背景色
            }
        }
    }

    // 开启自动排序
//    tableWidget->setSortingEnabled(true);

    // 设置所有单元格内容居中显示
    for(int row = 0; row < tableWidget->rowCount(); ++row) {
        for(int column = 0; column < tableWidget->columnCount(); ++column) {
            QTableWidgetItem *item = tableWidget->item(row, column);
            if(item) {
                item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);    // 水平垂直居中
            }
        }
    }

    delete query;
    return ;
}

QString MainWindow::SelectWeek(QDate date){
    QString strWeek;
    // 该日期是一周中的第几天
    int nDayOfWeek = date.dayOfWeek();

    switch (nDayOfWeek) {
    case Qt::Monday:
        strWeek.append("一\n");
        break;
    case Qt::Tuesday:
        strWeek.append("二\n");
        break;
    case Qt::Wednesday:
        strWeek.append("三\n");
        break;
    case Qt::Thursday:
        strWeek.append("四\n");
        break;
    case Qt::Friday:
        strWeek.append("五\n");
        break;
    case Qt::Saturday:
        strWeek.append("六\n");
        break;
    case Qt::Sunday:
        strWeek.append("日\n");
        break;
    default:
        strWeek.clear();
        break;
    }

    return strWeek;
}

void MainWindow::connectSlots(){
    // 单击高亮整行整列 槽函数连接
    for(auto it : m_mapTableAndDepartment){
        connect(it.second, &QTableWidget::cellClicked, this, &MainWindow::highlightRowAndColumn);
    }
}

void MainWindow::highlightRowAndColumn(int nRow, int nColumn){
    // 高亮整行整列
    // 获取当前tab标签
    int nTabIndex = ui->tabWidget->currentIndex();
    QString strTabName = ui->tabWidget->tabText(nTabIndex);
    auto it = m_mapTableAndDepartment.find(strTabName);
    if(it == m_mapTableAndDepartment.end()){
        // 未找到匹配项
        return ;
    }

    QColor color(216,176,255);

    // 清除高亮
    for (int i = 0; i < it->second->rowCount(); ++i) {
        for (int j = 0; j < it->second->columnCount(); ++j) {
            QTableWidgetItem *item = it->second->item(i, j);
            if (item) {
                item->setBackground(QBrush()); // 恢复默认背景色
            }
        }
    }

    // 如果是首列，高亮两行数据
    if(nColumn == 0){
        for(int i = 0; i < it->second->columnCount(); i++){
            QTableWidgetItem* item = it->second->item(nRow, i);
            if(item){
                item->setBackground(color);
            }
            item = it->second->item(nRow+1, i);
            if(item){
                item->setBackground(color);
            }
        }
    }
    else{
        // 高亮当前行
        for(int i = 0; i < it->second->columnCount(); i++){
            QTableWidgetItem* item = it->second->item(nRow, i);
            if(item){
                item->setBackground(color);

                // 如果是奇数行，额外高亮 (nRow-1, 0)
                if(nRow % 2 != 0){
                    it->second->item(nRow-1, 0)->setBackground(color);
                }
            }
        }

        // 高亮当前列
        for(int i = 0; i < it->second->rowCount(); i++){
            QTableWidgetItem* item = it->second->item(i, nColumn);
            if(item){
                item->setBackground(color);
            }
        }
    }
}

void MainWindow::on_btn_Pre_clicked()
{
    // 修改label的年份
    m_dateLabel = m_dateLabel.addMonths(-1);
    ui->label_Date->setText(m_dateLabel.toString("yyyy年MM月"));

    // 更新表格的日期
    int nCurrentIndex = ui->tabWidget->currentIndex();              // 获取当前的tab标签
    QString strCurrentTab = ui->tabWidget->tabText(nCurrentIndex);
    auto it = m_mapTableAndDepartment.find  (strCurrentTab);          // 查找map
    if(it != m_mapTableAndDepartment.end()){
        // 找到匹配项，初始化表头（日期）
        Init_tableFormat(it->second);
        Init_tableFormat_Name(it->first, it->second);
    }

    // 判断是否需要初始化应出勤天数
    InitDays();

    // 刷新界面
    ShowInfo_Update();
}


void MainWindow::on_btn_Next_clicked()
{
    // 修改label的年份
    m_dateLabel = m_dateLabel.addMonths(1);
    ui->label_Date->setText(m_dateLabel.toString("yyyy年MM月"));

    // 更新表格的日期
    int nCurrentIndex = ui->tabWidget->currentIndex();              // 获取当前的tab标签
    QString strCurrentTab = ui->tabWidget->tabText(nCurrentIndex);
    auto it = m_mapTableAndDepartment.find(strCurrentTab);          // 查找map
    if(it != m_mapTableAndDepartment.end()){
        // 找到匹配项，初始化表头（日期）
        Init_tableFormat(it->second);
        Init_tableFormat_Name(it->first, it->second);
    }

    // 判断是否需要初始化应出勤天数
    InitDays();

    // 刷新界面
    ShowInfo_Update();
}


void MainWindow::on_btn_PreYear_clicked()
{
    // 修改label的年份
    m_dateLabel = m_dateLabel.addYears(-1);
    ui->label_Date->setText(m_dateLabel.toString("yyyy年MM月"));

    // 更新表格的日期
    int nCurrentIndex = ui->tabWidget->currentIndex();              // 获取当前的tab标签
    QString strCurrentTab = ui->tabWidget->tabText(nCurrentIndex);
    auto it = m_mapTableAndDepartment.find(strCurrentTab);          // 查找map
    if(it != m_mapTableAndDepartment.end()){
        // 找到匹配项，初始化表头（日期）
        Init_tableFormat(it->second);
        Init_tableFormat_Name(it->first, it->second);
    }

    // 判断是否需要初始化应出勤天数
    InitDays();

    // 刷新界面
    ShowInfo_Update();
}


void MainWindow::on_btn_NextYear_clicked()
{
    // 修改label的年份
    m_dateLabel = m_dateLabel.addYears(1);
    ui->label_Date->setText(m_dateLabel.toString("yyyy年MM月"));

    // 更新表格的日期
    int nCurrentIndex = ui->tabWidget->currentIndex();              // 获取当前的tab标签
    QString strCurrentTab = ui->tabWidget->tabText(nCurrentIndex);
    auto it = m_mapTableAndDepartment.find(strCurrentTab);          // 查找map
    if(it != m_mapTableAndDepartment.end()){
        // 找到匹配项，初始化表头（日期）
        Init_tableFormat(it->second);
        Init_tableFormat_Name(it->first, it->second);
    }

    // 判断是否需要初始化应出勤天数
    InitDays();

    // 刷新界面
    ShowInfo_Update();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    // 获取当前的tab标签
    QString strCurrentTabName = ui->tabWidget->tabText(index);
    auto it = m_mapTableAndDepartment.find(strCurrentTabName);
    if(it == m_mapTableAndDepartment.end()){
        OutputDebug("map find error");
        return ;
    }
    // 更新表
    Init_tableFormat(it->second);
    Init_tableFormat_Name(it->first, it->second);

    // 刷新界面
    ShowInfo_Update();
}

/* 搜索数据 */
void MainWindow::OpenSearchDialog(){
    // 初始化对话框，接收对话框信号，连接槽函数
    m_searchDialog = new Dialog_search(m_db, this);
    connect(m_searchDialog, SIGNAL(sendAddData(QString)), this, SLOT(receiveSearchData(QString)));
    m_searchDialog->open();

    return ;
}
void MainWindow::receiveSearchData(QString str){
    OutputDebug(str);
    // (id, name, department)
    QStringList strList = str.split("-");
    // 切换该部门(获取到index)
    auto it = m_mapTableAndDepartment.find(strList[2]);
    if(it == m_mapTableAndDepartment.end()){
        QMessageBox::information(this, "error", "无对应数据项");
        return ;
    }

    // 获取tablewidget在tabwidget中的索引值
    int nIndex = -1;
    QWidget* parent = it->second->parentWidget();               // tab_yunfeng <- 获取当前 tablewidget 的父级
    for(int i = 0; i < ui->tabWidget->count(); i++){
        QWidget* widget = ui->tabWidget->widget(i);             // 遍历 tabwidget -> tab_*
        if(widget == parent){
            nIndex = i;
            break;
        }
    }
    if(nIndex == -1){
        return ;
    }

    // 切换到该 tablewidget
    ui->tabWidget->setCurrentIndex(nIndex);

    // 在table中找该name，获取行数
    for(int nRow = 0; nRow < it->second->rowCount(); nRow += 2){
        if(strList[1] == it->second->item(nRow, 0)->text()){
            // 定位到该行
            it->second->scrollToItem(it->second->item(nRow, 0));
            it->second->setCurrentCell(nRow, 0);
            highlightRowAndColumn(nRow, 0);
            break;
        }
    }

    m_searchDialog->close();
//    delete m_searchDialog;

    return ;
}

/* delete 快捷键删除数据 */
void MainWindow::onDeleteData(){
    // 获取当前的选中项
    int nCurrentIndex = ui->tabWidget->currentIndex();
    QString strCurrentName = ui->tabWidget->tabText(nCurrentIndex);
    auto it = m_mapTableAndDepartment.find(strCurrentName);
    if(it == m_mapTableAndDepartment.end()){
        // 没找到
        OutputDebug("[map.find]tablewidget not find");
        return ;
    }

    int nRow = it->second->currentRow();
    int nCol = it->second->currentColumn();
    // 不在表格内delete直接退出
    if(!IsCanChanged(nRow, nCol)){
        return ;
    }

    bool bIsodd = false;
    if(nRow % 2 == 0){
        // 上午
        bIsodd = true;
    }
    else{
        // 下午
        bIsodd = false;
    }

    // 获取id
    QString strTemp = strCurrentName + "," +
                      it->second->item(nRow, 0)->text() + "," +
                      QDate(m_dateLabel.year(), m_dateLabel.month(), 1).toString("yyyy/MM/dd");
    int nId = GetId(strTemp);
    if(nId == -1){
        return ;
    }

    // 构造sql语句
    QSqlQuery* query = new QSqlQuery;
    QString strSqlExec;
    // 判断是否有数据，如果没数据直接退出
    //select * from data_status where id=14 and date_day=6;
    strSqlExec = "select * from data_status where id=" +
                 QString::number(nId) +
                 " and date_day=" +
                 QString::number(nCol-1) + ";";
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]" + strSqlExec);
        return ;
    }
    if(!query->next()){
        OutputDebug("[sql.next]没有数据项");
        return ;
    }

    // update data_status set status_2=NULL where id=159 and date_day=6;
    strSqlExec.clear();
    if(bIsodd){
        strSqlExec = "update data_status set status_1=NULL where id=" +
                     QString::number(nId) +
                     " and date_day=" +
                     QString::number(nCol-1) + ";";
    }
    else{
        strSqlExec = "update data_status set status_2=NULL where id=" +
                     QString::number(nId) +
                     " and date_day=" +
                     QString::number(nCol-1) + ";";
    }
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]" + strSqlExec);
        return ;
    }

    // 检查状态是否都被删除，如果都为 NULL 将该条记录删除
    //select status_1, status_2 from data_status where id=37 and date_day=6;
    strSqlExec.clear();
    strSqlExec = "select status_1, status_2 from data_status where id=" +
                 QString::number(nId) +
                 " and date_day=" +
                 QString::number(nCol-1) + ";";
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]" + strSqlExec);
        return ;
    }
    if(!query->next()){
        return ;
    }
    if(query->value(0).toString() == nullptr && query->value(1).toString() == nullptr){
        // 为空需要进行删除
        //delete from data_status where id=159 and date_day=6;
        strSqlExec.clear();
        strSqlExec = "delete from data_status where id=" +
                     QString::number(nId) +
                     " and date_day=" +
                     QString::number(nCol-1) + ";";
        if(!query->exec(strSqlExec)){
            OutputDebug("[sql.exec]" + strSqlExec);
            return ;
        }
    }

//    QMessageBox::information(this, "tip", "success");

    // 刷新界面
    if(nRow % 2 == 0){
        // 上午
        UpdateShowInfo_Row(nRow);
    }
    else{
        // 下午
        UpdateShowInfo_Row(nRow-1);
    }


    delete query;
    return ;
}

/* 初始化应出勤天数 */
void MainWindow::InitDays(){
    // 判断是否需要初始化(从数据库中读取数据，对比当前日期是否存在)
    // select days from initDays where date_month="2024/5/1";
    QDate date = QDate(m_dateLabel.year(), m_dateLabel.month(), 1);
    QString strSqlExec = "select days from initDays where date_month=\"" +
                        date.toString("yyyy/MM/dd") +
                         "\";";
    QSqlQuery* query = new QSqlQuery;

    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]select init days error");
        return ;
    }
    if(query->next()){
        // 不为空，不需要初始化
        m_nDaysOfMonth = query->value(0).toInt();
        return ;
    }

    // 为空，初始化对话框，添加应出勤天数
    m_InitDaysDialog = new Dialog_InitDays(date, this);
    connect(m_InitDaysDialog, SIGNAL(sendAddData(int)), this, SLOT(receiveInitDaysData(int)));
    m_InitDaysDialog->open();

    delete query;
    return ;
}
void MainWindow::receiveInitDaysData(int days){
    // 获取到输入的应出勤天数
    m_nDaysOfMonth = days;

    // insert into initDays(date_month, days) VALUES("2024/5/1", 21);
    QSqlQuery* query = new QSqlQuery;
    QString strSqlExec = "insert into initDays(date_month, days) VALUES(\"" +
                        QDate(m_dateLabel.year(), m_dateLabel.month(), 1).toString("yyyy/MM/dd") +
                        "\", " +
                        QString::number(days) +
                        ");";

    // 将数据添加到数据库
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]insert init days error");
        return ;
    }

    // 添加该月份所有成员的初始化数据
    InitDatakaoqin();

    // 关闭窗口，资源回收
    m_InitDaysDialog->close();
    delete m_InitDaysDialog;
}

/* 初始化所有成员的考勤信息 */
void MainWindow::InitDatakaoqin(){
    QSqlQuery* query = new QSqlQuery;
    QString strSqlExec = "select * from deleteid;";
    // 获取已经被删除的id号
    QList<int> listDeleteId;
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]select deleteid error");
        return ;
    }
    if(!query->next()){
        OutputDebug("[sql.next]deleteid is empty");
    }
    else{
        // 不为空
        do{
            listDeleteId.append(query->value(0).toInt());
        }while(query->next());
    }

    // 遍历所有的用户id
    //select id from information_kaoqin;
    strSqlExec.clear();
    strSqlExec = "select id from information_kaoqin;";
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]select id error");
        return ;
    }
    if(!query->next()){
        OutputDebug("[sql.next]information_kaoqin is empty");
        return ;
    }

    do{
        // 获取id
        int nId = query->value(0).toInt();
        QSqlQuery* queryTemp = new QSqlQuery;
        // 添加该成员的数据项
        //insert into data_kaoqin(id_data, id_info, date_month, days) VALUES(deleteId, id,"2024/4/1",m_nDaysOfMonth);
        //insert into data_kaoqin(id_info, date_month, days) VALUES(id,"2024/4/1",m_nDaysOfMonth);
        strSqlExec.clear();
        if(listDeleteId.isEmpty()){
            strSqlExec = "insert into data_kaoqin(id_info, date_month, days) VALUES(" +
                         QString::number(nId) +
                         ",\"" +
                         QDate(m_dateLabel.year(), m_dateLabel.month(), 1).toString("yyyy/MM/dd") +
                         "\"," +
                         QString::number(m_nDaysOfMonth) +
                         ");";
        }
        else{
            // 删除deleteId表中的数据
            //delete from deleteid where id=listDeleteId.first();
            strSqlExec = "delete from deleteid where id=" +
                         QString::number(listDeleteId.first()) + ";";
            if(!queryTemp->exec(strSqlExec)){
                OutputDebug("[sql.exec]" + strSqlExec);
                return ;
            }

            strSqlExec.clear();
            strSqlExec = "insert into data_kaoqin(id_data, id_info, date_month, days) VALUES(" +
                         QString::number(listDeleteId.takeFirst()) +                // 获取首项数据，并进行删除
                         "," +
                         QString::number(nId) +
                         ",\"" +
                         QDate(m_dateLabel.year(), m_dateLabel.month(), 1).toString("yyyy/MM/dd") +
                         "\"," +
                         QString::number(m_nDaysOfMonth) +
                         ");";
        }
        if(!queryTemp->exec(strSqlExec)){
            OutputDebug("[sql.exec]insert data_kaoqin(id_info, date_month, days) error");
            return ;
        }

        delete queryTemp;
    }while(query->next());

    delete query;
    return ;
}

/* 添加考勤数据，双击事件响应 */
void MainWindow::onDoubleClickEvent(int row, int col){
    // 判断是否在可编辑区域
    if(!IsCanChanged(row, col)){
        return ;
    }

    // 获取当前数据：(department, name, date_year_month, date_day, statu)
    QString strData;

    // 判断当前行是否是奇数行(设计问题，被迫使用全局标志)
    if(row % 2 == 0){
        // 偶数行:上午
        m_bIsodd = false;
    }
    else{
        // 奇数行:下午
        m_bIsodd = true;
    }

    int nTableIndex = ui->tabWidget->currentIndex();
    QString strDepartment = ui->tabWidget->tabText(nTableIndex);
    auto it = m_mapTableAndDepartment.find(strDepartment);
    if(it == m_mapTableAndDepartment.end()){
        return ;
    }

    strData = strDepartment +
            "," +
            it->second->item(row, 0)->text() +
            "," +
            QDate(m_dateLabel.year(), m_dateLabel.month(), 1).toString("yyyy/MM/dd") +
            "," +
            QString::number(col-1) +
            ",";

    // dialog.open 获取考勤情况
    m_dialogStatus = new Dialog_Status(strData);
    connect(m_dialogStatus, SIGNAL(sendAddData(QString)), this, SLOT(receiveStatusData(QString)));
    m_dialogStatus->open();

    return ;
}
void MainWindow::receiveStatusData(QString strStatu){
    //strData = department, name, date_year_month, date_day, statu
    QStringList strList = strStatu.split(",");
    QSqlQuery* query = new QSqlQuery;
    QString strSqlExec;

    // 获取 id
    int nId = GetId(strStatu);
    if(nId == -1){
        OutputDebug_Box("该成员不允许被插入数据");
        m_dialogStatus->close();
        delete m_dialogStatus;
        delete query;
        return ;
    }

    // 插入数据(要先检查是否有数据，有数据就update，没有就insert)
    // select * from data_status where id=1 and date_day=12;
    strSqlExec = "select * from data_status where id=" +
                 QString::number(nId) +
                 " and date_day=" +
                 strList[3] +
                 ";";
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]" + strSqlExec);
        m_dialogStatus->close();
        delete m_dialogStatus;
        delete query;
        return ;
    }
    strSqlExec.clear();
    if(!query->next()){
        // 没有数据 insert
        //insert into data_status(id, date_day, status_1) VALUES(1, 1, "√");
        if(!m_bIsodd){
            // 上午
            strSqlExec = "insert into data_status(id, date_day, status_1) VALUES(" +
                         QString::number(nId) + ", " +
                         strList[3] + ", \"" +
                         strList[4] + "\");";
        }
        else{
            // 下午
            strSqlExec = "insert into data_status(id, date_day, status_2) VALUES(" +
                         QString::number(nId) + ", " +
                         strList[3] + ", \"" +
                         strList[4] + "\");";
        }
    }
    else{
        // update data_status set status_1="√" where id=1 and date_day=12;
        if(!m_bIsodd){
            // 上午
            strSqlExec = "update data_status set status_1=\"" +
                         strList[4] +
                         "\" where id=" +
                         QString::number(nId) +
                         " and date_day=" +
                         strList[3] +
                         ";";
        }
        else{
            // 下午
            strSqlExec = "update data_status set status_2=\"" +
                         strList[4] +
                         "\" where id=" +
                         QString::number(nId) +
                         " and date_day=" +
                         strList[3] +
                         ";";
        }
    }

    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]" + strSqlExec);
        m_dialogStatus->close();
        return ;
    }


    // 刷新界面(获取行数)
    int nIndex = ui->tabWidget->currentIndex();
    QString strDepartment = ui->tabWidget->tabText(nIndex);
    auto it = m_mapTableAndDepartment.find(strDepartment);
    UpdateShowInfo_Row(it->second->currentRow());


    // 释放资源，关闭选择状态的dialog
    m_dialogStatus->close();
    delete m_dialogStatus;
    delete query;
    return ;
}
int MainWindow::GetId(QString strData){
    //strData = (department,name,date_year_month)

    QStringList strList = strData.split(",");
    QSqlQuery* query = new QSqlQuery;
    QString strSqlExec;

    // 获取 id
    //select id from information_kaoqin where name="乔泽华" and department="中健";
    strSqlExec = "select id from information_kaoqin where name=\"" +
                 strList[1] +
                 "\" and department=\"" +
                 strList[0] +
                 "\";";
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]select id for name and department");
        return -1;
    }
    if(!query->next()){
        OutputDebug("[sql.next]找不到此人的id");
        return -1;
    }
    int nId = query->value(0).toInt();

    // 获取 id from data_kaoqin
    //select id_data from data_kaoqin where id_info=nId and date_month="2024/04/01";
    strSqlExec.clear();
    strSqlExec = "select id_data from data_kaoqin where id_info=" +
                 QString::number(nId) +
                 " and date_month=\"" +
                 strList[2] +
                 "\";";
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]select id for name and department");
        return -1;
    }
    if(!query->next()){
        OutputDebug("[sql.next]找不到此人的id_data");
        return -1;
    }
    int nId_data = query->value(0).toInt();

    return nId_data;
}

/* 显示数据到界面 */
void MainWindow::ShowInfo(){
    // department
    int nTemp = ui->tabWidget->currentIndex();
    QString strDepartment = ui->tabWidget->tabText(nTemp);
    auto it = m_mapTableAndDepartment.find(strDepartment);
    if(it == m_mapTableAndDepartment.end()){
        return ;
    }

    // date daysInMonth
    QDate date = QDate(m_dateLabel.year(), m_dateLabel.month(), 1);
    int ndaysInMonth = date.daysInMonth();
    // name row col
    int nCol = 0;
    int nRow = 0;
    for(; nRow < it->second->rowCount(); nRow+=2){
        QString strName = it->second->item(nRow, 0)->text();
        QString strInParam = strDepartment + "," + strName + "," + date.toString("yyyy/MM/dd");
        // status
        int nId = GetId(strInParam);
        if(nId == -1){
            continue;
        }
        for(nCol = 2; nCol < ndaysInMonth+2; nCol++){
            QString strResult = GetStatus_day(nId, nCol-1);
            if(strResult == nullptr)
                continue;
            QStringList listStatus = strResult.split(",");

            // 设置数据
            it->second->item(nRow, nCol)->setText(listStatus[0]);
            it->second->item(nRow+1, nCol)->setText(listStatus[1]);
        }

        // 应出勤天数
        it->second->item(nRow, nCol++)->setText(QString::number(m_nDaysOfMonth));
        // 实际出勤天数
        float fTemp = 0;
        fTemp = (CalcStatus(nId, "√", STATUS_TIME::ALLDAY) +
                 CalcStatus(nId, "Y", STATUS_TIME::ALLDAY)) * 0.5 +
                (CalcStatus(nId, "●", STATUS_TIME::ALLDAY)) * 0.5 +
                (CalcStatus(nId, "※", STATUS_TIME::ALLDAY) +
                 CalcStatus(nId, "T", STATUS_TIME::ALLDAY)) * 0.5;
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 年假
        fTemp = CalcStatus(nId, "●", STATUS_TIME::ALLDAY) * 0.5;
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 事假
        fTemp = CalcStatus(nId, "○", STATUS_TIME::ALLDAY) * 0.5;
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 病假
        it->second->item(nRow, nCol++)->setText(QString::number(0));
        // 旷工
        fTemp = CalcStatus(nId, "×", STATUS_TIME::ALLDAY) * 0.5;
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 迟到
        fTemp = CalcStatus(nId, "※", STATUS_TIME::ALLDAY) +
                CalcStatus(nId, "T", STATUS_TIME::ALLDAY);
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 早退
        it->second->item(nRow, nCol++)->setText(QString::number(0));
        // 签到分
        fTemp = CalcStatus(nId, "√", STATUS_TIME::ALLDAY);
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 全勤分
        fTemp = CalcStatus(nId, "√", STATUS_TIME::ALLDAY) +
                CalcStatus(nId, "●", STATUS_TIME::ALLDAY) +
                CalcStatus(nId, "Y", STATUS_TIME::PM);
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
    }

    return ;
}
void MainWindow::ShowInfo_Update(){
    // department
    int nTemp = ui->tabWidget->currentIndex();
    QString strDepartment = ui->tabWidget->tabText(nTemp);
    auto it = m_mapTableAndDepartment.find(strDepartment);
    if(it == m_mapTableAndDepartment.end()){
        return ;
    }

    // date daysInMonth
    QDate date = QDate(m_dateLabel.year(), m_dateLabel.month(), 1);
    int ndaysInMonth = date.daysInMonth();
    // name row
    int nRow = 0;
    for(; nRow < it->second->rowCount(); nRow += 2){
        QString strInParam = strDepartment + "," + it->second->item(nRow, 0)->text() + "," + date.toString("yyyy/MM/dd");
        // 获取 status
        int nId = GetId(strInParam);
        if(nId == -1){
            continue;
        }
        QString strTemp = GetStatus_month(nId);
        if(strTemp == nullptr){
            continue;
        }
        QStringList strListAll = strTemp.split(",");
        for(int nIndex = 0; nIndex < strListAll.count(); nIndex++){
            // day-status_1-status_2
            QStringList strListStatus = strListAll[nIndex].split("-");
            // nCol = day+2
            it->second->item(nRow, strListStatus[0].toInt()+1)->setText(strListStatus[1]);
            it->second->item(nRow+1, strListStatus[0].toInt()+1)->setText(strListStatus[2]);
        }

        int nCol = ndaysInMonth + 2;
        // 应出勤天数
        it->second->item(nRow, nCol++)->setText(QString::number(m_nDaysOfMonth));
        // 实际出勤天数
        float fTemp = 0;
        fTemp = (CalcStatus(nId, "√", STATUS_TIME::ALLDAY) +
                 CalcStatus(nId, "Y", STATUS_TIME::ALLDAY)) * 0.5 +
                (CalcStatus(nId, "●", STATUS_TIME::ALLDAY)) * 0.5 +
                (CalcStatus(nId, "※", STATUS_TIME::ALLDAY) +
                 CalcStatus(nId, "T", STATUS_TIME::ALLDAY)) * 0.5;
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 年假
        fTemp = CalcStatus(nId, "●", STATUS_TIME::ALLDAY) * 0.5;
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 事假
        fTemp = CalcStatus(nId, "○", STATUS_TIME::ALLDAY) * 0.5;
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 病假
        it->second->item(nRow, nCol++)->setText(QString::number(0));
        // 旷工
        fTemp = CalcStatus(nId, "×", STATUS_TIME::ALLDAY) * 0.5;
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 迟到
        fTemp = CalcStatus(nId, "※", STATUS_TIME::ALLDAY) +
                CalcStatus(nId, "T", STATUS_TIME::ALLDAY);
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 早退
        it->second->item(nRow, nCol++)->setText(QString::number(0));
        // 签到分
        fTemp = CalcStatus(nId, "√", STATUS_TIME::ALLDAY);
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
        // 全勤分
        fTemp = CalcStatus(nId, "√", STATUS_TIME::ALLDAY) +
                CalcStatus(nId, "●", STATUS_TIME::ALLDAY) +
                CalcStatus(nId, "Y", STATUS_TIME::PM);
        it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
    }
}
void MainWindow::UpdateShowInfo_Row(int nRow){
    if(nRow % 2 != 0){
        nRow = nRow-1;
    }

    int nTemp = ui->tabWidget->currentIndex();
    QString strDepartment = ui->tabWidget->tabText(nTemp);
    auto it = m_mapTableAndDepartment.find(strDepartment);
    if(it == m_mapTableAndDepartment.end()){
        return ;
    }
    // date daysInMonth
    QDate date = QDate(m_dateLabel.year(), m_dateLabel.month(), 1);
    int ndaysInMonth = date.daysInMonth();
    // name
    QString strName = it->second->item(nRow, 0)->text();
    QString strInParam = strDepartment + "," + strName + "," + date.toString("yyyy/MM/dd");
    int nId = GetId(strInParam);
    if(nId == -1){
        return ;
    }

    // 循环获取状态
    int nCol = 2;
    for(; nCol < m_dateLabel.daysInMonth()+2; nCol++){
        QString strResult = GetStatus_day(nId, nCol-1);
        if(strResult == nullptr){
            it->second->item(nRow, nCol)->setText(nullptr);
            it->second->item(nRow+1, nCol)->setText(nullptr);
        }
        else{
            QStringList listStatus = strResult.split(",");

            // 设置数据
            it->second->item(nRow, nCol)->setText(listStatus[0]);
            it->second->item(nRow+1, nCol)->setText(listStatus[1]);
        }
    }

    // 应出勤天数
    it->second->item(nRow, nCol++)->setText(QString::number(m_nDaysOfMonth));
    // 实际出勤天数
    float fTemp = 0;
    fTemp = (CalcStatus(nId, "√", STATUS_TIME::ALLDAY) +
             CalcStatus(nId, "Y", STATUS_TIME::ALLDAY)) * 0.5 +
            (CalcStatus(nId, "●", STATUS_TIME::ALLDAY)) * 0.5 +
            (CalcStatus(nId, "※", STATUS_TIME::ALLDAY) +
             CalcStatus(nId, "T", STATUS_TIME::ALLDAY)) * 0.5;
    it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
    // 年假
    fTemp = CalcStatus(nId, "●", STATUS_TIME::ALLDAY) * 0.5;
    it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
    // 事假
    fTemp = CalcStatus(nId, "○", STATUS_TIME::ALLDAY) * 0.5;
    it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
    // 病假
    it->second->item(nRow, nCol++)->setText(QString::number(0));
    // 旷工
    fTemp = CalcStatus(nId, "×", STATUS_TIME::ALLDAY) * 0.5;
    it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
    // 迟到
    fTemp = CalcStatus(nId, "※", STATUS_TIME::ALLDAY) +
            CalcStatus(nId, "T", STATUS_TIME::ALLDAY);
    it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
    // 早退
    it->second->item(nRow, nCol++)->setText(QString::number(0));
    // 签到分
    fTemp = CalcStatus(nId, "√", STATUS_TIME::ALLDAY);
    it->second->item(nRow, nCol++)->setText(QString::number(fTemp));
    // 全勤分
    fTemp = CalcStatus(nId, "√", STATUS_TIME::ALLDAY) +
            CalcStatus(nId, "●", STATUS_TIME::ALLDAY) +
            CalcStatus(nId, "Y", STATUS_TIME::PM);
    it->second->item(nRow, nCol++)->setText(QString::number(fTemp));

    return ;
}
QString MainWindow::GetStatus_day(int nId, int day){
    // 获取上下午状态
    // 获取id
//    int nId = GetId(strParam);

    // select status_1, status_2 from data_status where id=14 and date_day=6;
    QSqlQuery* query = new QSqlQuery;
    QString strSqlExec = "select status_1, status_2 from data_status where id=" +
                         QString::number(nId) + " and date_day=" +
                         QString::number(day) + ";";
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]" + strSqlExec);
        return nullptr;
    }
    if(!query->next()){
        OutputDebug("[sql.next]not find status");
        return nullptr;
    }
    QString strResult = query->value(0).toString() + "," + query->value(1).toString();

    return strResult;
}
QString MainWindow::GetStatus_month(int nId){
    //select group_concat(IFNULL(date_day, ''), '-', IFNULL(status_1, ''), '-', IFNULL(status_2, '')) from data_status where id=37;
    QSqlQuery* query = new QSqlQuery;
    QString strSqlExec = "select group_concat(IFNULL(date_day, ''), '-', IFNULL(status_1, ''), '-', IFNULL(status_2, '')) from data_status where id=" +
                         QString::number(nId) + ";";
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]" + strSqlExec);
        return nullptr;
    }
    if(!query->next()){
        OutputDebug("[sql.next]not find status");
        return nullptr;
    }
    QString strResult;
    strResult.reserve(512);
    strResult.clear();
    strResult = query->value(0).toString();

    return strResult;
}

int MainWindow::CalcStatus(const int &nId, QString strStatus, STATUS_TIME time){
    int nResult = 0;

    //select count(date_day) from data_status where id=? and status_1="?";
    //select count(date_day) from data_status where id=? and status_2="?";
    QSqlQuery* query = new QSqlQuery;
    QString strSqlExec;
    if(time == STATUS_TIME::AM){
        strSqlExec = "select count(date_day) from data_status where id=" +
                     QString::number(nId) +
                     " and status_1=\"" + strStatus + "\"";
    }
    else if(time == STATUS_TIME::PM){
        strSqlExec = "select count(date_day) from data_status where id=" +
                     QString::number(nId) +
                     " and status_2=\"" + strStatus + "\"";
    }
    else if(time == STATUS_TIME::ALLDAY){
        strSqlExec = "select count(date_day) from data_status where id=" +
                     QString::number(nId) +
                     " and status_1=\"" + strStatus + "\"";
        if(!query->exec(strSqlExec)){
            OutputDebug("[sql.exec]" + strSqlExec);
            return 0;
        }
        if(!query->next())
        {
            return 0;
        }
        nResult = query->value(0).toInt();

        strSqlExec = "select count(date_day) from data_status where id=" +
                     QString::number(nId) +
                     " and status_2=\"" + strStatus + "\"";
    }
    if(!query->exec(strSqlExec)){
        OutputDebug("[sql.exec]" + strSqlExec);
        return 0;
    }
    if(!query->next())
    {
        return 0;
    }
    nResult += query->value(0).toInt();

    return nResult;
}

/* 判断是否可以进行操作 */
bool MainWindow::IsCanChanged(int nRow, int nCol){
    if(nRow == -1 || nCol == -1){
        return false;
    }
    if(nCol < 2 || nCol >= (m_dateLabel.daysInMonth()+2)){
        return false;
    }

    return true;
}

/* 增删员工数据 */
void MainWindow::AddAndDelPersonal(){
    m_dialogPersonalInfo = new Dialog_PersonalInfo(this);
    connect(m_dialogPersonalInfo, SIGNAL(sendAddData(QString)), this, SLOT(receiveAddData(QString)));
    connect(m_dialogPersonalInfo, SIGNAL(sendDelData(QString)), this, SLOT(receiveDelData(QString)));
    m_dialogPersonalInfo->open();

    return ;
}
void MainWindow::receiveAddData(QString str){
    // department, name, date
    QSqlQuery* query = new QSqlQuery();
    QString strSqlExec;
    QStringList listTemp;

    // 获取数据
    QStringList listData = str.split(",");
    QDate date = QDate::fromString(listData[2], "yyyy/MM/dd");
    QString strName = listData[1];
    QString strDepartment = listData[0];

    /* information 增加数据 */
    // insert into information(name, in_date) values('测试1', '2024/4/26');
    strSqlExec = "insert into information(name, in_date) values('" +
                 strName +
                 "', '" +
                 date.toString("yyyy/MM/dd") +
                 "');";
    if(!query->exec(strSqlExec)){
        //        sendDelData("delete");
        OutputDebug_Box("[add personal]insert into information error");
        return ;
    }

    /* information_kaoqin 增加数据 */
    // insert into information_kaoqin(name, department) values('测试1', '云丰');
    strSqlExec.clear();
    strSqlExec = "insert into information_kaoqin(name, department) values('" +
                 strName +
                 "', '" +
                 strDepartment +
                 "');";
    if(!query->exec(strSqlExec)){
        OutputDebug_Box("[add personal]insert into information_kaoqin error");
        return ;
    }
    // 获取 id
    // select id from information_kaoqin where name='?';
    strSqlExec.clear();
    strSqlExec = "select id from information_kaoqin where name='" + strName + "';";
    if(!query->exec(strSqlExec)){
        return ;
    }
    if(!query->next()){
        return ;
    }
    QString strId = query->value(0).toString();

    /* data_kaoqin 增加数据 */
    // initdays 中获取需要初始化的日期 -> ( >= 入职year_month_1 )
    // select * from initDays;
    strSqlExec.clear();
    strSqlExec = "select * from initDays;";
    if(!query->exec(strSqlExec)){
        return ;
    }
    if(!query->next()){
        return ;
    }
    do{
        listTemp.append(query->value(0).toDate().toString("yyyy/MM/dd") + "," + query->value(1).toString());
    }while(query->next());
    // 获取需要增加的日期
    for(auto it = listTemp.begin(); it != listTemp.end(); it++){
        QStringList listStr = it->split(",");
        QDate dateInit = QDate::fromString(listStr[0], "yyyy/MM/dd");
        QDate dateCurrent(date.year(), date.month(), 1);
        if(dateCurrent <= dateInit){
            // deleteid 中获取被删除的id
            // select * from deleteid;
            strSqlExec.clear();
            strSqlExec = "select * from deleteid;";
            if(!query->exec(strSqlExec)){
                return ;
            }
            if(!query->next()){
                // 没有可用 id
                // insert into data_kaoqin(id_info, date_month, days) VALUES(3,'2024/4/1',23);
                strSqlExec.clear();
                strSqlExec = "insert into data_kaoqin(id_info, date_month, days) VALUES(" +
                             strId +
                             ",'" +
                             dateInit.toString("yyyy/MM/dd") +
                             "'," +
                             listStr[1] +
                             ");";
                if(!query->exec(strSqlExec)){
                    OutputDebug_Box("[add personal]insert into data_kaoqin error");
                    return ;
                }
            }
            else{
                // 有可用 id
                // insert into data_kaoqin(id_info, id_data, date_month, days) VALUES(3, 1, "2024/4/1",23);
                int nDeleteId = query->value(0).toInt();
                strSqlExec.clear();
                strSqlExec = "insert into data_kaoqin(id_info, id_data, date_month, days) VALUES(" +
                             strId +
                             "," +
                             QString::number(nDeleteId) +
                             ",'" +
                             dateInit.toString("yyyy/MM/dd") +
                             "'," +
                             listStr[1] +
                             ");";
                if(!query->exec(strSqlExec)){
                    OutputDebug_Box("[add personal]insert into data_kaoqin.deleteid error");
                    return ;
                }

                // 删除 deleteid 中已经被使用的 id
                // delete from deleteid where id=?;
                strSqlExec.clear();
                strSqlExec = "delete from deleteid where id=" + QString::number(nDeleteId) + ";";
                if(!query->exec(strSqlExec)){
                    OutputDebug_Box("[add personal]delete from deleteid="+QString::number(nDeleteId));
                    return ;
                }
            }
        }
    }

    OutputDebug_Box("add success!");
    m_dialogPersonalInfo->close();
    delete m_dialogPersonalInfo;

    return ;
}
void MainWindow::receiveDelData(QString str){
    // department, name, date
    QSqlQuery* query = new QSqlQuery();
    QString strSqlExec;
    QStringList listTemp;

    // 获取数据
    QStringList listData = str.split(",");
    QString strName = listData[1];
    QString strDepartment = listData[0];

    /* information 删除数据 (年假数据不删除了)*/
    // delete from information where name='?';
    strSqlExec.clear();
    strSqlExec = "delete from information where name='" +
                 strName +
                 "';";
    if(!query->exec(strSqlExec))
    {
        OutputDebug("[sql.exec]error: " + strSqlExec);
    }

    /* data_status 删除数据 */
    // data_kaoqin.id_data -> 通过information_kaoqin.id_info获取要删除的id
    // select id from information_kaoqin where name='?' and department='?';
    strSqlExec.clear();
    strSqlExec = "select id from information_kaoqin where name='" +
                 strName +
                 "' and department='" +
                 strDepartment +
                 "';";
    if(!query->exec(strSqlExec)){
        OutputDebug_Box("删除失败");
        return ;
    }
    if(!query->next()){
        OutputDebug_Box("删除失败");
        return ;
    }
    int nId_Info = query->value(0).toInt();

    // select id_data from data_kaoqin where id_info=?;
    QList<int> listDeleteId;
    strSqlExec.clear();
    strSqlExec = "select id_data from data_kaoqin where id_info=" +
                 QString::number(nId_Info) + ";";
    if(!query->exec(strSqlExec)){
        OutputDebug_Box("删除失败");
        return ;
    }
    if(!query->next()){
        OutputDebug_Box("删除失败");
        return ;
    }
    do{
        listDeleteId.append(query->value(0).toInt());
    }while(query->next());

    // 遍历list，删除 data_status 的数据
    // delete from data_status where id=?;
    for(auto it = listDeleteId.begin(); it != listDeleteId.end(); it++){
        strSqlExec.clear();
        strSqlExec = "delete from data_status where id=" + QString::number(*it) + ";";
        if(!query->exec(strSqlExec)){
            continue ;
        }
    }

    /* data_kaoqin 删除数据 */
    // delete from data_kaoqin where id_info=?;
    strSqlExec.clear();
    strSqlExec = "delete from data_kaoqin where id_info=" + QString::number(nId_Info) + ";";
    if(!query->exec(strSqlExec)){
        OutputDebug_Box("删除失败");
        return ;
    }

    /* deleteid 增加数据 */
    // insert into deleteid(id) VALUES(?);
    for(auto it = listDeleteId.begin(); it != listDeleteId.end(); it++){
        strSqlExec.clear();
        strSqlExec = "insert into deleteid(id) VALUES(" + QString::number(*it) + ");";
        if(!query->exec(strSqlExec)){
            continue ;
        }
    }

    /* information_info 删除数据 */
    // delete from information_info where id=?;
    strSqlExec.clear();
    strSqlExec = "delete from information_kaoqin where id=" + QString::number(nId_Info) + ";";
    if(!query->exec(strSqlExec)){
        OutputDebug_Box("删除失败");
        return ;
    }

    OutputDebug_Box("delete success!");
    m_dialogPersonalInfo->close();
    delete m_dialogPersonalInfo;

    return ;
}

/* 导入员工考勤数据 */
void MainWindow::onImportPersonalData(){
    m_dialogImportData = new Dialog_ImportData(this);
    m_dialogImportData->open();

    return ;
}
/*
{
    部门名称
}
{
    id
    name
    部门
}information_kaoqin

{
    id1(外键，关联information_kaoqin.id)
    id2（主键，唯一，自增）
    date_year_month
    应出勤天数
    签到分
    全勤分
}data_kaoqin

{
    id(外键，关联data_kaoqin.id2)
    date_day
    status_1
    status_2
}data_status


*/

/*
1  张三	中健
2  里斯	中健
3  王五	星禾

id name
1   1   2024-04-01	21
2	1	2024-05-01	22
3	2	2024-05-01	23
4	2	2024-06-01	22
5	3	2024-06-01	23

张三 4月份数据
4.1 √ √：
    1 1 √ √
4.2 √ ※
    1 2 √ ※
4.4 Y Y
    1 4 Y Y

里斯 5月份数据
5.1 √ Y
    3 1 √ Y
5.6 √ Y
    3 6 √ Y

维护一张表（删除序号的表）
    被删除之后将序号存入表中
    分配时，如果列表为空，直接默认分配序号
    如果列表不为空，获取序号进行分配
*/


