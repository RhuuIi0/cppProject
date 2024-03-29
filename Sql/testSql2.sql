select * from information where name="白云";
select * from nianjia where name="白云";
select * from nianjia;

select sum(days) from nianjia where name="白云" and in_date BETWEEN "2021/6/26" and "2022/6/26" group by name;

select name, group_concat(in_date) from nianjia where name="白云" and in_date BETWEEN "2021/6/26" and "2022/6/26" group by name;
select COUNT(in_date) from nianjia where name="白云" and in_date BETWEEN "2022/6/26" and "2023/6/26";
select in_date, days from nianjia where name="白云" and in_date BETWEEN "2022/6/26" and "2023/6/26";

select * from information where month(in_date)="1";

insert into nianjia values("邸玉婷", "2021/3/3", 2);
delete from nianjia where name="白云" and in_date="2021/09/30" and days=1.5;
delete from nianjia where name="邸玉婷";

delete from nianjia where year(in_date)=2021;