/*
 * Copyright (c) 1999 - 2005 NetGroup, Politecnico di Torino (Italy)
 * Copyright (c) 2005 - 2006 CACE Technologies, Davis (California)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Politecnico di Torino, CACE Technologies
 * nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifdef _MSC_VER
 /*
  * we do not want the warnings about the old deprecated and unsecure CRT functions
  * since these examples can be compiled under *nix as well
  */
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <pcap.h>
#include<iphlpapi.h>
#include<Windows.h>
#include <time.h>
#include <stdio.h>
#define HAVE_REMOTE
#include <Packet32.h>
#include <ntddndis.h>
#include <Iphlpapi.h>
#pragma comment(lib, "Iphlpapi")
#pragma comment(lib, "Packet")
#pragma comment(lib, "wpcap")
#pragma comment(lib,"Iphlpapi")
#pragma comment(lib,"WS2_32")
using namespace std;
static u_char LOCALMAC[6];

  /* 4 bytes IP address */
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

/* IPv4 header */
typedef struct ip_header
{
	u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
	u_char	tos;			// Type of service 
	u_short tlen;			// Total length 
	u_short identification; // Identification
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
	u_char	ttl;			// Time to live
	u_char	proto;			// Protocol
	u_short crc;			// Header checksum
	ip_address	saddr;		// Source address
	ip_address	daddr;		// Destination address
	u_int	op_pad;			// Option + Padding
}ip_header;

typedef struct mac_header
{
	u_char dest_addr[6]; 
	u_char src_addr[6]; 
	u_char type[2];
}mac_header;

/* UDP header*/
typedef struct udp_header
{
	u_short sport;			// Source port
	u_short dport;			// Destination port
	u_short len;			// Datagram length
	u_short crc;			// Checksum
}udp_header;

/* prototype of the packet handler */
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data);


int main()
{
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int inum;
	int i = 0;
	pcap_t* adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[] = "ip and udp";
	struct bpf_program fcode;

	/* Retrieve the device list 获取设备列表*/
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* Print the list */
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

	/* Check if the user specified a valid adapter */
	if (inum < 1 || inum > i)
	{
		printf("\nAdapter number out of range.\n");

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* Jump to the selected adapter 跳转到所选适配器*/
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	/* Open the adapter */
	if ((adhandle = pcap_open_live(d->name,	// name of the device
		65536,			// portion of the packet to capture. 
					   // 65536 grants that the whole packet will be captured on all the MACs.
		1,				// promiscuous mode (nonzero means promiscuous)
		1000,			// read timeout
		errbuf			// error buffer
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/*获取Mac地址并存在定向容器里*/
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulLen = 0;
	::GetAdaptersInfo(pAdapterInfo, &ulLen);
	pAdapterInfo = (PIP_ADAPTER_INFO)::malloc(ulLen);
	::GetAdaptersInfo(pAdapterInfo, &ulLen);
	int count = 0;
	while (pAdapterInfo)
	{
		printf("NIC %d: \n", ++count);
		printf("\tIP: %s; Mask: %s; Gateway: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, pAdapterInfo->IpAddressList.IpMask.String, pAdapterInfo->GatewayList.IpAddress.String); printf("\tName: %s; Desc: %s\n", pAdapterInfo->AdapterName, pAdapterInfo->Description);
		printf("\tMAC: ");
		for (size_t i = 0; i < pAdapterInfo->AddressLength; i++)
		{
			printf("%02X", pAdapterInfo->Address[i]);
		}
		if (count == inum) { for (size_t i = 0; i < pAdapterInfo->AddressLength; i++) { LOCALMAC[i] = pAdapterInfo->Address[i]; } }
		printf("\n"); pAdapterInfo = pAdapterInfo->Next;
	}
	//system("pause");
	if (pAdapterInfo) { free(pAdapterInfo); }

	/* Check the link layer. We support only Ethernet for simplicity. */
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask = 0xffffff;


	//compile the filter
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	//set the filter
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("\nlistening on %s...\n", d->description);

	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);

	/* start the capture */
	pcap_loop(adhandle, 0, packet_handler, NULL);
	return 0;
}

static int r_time = 0;//抓的帧数
static int Glen = 0;//接受的总长度
static int Slen = 0;//发送的总长度

/* Callback function invoked by libpcap for every incoming packet */
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
	mac_header* a;
	struct tm* ltime;
	char timestr[32];
	ip_header* ih;
	udp_header* uh;
	u_int ip_len;
	u_short sport, dport;
	time_t local_tv_sec;
	time_t Local_Time;
	FILE* fileopen = fopen("putout.txt", "a+");
	if (!fileopen) {
		puts("文件打开失败");
		return;
	}

	/*
	 * unused parameter
	 */
	(VOID)(param);

	/* convert the timestamp to readable format */
	local_tv_sec = header->ts.tv_sec;
	ltime = localtime(&local_tv_sec);
	strftime(timestr, sizeof timestr, "%Y-%m-%d %H:%M:%S", ltime);

	/* print timestamp and length of the packet */
	//printf("%s.%.6d len:%d ", timestr, header->ts.tv_usec, header->len);

	/* retireve the position of the ip header */
	ih = (ip_header*)(pkt_data + sizeof(mac_header)); //length of ethernet header

	/* retireve the position of the udp header */
	ip_len = (ih->ver_ihl & 0xf) * 4;
	a= (mac_header*)pkt_data;
	/********************************************************************/
	/*打印时间*/
	printf("%s\t", timestr);
	int length = sizeof(mac_header) + sizeof(ip_header);
	for (int i = 0; i < length; i++)
	{
		printf("%02X ", pkt_data[i]);//pkt_data这个变量的意义为那一帧所包含的内容
		if ((i & 0xF) == 0xF) printf("\n");
	}
	printf("\nlength:%d\n",header->len);
	//打印IP地址和Mac地址
	printf("mac_header:\n");
	printf("\tdest_addr: ");
	for (int i = 0; i < 6; i++) 
	{
		printf("%02X ", a->dest_addr[i]);
	}
	printf("\n");
	printf("\tsrc_addr: ");
	for (int i = 0; i < 6; i++)
	{
		printf("%02X ", a->src_addr[i]);
	}
	printf("\n");
	printf("\ttype: %04X", ntohs((short int)(a->type)));
	printf("\n");
	printf("ip_header:\n");
	printf("\t%-10s: %02X\n", "ver_ihl", ih->ver_ihl);
	printf("\t%-10s: %02X\n", "tos", ih->tos);
	printf("\t%-10s: %04X\n", "tlen", ntohs(ih->tlen));
	printf("\t%-10s: %04X\n", "identification", ntohs(ih-> identification));
	printf("\t%-10s: %04X\n", "flags_fo", ntohs(ih->flags_fo));
	printf("\t%-10s: %02X\n", "ttl", ih->ttl); 
	printf("\t%-10s: %02X\n", "proto", ih->proto);
	printf("\t%-10s: %04X\n", "crc", ntohs(ih->crc));
	printf("\t%-10s: %08X\n", "op_pad", ntohs(ih->op_pad));
	printf("\t%-10s: ", "saddr");
	printf("%02X %02X %02X %02X  ", ih->saddr.byte1, ih->saddr.byte2, ih->saddr.byte3, ih->saddr.byte4);//列出源IP址
	printf("%d.%d.%d.%d\n", ih->saddr.byte1, ih->saddr.byte2, ih->saddr.byte3, ih->saddr.byte4);//列出源IP址
	printf("\t%-10s: ", "daddr");
	printf("%02X %02X %02X %02X  ", ih->daddr.byte1, ih->daddr.byte2, ih->daddr.byte3, ih->daddr.byte4);//列出目標IP址
	printf("%d.%d.%d.%d\n", ih->daddr.byte1, ih->daddr.byte2, ih->daddr.byte3, ih->daddr.byte4);//列出目標IP址
	r_time++;
	if (r_time == 60)//60帧抓一次
	{
		fprintf(fileopen, "%s,", timestr);
		for (int i = 0; i < 6; i++)
		{
			fprintf(fileopen, "%02X", a->src_addr[i]);
			if ((i + 1) != 6) {
				fprintf(fileopen, "-");
			}
		}
		fprintf(fileopen, ",%d.%d.%d.%d,", ih->saddr.byte1, ih->saddr.byte2, ih->saddr.byte3, ih->saddr.byte4);
		for (int i = 0; i < 6; i++)
		{
			fprintf(fileopen, "%02X", a->dest_addr[i]);
			if ((i + 1) != 6) {
				fprintf(fileopen, "-");
			}
		}
		fprintf(fileopen, ",%d.%d.%d.%d,", ih->daddr.byte1, ih->daddr.byte2, ih->daddr.byte3, ih->daddr.byte4);
		fprintf(fileopen, "%d\n", header->len);
		/* print length of the packet*/
		printf("Now,Getlen:%d,SendLen:%d", Glen, Slen);
		r_time = 0;
	}
	fclose(fileopen);
	for (int i = 0; i < 6; i++)
	{
		if (LOCALMAC[i] == a->src_addr[i])
		{
			if (i + 1 == 6)
			{
				Slen += header->len;
			}
			else { continue; }
		}
	}//表示发送端
	for (int i = 0; i < 6; i++)
	{
		if (LOCALMAC[i] == a->dest_addr[i])
		{
			if (i + 1 == 6)
			{
				Glen += header->len;
			}
			else { continue; }
		}
	}//表示接受端
	/* convert from network byte order to host byte order */
	//sport = ntohs(uh->sport);
	//dport = ntohs(uh->dport);

	/* print ip addresses and udp ports */
	/*printf("%d.%d.%d.%d.%d -> %d.%d.%d.%d.%d\n",
		ih->saddr.byte1,
		ih->saddr.byte2,
		ih->saddr.byte3,
		ih->saddr.byte4,
		sport,
		ih->daddr.byte1,
		ih->daddr.byte2,
		ih->daddr.byte3,
		ih->daddr.byte4,
		dport);*/
}
