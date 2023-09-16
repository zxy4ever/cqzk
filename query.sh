#!/bin/sh 

# cqzk需输入编号后的考生姓名，student可输入gb2312编码的考生姓名，但目前仍有问题

SUM=0
for line in $(cat codedName.txt)
do
	if test $(($SUM%2)) -eq 0 ; then
		xm=${line}
		# echo "${ksbh}"
	else
		ksbh=${line}
		echo "student ${xm} ${ksbh}"
		./student ${xm} ${ksbh}
		echo "已处理完第$((($SUM + 1) / 2))条信息"
		echo "-------------------------------------------------------------------------"
	fi
	SUM=$(( $SUM + 1))
done
echo "共查询了$(($SUM / 2))条信息"
