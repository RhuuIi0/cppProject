create table information_kaoqin(
	id int auto_increment unique primary key,
    name varchar(255) not null,
    department varchar(255) not null,
    foreign key(department) references department(department_name) on update restrict on delete restrict,
    unique(name, department)
);

--- 初始化initDays表时，改表也初始化（所有成员）
create table data_kaoqin(
	id_info int not null,
    id_data int auto_increment unique primary key,
    date_month date not null,
    days int,
    checkin_points int,
    full_points int,
    foreign key(id_info) references information_kaoqin(id) on update restrict on delete restrict,
    foreign key(date_month) references initDays(date_month) on update restrict on delete restrict,
    unique(id_info, date_month)
);

create table data_status(
	id int,
    date_day int not null,
    status_1 varchar(255),
    status_2 varchar(255),
    foreign key(id) references data_kaoqin(id_data) on update restrict on delete restrict,
    unique(id, date_day)
);

-- 维护初始化的表格，保存已初始化的月份的应出勤天数
create table initDays(
	date_month date primary key,
    days int not null
);

-- 保存被删除的id
create table deleteId(id int primary key);
insert into deleteId(id) values(2);

-- 部门
create table department(department_name varchar(255) primary key);

