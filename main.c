#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include<pthread.h>
#include <unistd.h>
#include <math.h>
#include<stdbool.h>
#include "creat_listen_socke.h"
#include "cJSON.h"
#include "send_server_state.h"
#include "get_time.h"
#define MAX_LISTEN 10
#define RECV_DATA_OUTTIME 300 //5分钟

int save_airdata(struct TIME_cj time,char *airdata);
//--------------------------------------
char *host="0.0.0.0";
char *port="9000";

float range_humidity=5;
float range_temperature=3;
float range_value=20;
float range_hcho=50;

//--------------------------------------
int Client_socket=0;
struct timeval recv_wait_time = {10,0}; 
cJSON * save_ahours_data[3][4];
int write_buf_index=0;


float max_vlue=0,max_hco=0;
float hcho=0;

int rev_interval_time_s=120,rev_all=0,rev_ok=0;
struct TIME_cj rev_time,last_save_time,save_rev_time[3];

int main(int argc,const char* argv[])
{
    {
        if(argc==1)
        {
            argv[1]=host;
            argv[2]=port;
        }
        else if(argc==2)
        {
            argv[2]=port;
        }
    }
    int listen_socke=creat_listen_socke(argv[2],argv[1],MAX_LISTEN);

    struct sockaddr_in remote;
	socklen_t len=sizeof(struct sockaddr_in);
    get_time(&last_save_time);

    while(1)
    {
        // printf("listen_socke has made\n");
        Client_socket=accept(listen_socke,(struct sockaddr*)&remote,&len);
        if(Client_socket<0)
		{
			// printf("Client_socket_error\n");
			continue;
		}
        printf("Client connecting\n");
        setsockopt(Client_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&recv_wait_time,sizeof(struct timeval)); //设置非拥塞等待
//-----------------------------------------------------
char rev_buf[120];
bool rev_airdata_flag=false;
char air_data[110];
int recv_outtime=0;
//-----------------------------------------------------
// pthread_t t0;
// pthread_create(&t0, NULL, text, NULL);
//----------------------------------------------------
        while(1)
        {
            //sleep(1);
            // printf("start_read\n");
            int rev_length=recv(Client_socket,rev_buf,sizeof(rev_buf),0);
            // int rev_length=read(Client_socket,rev_buf,sizeof(rev_buf));
            // printf("read_over\n");
            {
                if(rev_length<=0)       //如果没有收到数据
                {
                    recv_outtime++;     //计数，没有收到数据的计数
                    if(recv_outtime>=RECV_DATA_OUTTIME) //超过RECV_DATA_OUTTIME次，发送信息到M1，激活M1发送空气数据过来
                    {
                        char send_buf[55]; //发送缓冲器，发送的信息会激活M1发送空气数据
                        Get_send_char(send_buf);
                        // printf("try send state\n");
                        send(Client_socket,send_buf,55,MSG_NOSIGNAL);//发送激活信息,MSG_NOSINGNAL忽略信号
                        // printf("send over\n");
                        sleep(3);   //休眠三秒
                        if(recv_outtime>=(RECV_DATA_OUTTIME+3)) //如果没有收到数据超过规定次数，则任务改TCP连接已经断开
                        {
                            // printf("connect is close\n");
                            close(Client_socket);//服务器主动关闭套接字
                            break;//退出循环，然后重新等待M1连接
                        }
                    }
                    continue;
                }
            }
            recv_outtime=0;//重置计数器
            get_time(&rev_time);//获取收到数据的时间
            for(int i = 0; i < rev_length; i++)//从接受的数据提取信息,每次发送的数据中，除了json格式的空气数据，还有一些不明字符
            {
                if(rev_buf[i]=='{') //赛选空气数据
                {
                    // printf("%d:buf_to_data start\n%s:",i,rev_time.time_char);
                    for (int j = 0; rev_buf[i - 1] != '}'&&i<rev_length; j++,i++)
                    {
                        air_data[j] = rev_buf[i];
                        printf("%c",air_data[j]);
                    }
                    if(rev_buf[i-1]=='}')//收到的json数据完整，有时候会出现接收的json的数据不完整
                    rev_airdata_flag = true;//打开接受数据有效标志位
                    // printf("\nbuf_to_data over rev_airdata_flag:%d\n",rev_airdata_flag);
                }
            }
            if(rev_airdata_flag==true)//收到的数据完整，解析
            {
                save_airdata(rev_time,air_data);//存储数据到M1_airdata.txt
                rev_airdata_flag=false;
            }
        }
        
    } 
}
int save_airdata(struct TIME_cj time,char *airdata)//PM2.5数据最大值，甲醛数据最高值,每隔两小时采集一次数据
{
    cJSON *json=0,*hum=0,*tem=0,*vla=0,*hco=0;

    json=cJSON_Parse(airdata);
    if(!json)
    {
        return -1;
    }
    hum=cJSON_GetObjectItem(json,"humidity");
    tem=cJSON_GetObjectItem(json,"temperature");
    vla=cJSON_GetObjectItem(json,"value");
    hco=cJSON_GetObjectItem(json,"hcho");

    rev_all++;
    if(atof(vla->valuestring)<=50&&atof(hco->valuestring)<=80)
    rev_ok++;

    if(atof(vla->valuestring)>max_vlue)
    {
        save_rev_time[0]=time;
        max_vlue=atof(vla->valuestring);
        save_ahours_data[0][0]=hum;
        save_ahours_data[0][1]=tem;
        save_ahours_data[0][2]=vla;
        save_ahours_data[0][3]=hco;
    }

    if(atof(hco->valuestring)>max_hco)
    {
        save_rev_time[1]=time;
        max_hco=atof(hco->valuestring);
        save_ahours_data[1][0]=hum;
        save_ahours_data[1][1]=tem;
        save_ahours_data[1][2]=vla;
        save_ahours_data[1][3]=hco;
    }

    if((time.time_int-last_save_time.time_int)>=3600&&rev_all>3)//收到的数据有三分并且过了2小时
    {
        save_rev_time[2]=time;
        last_save_time=time;
        max_hco=0;max_vlue=0;
        save_ahours_data[2][0]=hum;
        save_ahours_data[2][1]=tem;
        save_ahours_data[2][2]=vla;
        save_ahours_data[2][3]=hco;

        FILE *fp=fopen("m1_airdata.txt","a+");//打开存储数据文本

        for(int i=0;i<3;i++)
        {
            char write_buf[110];
            if(atof(save_ahours_data[i][2]->valuestring)>=100||atof(save_ahours_data[i][3]->valuestring)>=120)
                sprintf(write_buf,"|X__%s__|__%s__|__%s__|__%s__|__%.2f__|__%d/%d__|\n",save_rev_time[i].time_char,save_ahours_data[i][0]->valuestring,save_ahours_data[i][1]->valuestring,save_ahours_data[i][2]->valuestring,atof(save_ahours_data[i][3]->valuestring)/1000,rev_ok,rev_all);
            else if(atof(save_ahours_data[i][2]->valuestring)>50||atof(save_ahours_data[i][3]->valuestring)>80)
                sprintf(write_buf,"|⚠_%s_|_%s_|_%s_|_%s_|_%.2f_|_%d/%d_|\n",save_rev_time[i].time_char,save_ahours_data[i][0]->valuestring,save_ahours_data[i][1]->valuestring,save_ahours_data[i][2]->valuestring,atof(save_ahours_data[i][3]->valuestring)/1000,rev_ok,rev_all);
            else
                sprintf(write_buf,"|√%s|%s|%s|%s|%.2f|%d/%d|\n",save_rev_time[i].time_char,save_ahours_data[i][0]->valuestring,save_ahours_data[i][1]->valuestring,save_ahours_data[i][2]->valuestring,atof(save_ahours_data[i][3]->valuestring)/1000,rev_ok,rev_all);

            fputs(write_buf,fp);
        }
        fclose(fp);
        rev_all=0;rev_ok=0;
    }
    //------------------------------------------------------------------------------------------------------

    // if(fabs(atof(hum->valuestring)-last_humidity)>=range_humidity ||
    //     fabs(atof(tem->valuestring)-last_temperature)>=range_temperature ||
    //     fabs(atof(vla->valuestring)-last_value)>=range_value ||
    //     fabs(atof(hco->valuestring)-last_hcho)>=range_hcho)
    // {
    //     last_humidity=atof(hum->valuestring);
    //     last_temperature=atof(tem->valuestring);
    //     last_value=atof(vla->valuestring);
    //     last_hcho=atof(hco->valuestring);
        
    //     if(atof(vla->valuestring)>=100||atof(hco->valuestring)>=120)
    //     sprintf(write_buf[write_buf_index++],"|X__%s__|__%s__|__%s__|__%s__|__%.2f__|\n",time.time_char,tem->valuestring,hum->valuestring,vla->valuestring,last_hcho/1000);

    //     else if(atof(vla->valuestring)>=50||atof(hco->valuestring)>=80)
    //     sprintf(write_buf[write_buf_index++],"|⚠_%s_|_%s_|_%s_|_%s_|_%.2f_|\n",time.time_char,tem->valuestring,hum->valuestring,vla->valuestring,last_hcho/1000);

    //     else
    //     sprintf(write_buf[write_buf_index++],"|√%s|%s|%s|%s|%.2f|\n",time.time_char,tem->valuestring,hum->valuestring,vla->valuestring,last_hcho/1000);
        
    //     if(write_buf_index==3)
    //     {
    //         write_buf_index=0;
    //         FILE *fp=fopen("m1_airdata.txt","a+");
    //         fputs(write_buf[0],fp);
    //         fputs(write_buf[1],fp);
    //         fputs(write_buf[2],fp);
    //         fclose(fp);
    //     }
    // }
    return 0;
}
