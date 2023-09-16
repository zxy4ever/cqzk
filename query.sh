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
		# ./student ${xm} ${ksbh} 2016
		
		if test ${xm}=${preXm} ; then
			if test $error -eq 1 ' then
				$error=./student ${xm} ${ksbh} 2016
			fi
		else
			if test $error -eq 1 ; then
				echo "无法找到${preXm}的信息！"
			fi
			$error=./student ${xm} ${ksbh} 2016
		fi
		${preXm}=$xm
		echo "已处理完第$((($SUM + 1) / 2))条信息"
	fi
	SUM=$(( $SUM + 1))
done
echo "共查询了$(($SUM / 2))条信息"
