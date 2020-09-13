/*
 * @Descripttion: 
 * @version: 
 * @Author: Kevincoooool
 * @Date: 2020-09-06 00:28:10
 * @LastEditors: Kevincoooool
 * @LastEditTime: 2020-09-06 12:03:08
 * @FilePath: \9_TcpAndUdp\app\user\UdpClient.c
 */

#include "driver/uart.h"	//����0��Ҫ��ͷ�ļ�
#include "osapi.h"			//����1��Ҫ��ͷ�ļ�
#include "user_interface.h" //WIFI������Ҫ��ͷ�ļ�
#include "espconn.h"		//TCP������Ҫ��ͷ�ļ�
#include "mem.h"			//ϵͳ������Ҫ��ͷ�ļ�
#include "gpio.h"			//ϵͳ������Ҫ��ͷ�ļ�

struct espconn user_udp_espconn;
os_timer_t checkTimer_wifistate;

void ICACHE_FLASH_ATTR user_udp_sent_cb(void *arg) //����
{
}

void ICACHE_FLASH_ATTR user_udp_recv_cb(void *arg, //����
										char *pdata, unsigned short len)
{
	//		os_printf("�������ݣ�%s", pdata);

	//ÿ�η�������ȷ����������
	//	 user_udp_espconn.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
	//	 user_udp_espconn.type = ESPCONN_UDP;
	//	 user_udp_espconn.proto.udp->local_port = 8266;
	//	 user_udp_espconn.proto.udp->remote_port = 8266;
	//	 const char udp_remote_ip[4] = {192, 168, 4, 1};
	//	 os_memcpy(user_udp_espconn.proto.udp->remote_ip, udp_remote_ip, 4);

	uart0_tx_buffer((uint8 *)pdata, len);
}

void UDP_Check_WifiState(void)
{

	uint8 getState = wifi_station_get_connect_status();

	//���״̬��ȷ��֤���Ѿ�����
	if (getState == STATION_GOT_IP)
	{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 0);
		os_delay_us(100000);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 1);
		os_delay_us(100000);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 0);
		os_delay_us(100000);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 1);
		os_printf("WIFI���ӳɹ���");
		os_timer_disarm(&checkTimer_wifistate);

		wifi_set_broadcast_if(0x01);										//���� ESP8266 ���� UDP�㲥��ʱ���� station �ӿڷ���
		user_udp_espconn.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp)); //����ռ�
		user_udp_espconn.type = ESPCONN_UDP;								//��������ΪUDPЭ��
		user_udp_espconn.proto.udp->local_port = 8266;						//���ض˿�
		user_udp_espconn.proto.udp->remote_port = 8266;						//Ŀ��˿�
		const char udp_remote_ip[4] = {192, 168, 4, 1};						//Ŀ��IP��ַ���㲥��
		os_memcpy(user_udp_espconn.proto.udp->remote_ip, udp_remote_ip, 4);
		const char udp_local_ip[4] = {192, 168, 5, 1}; //Ŀ��IP��ַ���㲥��
		os_memcpy(user_udp_espconn.proto.udp->local_ip, udp_local_ip, 4);
		espconn_regist_recvcb(&user_udp_espconn, user_udp_recv_cb); //����
		espconn_regist_sentcb(&user_udp_espconn, user_udp_sent_cb); //����
		espconn_create(&user_udp_espconn);							//���� UDP ����
	}
}

void udp_client_init() //��ʼ��
{
	wifi_set_opmode(0x01); //����ΪSTATIONģʽ
	struct station_config stationConf;
	os_strcpy(stationConf.ssid, "DAPAP");		 //�ĳ���Ҫ���ӵ� ·�������û���
	os_strcpy(stationConf.password, "99999999"); //�ĳ���Ҫ���ӵ�·����������

	wifi_station_set_config(&stationConf);	//����WiFi station�ӿ����ã������浽 flash
	wifi_station_connect();					//����·����
	os_timer_disarm(&checkTimer_wifistate); //ȡ����ʱ����ʱ
	os_timer_setfn(&checkTimer_wifistate, (os_timer_func_t *)UDP_Check_WifiState,
				   NULL);						  //���ö�ʱ���ص�����
	os_timer_arm(&checkTimer_wifistate, 1000, 1); //������ʱ������λ������
}
