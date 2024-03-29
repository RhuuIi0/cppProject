create table infomation(
	name varchar(255) primary key not null, 
    in_date date not null
    );
show tables;
desc information;
select *from information;
insert into information values("邸玉婷", "2019/7/1");

create table nianjia(
	name varchar(255) not null,
	in_date date not null,
    days FLOAT not null,
    UNIQUE(name, in_date, days),
    FOREIGN KEY(name) REFERENCES information(name) ON UPDATE RESTRICT on DELETE RESTRICT
	);
desc nianjia;
select *from nianjia;
insert into nianjia values("邸玉婷", "2021/6/30", 1.0);

select name, group_concat(in_date) from nianjia group by name;

delete from information where name="田钧辉";

show character set;

select sum(days) from nianjia where name="heitu" group by name;

delete from information;