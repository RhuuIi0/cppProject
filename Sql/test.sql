select * from initDays;
delete  from initDays;

select * from deleteid;
insert into deleteid(id) VALUES(1);
delete from deleteid;

select * from information_kaoqin;

select * from data_kaoqin;
delete  from data_kaoqin;

select * from data_status;

insert into initDays(date_month, days) VALUES("2024/5/1", 21);
select id from information_kaoqin;
insert into data_kaoqin(id_info, date_month, days) VALUES(3,"2024/4/1",23);
insert into data_kaoqin(id_info, id_data, date_month, days) VALUES(3, 1, "2024/4/1",23);

select id_data from data_kaoqin where id_info=1 and date_month="2024/04/01";

-- 检查此日期是否有数据 
select * from data_status where id=14 and date_day=6;
select status_1, status_2 from data_status where id=37 and date_day=27;
-- update数据
update data_status set status_2=NULL where id=159 and date_day=6;
-- delete数据
delete from data_status where id=37 and date_day=25;

-- 事假 查询
select count(date_day) from data_status where id=37 and status_1="○";
select count(date_day) from data_status where id=37 and status_2="○";

-- 批量状态查询(一次性获取一个月的数据)
select group_concat(date_day, "-", status_1, "-", status_2) from data_status where id=37;
SELECT GROUP_CONCAT(IFNULL(date_day, ''), '-', IFNULL(status_1, ''), '-', IFNULL(status_2, '')) 
FROM data_status 
WHERE id=37;

-- 查询department
select * from department;
insert into department(department_name) values("恒东瀛");
    -- ("中健"),("全彩"),("忠达"),("全实快"),("涵泰"),("星禾"),("火眼"),
    -- ("荣康"),("国鑫"),("云丰"),("刚需"),("上九"),("中正"),("恒东瀛")
    
-- 增加员工 addPersonal
select * from information;
insert into information(name, in_date) values('测试1', '2024/5/26');
insert into information_kaoqin(name, department) values('测试1', '云丰');
delete from information where name='测试';
delete from information_kaoqin where id=177;
delete from data_kaoqin where id_info=176;


-- 删除员工 deletePersonal
select * from information where name = "武福子";
select * from nianjia;
select id from information_kaoqin where name='丁赵琼' and department='涵泰';
select id_data from data_kaoqin where id_info=177;