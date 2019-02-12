#include <stdio.h>
#include <winsock2.h>	// 适用于Windows下的VS编译环境

#define BUFSIZE 2048
#define IPSTR "61.128.226.218"		// 服务器IP地址; Host: www.cqzk.com.cn
#define PORT 80				// 可使用浏览器直接访问的网站均为80端口
#define OUTFILE "研究生信息.txt"		// 查询的结果保存到输出的文件中

#pragma  comment(lib,"ws2_32.lib")

struct student
{
	char identity[19];
	char examNum[16];
	char name[20];
	char politics[4];
	char foreign[4];
	char major1[4];
	char major2[4];
	char sumScore[4];
};

char* code(const char* strIn)
{
	char* item;
	unsigned int len = strlen(strIn);
	item = (char*)malloc(sizeof(char) * 3 * len + 1);
	if (item == NULL)
	{
		printf("由于内存不足，分配内存失败！\n");
		exit(0);
	}
	unsigned int h = 0;
	for (unsigned int i = 0; i < len; i++)
	{
		h += sprintf(item + h, "%%%X", (unsigned char)strIn[i]);
	}
	return item;
}

unsigned int ksbhCheck(const char* ksbh)
{
	switch (ksbh[5])
	{
	case '9':		return 2009;
	case '0':		return 2010;
	case '1':		return 2011;
	case '2':		return 2012;
	case '3':		return 2013;
	case '4':		return 2014;
	case '5':		return 2015;
	case '6':		return 2016;
	case '7':		return 2017;
	case '8':		return 2018;
	default:		return 2018;
	}
}

const char* grade(unsigned int gradeNum)
{
	switch (gradeNum)
	{ //当考生级数参数空缺时默认为2018级
	case 2008:		return "yjs08";
	case 2009:		return "yjs";
	case 2010:		return "";
	case 2011:		return "";
	case 2012:		return "yjs2012!";
	case 2013:		return "yjs2013_0204";
	case 2014:		return "YJS2014_0217B";
	case 2015:		return "YJS2015_Bir0212";
	case 2016:		return "yjscj2016";
	case 2017:		return "yjscj_2017";
	case 2018:		return "yjscjcx_2018";
	default:		return "yjscjcx_2018";
	}
}

SOCKET socketConnect(const char* src)
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		exit(1);
	}

	SOCKET sclient = socket(AF_INET, SOCK_STREAM, 0);
	if (sclient == INVALID_SOCKET)
	{
		printf("创建网络连接失败，本线程即将终止！socket error\n");
		exit(1);
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(PORT);
	serAddr.sin_addr.S_un.S_addr = inet_addr(src);
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("连接到服务器失败，本线程即将终止！connect error\n");
		closesocket(sclient);
		exit(1);
	}
	printf("已成功与远端建立了连接");
	return sclient;
}

char* getForm(const char* sfzh, const char* ksbh, char* name, const char* submit)
{
	unsigned int len;
	len = strlen("sfzh=&ksbh=&xm=&Submit=++++") + strlen(sfzh) + strlen(ksbh) + strlen(name) + strlen(submit) + 1;
	char* form;
	form = (char*)malloc(len * sizeof(char));
	if (form == NULL)
	{
		printf("分配内存失败！\n");
		exit(1);
	}
	memset(form, 0, len);		// 发送数据包中post的内容
	sprintf(form, "sfzh=%s&ksbh=%s&xm=%s&Submit=++%s++", sfzh, ksbh, name, submit);
	free(name);					// 查询时服务器并不会检测xm之后的数据
	return form;
}

unsigned int getPost(char* post, char* form, const char* grade)
{
	unsigned int h;
	memset(post, 0, BUFSIZE);		// 发送数据包中的格式控制信息
	h = sprintf(post, "POST /others/%s/yjs_search.jsp HTTP/1.1\n", grade);
	h += sprintf(post + h, "Host: www.cqzk.com.cn\n");
	h += sprintf(post + h, "Content-Type: application/x-www-form-urlencoded\n");
	h += sprintf(post + h, "Content-Length: %d\n\n", strlen(form));

	strcat(post, form);
	strcat(post, "\r\n\r\n");
	free(form);

	return strlen(post);
}

int socketRecv(char* strOut, SOCKET sclient)
{
	int ret;
	memset(strOut, 0, BUFSIZE);
	ret = recv(sclient, strOut, BUFSIZE - 1, 0);
	if (ret < 0)
	{
		printf("接收失败！\n");
	}
	else if (ret == 0)
	{
		printf("接收失败，连接被关闭！\n");
	}
	else
	{
		strOut[ret] = 0;
	}
	closesocket(sclient);
	WSACleanup();
	return ret;
}	// 使用后需要free(buf)

struct student format(const char* rcvData)
{
	struct student grdt;
	unsigned int i, cnt;
	i = 580;	// 前面为数据包格式控制信息，查询返回的内容从buf+580位置才开始
	cnt = 0;
	while (rcvData[i] != '\0') {
		if (rcvData[i - 3] == 'F') {
			if ((rcvData[i - 2] == 'F') && (rcvData[i - 1] == '>')) {
				switch (cnt++) {
				case 1:
					*((char*)memccpy(grdt.identity, rcvData + i, ' ', 19) - 1) = 0;
					i += 25;
					break;
				case 3:
					*((char*)memccpy(grdt.examNum, rcvData + i, ' ', 16) - 1) = 0;
					i += 25;
					break;
				case 5:
					*((char*)memccpy(grdt.name, rcvData + i, ' ', 20) - 1) = 0;
					i += 25;
					break;
				case 7:
					*((char*)memccpy(grdt.politics, rcvData + i, ' ', 4) - 1) = 0;
					i += 25;
					break;
				case 9:
					*((char*)memccpy(grdt.foreign, rcvData + i, ' ', 4) - 1) = 0;
					i += 25;
					break;
				case 11:
					*((char*)memccpy(grdt.major1, rcvData + i, ' ', 4) - 1) = 0;
					i += 25;
					break;
				case 13:
					*((char*)memccpy(grdt.major2, rcvData + i, ' ', 4) - 1) = 0;
					i += 25;
					break;
				case 15:
					*((char*)memccpy(grdt.sumScore, rcvData + i, ' ', 4) - 1) = 0;
					i += 60;
					break;
				default:
					//printf("项目标题\n");
					i += 25;
					break;
				}
			}	else { i++; }
		}	else { i += 5; }
	}
	if (cnt != 16)
	{
		printf("检测到的信息数量错误！\n");
		exit(1);
	}
	printf("身份证号：%s\t考生编号：%s\t姓名：%s\n", grdt.identity, grdt.examNum, grdt.name);
	printf("政治：\t%s\t外语：\t%s\t业务1：\t%s\t业务2：\t%s\n", grdt.politics, grdt.foreign, grdt.major1, grdt.major2);
	return grdt;
}

unsigned int writeFile(const char* strFile, const char* strIn, const char* rwa)
{
	FILE* fstream;
	int ret;
	fstream = fopen(strFile, rwa);
	if (fstream == NULL) {
		printf("打开文件失败！\n");
		ret = -1;
	}
	else {
		printf("成功打开文件");
		ret = fwrite(strIn, strlen(strIn), 1, fstream);
		if (ret == 0) {
			printf("，但写入数据时失败！\n");
		}
		else {
			printf("并写入数据\n");
		}
	}
	return ret;
}

int main(int argc, char* argv[])
{
	unsigned int gradeNum;
	char sfzh[19], ksbh[16];
	switch (argc)
	{
	case 4:
		gradeNum = atoi(argv[3]);
	case 3:
		if (strlen(argv[2]) == 15)
		{	//printf("考生编号读取正常\n");
			strcpy(sfzh, "");
			strcpy(ksbh, argv[2]);
			gradeNum = ksbhCheck(ksbh);	//当输入准考证号的时候忽略输入的考生级数
		}
		else if (strlen(argv[2]) == 16)
		{	//printf("考生编号最后多读取了一个换行符！\n");
			strcpy(sfzh, "");
			strcpy(ksbh, argv[2]);
			ksbh[15] = '\0';
			gradeNum = ksbhCheck(ksbh);	//当输入准考证号的时候忽略输入的考生级数
		}
		else if (strlen(argv[2]) == 18)
		{	//printf("身份证号读取正常\n");
			strcpy(sfzh, argv[2]);
			strcpy(ksbh, "");
			if (argc == 3) {
				printf("通过身份证查询考生信息时必须输入考生级数！\n");
			}
		}
		else if (strlen(argv[2]) == 19)
		{	//printf("身份证号最后多读取了一个换行符！\n");
			strcpy(sfzh, argv[2]);
			strcpy(ksbh, "");
			sfzh[18] = '\0';
			if (argc == 3) {
				printf("通过身份证查询考生信息时必须输入考生级数！\n");
			}
		}
		else {
			printf("请输入正确的15位考生编号或18位身份证号！\n");
			exit(0);
		}		// 当考生姓名参数空缺时，需要根据考生编号检索出考生姓名
		break;
	default:
		printf("使用方法：\n\tgraduate [考生姓名] [考生编号] [考生级数]\n");
		printf("\t或\n\tgraduate [考生姓名] [身份证号] [考生级数]\n\n");
		exit(0);
	}

	int ret;
	char buf[BUFSIZE];
	printf("-------------------------------------------------------------------------\n");
	SOCKET sclient = socketConnect(IPSTR);
	getPost(buf, getForm(sfzh, ksbh, code(argv[1]), "%B2%E9%D1%AF"), grade(gradeNum));

	ret = send(sclient, buf, strlen(buf), 0);
	if (ret < 0) { printf("发送失败！\n"); }
	else if (ret == 0) { printf("发送失败，连接被关闭！\n"); }
	else {
		printf("，消息发送成功，共发送了%d个字节\n", ret);
		//printf(buf);		// 打印发送的数据包内容，但因为%会导致显示格式错误
	}

	ret = socketRecv(buf, sclient);
	//printf("%s\n", buf + 580);
	if (strlen(buf) < 1000)				// 该值在Linux下为580，原因暂时不明
	{
		printf("查询结果中无有效数据，无法查询到考生%s的信息！\n", ksbh);
		exit(1);	// 根据提供的信息无法查到考生信息，极可能是非重庆考生
	}
	else
	{
		struct student postGrdt;
		postGrdt = format(buf);
		char line[60];					// Linux下不保存姓名，可以设置为50
		sprintf(line, "%s\t%s\t%s\t%s\t%s\t%s\t%s\n", postGrdt.examNum, postGrdt.identity, postGrdt.name, postGrdt.politics, postGrdt.foreign, postGrdt.major1, postGrdt.major2);
		if (writeFile(OUTFILE, line, "a") < 1) { exit(1); }
	}
	return 0;
}
