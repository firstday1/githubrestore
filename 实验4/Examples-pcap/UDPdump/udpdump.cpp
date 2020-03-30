#include<iostream>
#include<sstream>
#include<string>
#include<map>
#include<fstream>
#include <pcap.h>
#pragma warning(disable:4996)
using namespace std;
map<string, string[3]> ftp;//类模板一样的东西？可以储存IP地址和是否登录成功,如果要记录是否登录成功需要
ofstream out("mark.txt");
string txt;//输入到txt里面的源 MAC、源 IP、目标 MAC、目标 IP
typedef struct mac_header
{
	u_char dest_addr[6];
	u_char src_addr[6];
	u_char type[2];
} mac_header;

/* IPv4 首部 ,20字节*/
typedef struct ip_header {
	u_char  ver_ihl;        // 版本 (4 bits) + 首部长度 (4 bits)  
	u_char  tos;            // 服务类型(Type of service)  
	u_short tlen;           // 总长(Total length)  
	u_short identification; // 标识(Identification)  
	u_short flags_fo;       // 标志位(Flags) (3 bits) + 段偏移量(Fragment offset) (13 bits)  
	u_char  ttl;            // 存活时间(Time to live)  
	u_char  proto;          // 协议(Protocol)  
	u_short crc;            // 首部校验和(Header checksum)  
	u_char  saddr[4];      // 源地址(Source address)  
	u_char  daddr[4];      // 目的地址(Destination address)  
	u_int   op_pad;         // 选项与填充(Option + Padding)  
}ip_header;
//TCP头部，总长度20字节  
typedef struct tcp_header
{
	u_short sport;            //源端口号  
	u_short dport;             //目的端口号  
	u_int th_seq;                //序列号  
	u_int th_ack;               //确认号  
	u_int th1 : 4;              //tcp头部长度  
	u_int th_res : 4;             //6位中的4位首部长度  
	u_int th_res2 : 2;            //6位中的2位首部长度  
	u_char th_flags;            //6位标志位  
	u_short th_win;             //16位窗口大小  
	u_short th_sum;             //16位tcp检验和  
	u_short th_urp;             //16位紧急指针  
}tcp_header;

/* 回调函数原型 */
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data);
string get_request_m_ip_message(const u_char* pkt_data);
string get_response_m_ip_message(const u_char* pkt_data);
void print(const struct pcap_pkthdr* header, string m_ip_message);

int main()
{
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int inum;
	int i = 0;
	pcap_t* adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[] = "tcp";
	struct bpf_program fcode;

	/* 获得设备列表 */
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* 打印列表 */
	for (d = alldevs; d; d = d->next)
	{
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}

	printf("Enter the interface number (1-%d):", i);
	scanf("%d", &inum);

	if (inum < 1 || inum > i)
	{
		printf("\nInterface number out of range.\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* 跳转到已选设备 */
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	/* 打开适配器 */
	if ((adhandle = pcap_open_live(d->name,  // 设备名  
		65536,     // 要捕捉的数据包的部分  
				   // 65535保证能捕获到不同数据链路层上的每个数据包的全部内容  
		1,         // 混杂模式  
		1000,      // 读取超时时间  
		errbuf     // 错误缓冲池  
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* 检查数据链路层，为了简单，我们只考虑以太网 */
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
		/* 获得接口第一个地址的掩码 */
		netmask = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* 如果接口没有地址，那么我们假设一个C类的掩码 */
		netmask = 0xffffff;


	//编译过滤器  
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	//设置过滤器  
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("\nlistening on %s...\n", d->description);

	/* 释放设备列表 */
	pcap_freealldevs(alldevs);

	/* 开始捕捉 */
	pcap_loop(adhandle, 0, packet_handler, NULL);
	out.close();
	return 0;
}

/* 回调函数，当收到每一个数据包时会被libpcap所调用 */
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
	struct tm* ltime;
	//char timestr[16];
	ip_header* ih;
	tcp_header* uh;
	u_int ip_len;
	u_short sport, dport;
	time_t local_tv_sec;

	/* 将时间戳转换成可识别的格式 */
	local_tv_sec = header->ts.tv_sec;
	ltime = localtime(&local_tv_sec);
	//strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

	int head = 54;//14位以太网头，20位ip头，20位tcp头  选择出command为USER和PASS和230,530的包
	string com;
	for (int i = 0; i < 4; i++)
		com += (char)pkt_data[head + i];

	//下面的m_ip_message都为同一个值：目的IP地址
	if (com == "USER")
	{
		string m_ip_message = get_request_m_ip_message(pkt_data);
		string user;
		ostringstream sout;
		for (int i = head + 5; pkt_data[i] != 13; i++)
		{
			sout << pkt_data[i];
		}
		user = sout.str();
		ftp[m_ip_message][0] = user;
	}
	if (com == "PASS")
	{
		string m_ip_message = get_request_m_ip_message(pkt_data);
		string pass;
		ostringstream sout;
		for (int i = head + 5; pkt_data[i] != 13; i++)
		{
			sout << pkt_data[i];
		}
		pass = sout.str();
		ftp[m_ip_message][1] = pass;
	}
	if (com == "230 ")
	{
		string m_ip_message = get_response_m_ip_message(pkt_data);
		ftp[m_ip_message][2] = "SUCCEED";
		print(header, m_ip_message);
	}
	if (com == "530 ")
	{
		string m_ip_message = get_response_m_ip_message(pkt_data);
		ftp[m_ip_message][2] = "FAILD";
		print(header, m_ip_message);
	}
}
//返回接收方IP地址
string get_request_m_ip_message(const u_char* pkt_data)
{
	mac_header* mh;
	ip_header* ih;
	string m_ip_message;
	string str;//empty string
	ostringstream sout;
	ostringstream txt_sout;//输入到文件里的
	int length = sizeof(mac_header) + sizeof(ip_header);
	mh = (mac_header*)pkt_data;
	ih = (ip_header*)(pkt_data + sizeof(mac_header));

	//输出源Mac地址
	for (int i = 0; i < 5; i++)
		txt_sout << hex << (int)(mh->src_addr[i]) << "-";
	txt_sout << (int)(mh->src_addr[5]) << ",";


	//输出源IP地址(本机的IP地址)
	for (int i = 0; i < 3; i++)
		txt_sout << dec << (int)(ih->saddr[i]) << ".";
	txt_sout << (int)(ih->saddr[3]) << ",";


	//输出接收方Mac地址
	for (int i = 0; i < 5; i++)
		txt_sout << hex << (int)(mh->dest_addr[i]) << "-";
	txt_sout << (int)(mh->dest_addr[5]) << ",";


	//输出接收方IP地址(ftp的IP地址)
	for (int i = 0; i < 3; i++)
	{
		sout << dec << (int)(ih->daddr[i]) << ".";
		txt_sout << dec << (int)(ih->daddr[i]) << ".";
	}
	sout << (int)(ih->daddr[3]);
	txt_sout << (int)(ih->daddr[3]);
	txt = txt_sout.str();
	m_ip_message = sout.str();
	return m_ip_message;
}
//返回反馈信息发送方的IP地址(因为反馈信息是本机接受方故要和上方函数反过来操作返回发送方IP地址)
string get_response_m_ip_message(const u_char* pkt_data)
{
	//mac_header* mh;
	ip_header* ih;
	string m_ip_message;
	string str;//empty string
	ostringstream sout;
	int length = sizeof(mac_header) + sizeof(ip_header);
	//mh = (mac_header*)pkt_data;
	ih = (ip_header*)(pkt_data + sizeof(mac_header));

	//输出目的Mac地址
	/*for (int i = 0; i < 5; i++)
		sout << hex << (int)(mh->dest_addr[i]) << "-";
	sout << (int)(mh->dest_addr[5]) << ",";
	*/

	//输出源ip地址(本机IP地址)
	/*for (int i = 0; i < 3; i++)
		sout << dec << (int)(ih->daddr[i]) << ".";
	sout << (int)(ih->daddr[3]) << ",";
	*/

	//输出源Mac地址
	/*for (int i = 0; i < 5; i++)
		sout << hex << (int)(mh->src_addr[i]) << "-";
	sout << (int)(mh->src_addr[5]) << ",";
	*/
	for (int i = 0; i < 3; i++)
		sout << dec << (int)(ih->saddr[i]) << ".";
	sout << (int)(ih->saddr[3]);
	m_ip_message = sout.str();
	return m_ip_message;
}
void print(const struct pcap_pkthdr* header, string m_ip_message)
{
	struct tm* ltime;
	char timestr[32];
	time_t local_tv_sec;
	/* 将时间戳转化为可识别的格式 */
	local_tv_sec = header->ts.tv_sec;
	ltime = localtime(&local_tv_sec);
	strftime(timestr, sizeof timestr, "%Y-%m-%d %H:%M:%S", ltime);
	/* 打印时间戳*/
	cout << timestr << "  ";

	//输出ftp地址
	cout << "FTP:" << m_ip_message << "  ";

	//输出账号
	cout << "USR:" << ftp[m_ip_message][0] << "  ";

	//输出密码
	cout << "PAS:" << ftp[m_ip_message][1] << "  ";

	//输出是否登录成功
	cout << "STA:" << ftp[m_ip_message][2] << endl;

	out << timestr << ",";
	out << txt << ",";
	for (int i = 0; i < 2; i++)
		out << ftp[m_ip_message][i] << ",";
	out << ftp[m_ip_message][2] << endl;
	ftp.erase(m_ip_message);
}