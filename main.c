#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
//#include <lirc/lirc_client.h>
#include <time.h>
//#include <fcntl.h>
#include "udpClient.h"
#include "tcpClient.h"
#include "HostIP.h"

#define GPIO_PIN_IN	1
#define BASEHZ		560  	//NEC
//gcc -o main *.c -lwiringPi


int btn1[32] = {   8, 1, 1, 1, 1,
               		3, 3, 1, 1, 3,
               		3, 3, 3, 1, 1,
              		3, 3, 3, 3, 3,
               		1, 3, 1, 1, 3,
               		1, 1, 1, 3, 1,
               		3, 3};

int btn1_press1[2] = { 4, 172 };

int btn2[32] = {  8, 1, 1, 3, 3,
               		    1, 3, 3, 1, 1,
               		    1, 1, 3, 1, 3,
              		    3, 1, 1, 3, 3,
               		    1, 3, 1, 3, 1,
               		    3, 1, 1, 3, 1,
               		    3, 1};
int btn2_press1[2] = { 4, 172 };

void sendToPC(char *hostIP, char *msg)
{
	 printf("%sRev IR and send message to server%s\n", KCYN, KNRM);
   	 SendMessage(hostIP, 5000, msg );
}


int main()
{

    	if (wiringPiSetup() == -1)
    	{
       		printf("fault:wiringPiSetup\n");
      		return 0;
    	}

 	char *IP = getIP("eth0");
   	printf("%seth0:%s%s%s\n", KMAG, KGRN, IP, KNRM);

    	if (strcmp(IP, "0.0.0.0") == 0 || strcmp(IP, "1.0.0.0") == 0 || strcmp(IP, "2.0.0.0") == 0) 
    	{
       		IP = getIP("wlan0");
       		printf("%swlan0:%s%s%s\n", KMAG, KGRN, IP, KNRM);
    	}

 	char *hostIP = StartBroadcastWithHost(IP, 8090);
    	printf("%sFind server:%s%s%s\n", KMAG, KGRN, hostIP, KNRM);

    	int in_invoke = 0;    
    	SendMessage(hostIP, 5000, "start from raspberrypi");

    	//--------------------------------------------------
   	 pinMode(GPIO_PIN_IN, INPUT);

  	clock_t start, stop;
   	start = clock();
    	stop = clock(); 
    	int i, j;
   	int count = -1;
    	int fBuf[64];
    	memset(fBuf, 0, sizeof(fBuf));

    	while (1)
   	{
        	start = clock();
         	float durDown = ((float)(start-stop)/CLOCKS_PER_SEC);
         	while (digitalRead(GPIO_PIN_IN) == 0)
         	{
               		start = clock();              
        	}
         	float durPulse = ((float)(start-stop)/CLOCKS_PER_SEC); 
         	while (digitalRead(GPIO_PIN_IN) == 1)
         	{
               		stop = clock();
        	 }
         	float durSpace = ((float)(stop-start)/CLOCKS_PER_SEC); 
	 
	 
         
        	int diPulse = durPulse*1000000 > 1000 ? 1 : 0;
        	int diSpace = (durSpace*1000000 + BASEHZ/2)/ BASEHZ;

		
 		if (diPulse == 1 && diSpace == 8)
		{
			count = 0;
		}
		
		if (count != -1)
		{
         		fBuf[count] = (int)diSpace;
         		count++;
         		count%=64;
			if (count == 32) 
                        {
				
                               	char szMsg[33];
                                memset(szMsg, 0, sizeof(szMsg));
				szMsg[32] = '\0';	
   				for (i=0; i<32; i++)
				{
					sprintf(szMsg, "%s%d", szMsg, fBuf[i]);
					//szMsg[i] = fBuf[i];
					//printf("%d ", fBuf[i]);
					//if (i % 5 == 0) printf("\n");
				}//end for
				//printf("\n");
				//printf("%s\n\n", szMsg);
				sendToPC(hostIP, szMsg);
				count = -1;				
				memset(fBuf, 0, sizeof(fBuf));				
			}// end if
		}// end if
   	}// end while

   	printf("End!\n");
    	return 0;
}

