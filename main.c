#include <stdio.h>
#include <time.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "soapH.h"

#define MULTICAST_ADDR "239.255.255.250"
#define MULTICAST_PORT 3702
#define LOCAL_ADDR "192.168.2.102"

char * CopyString(char *pSrc);
int CreateMulticast(void);
int SendHello(int socket);
int SendBye(int socket);

struct sockaddr_in gMSockAddr;
char pBuffer[10000]; // XML buffer 
int  vBufLen = 0; 

   
SOAP_NMAC struct Namespace namespaces[] =
{
   {"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL},
   {"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
   {"wsa2", "http://www.w3.org/2005/03/addressing", NULL, NULL},
   {"dis", "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01", NULL, NULL},
   {NULL, NULL, NULL, NULL}
};


int main(int argc, char **argv)
{
	int socket = 0;
	
	socket = CreateMulticast();
	
	SendHello(socket); usleep(500000);
	//SendHello(socket); usleep(500000);
	//SendHello(socket); usleep(500000);

	// Handle Probe and Resolve request
	
	//SendBye(socket); usleep(500000);
	//SendBye(socket); usleep(500000);
	//SendBye(socket); usleep(500000);	
}


// Utilities for gSOAP XML handle
char * CopyString(char *pSrc)
{
	int vLen = 0;
	char *pDst = NULL;
	
	if(!pSrc) return NULL;
		
	vLen = strlen(pSrc);
	pDst = malloc(vLen);	
	memset(pDst, 0, vLen);
	memcpy(pDst, pSrc, vLen);
	
	return pDst;
}

void clearBuffer(void)
{
	vBufLen = 0;
	memset(pBuffer, 0, sizeof(pBuffer));
}

int mysend(struct soap *soap, const char *s, size_t n) 
{ 
   if (vBufLen + n > sizeof(pBuffer)) 
      return SOAP_EOF; 
   strcpy(pBuffer + vBufLen, s); 
   vBufLen += n; 
   return SOAP_OK; 
} 


// Send Multicast Packet (Hello and Bye)
int SendHello(int socket)
{
	int vErr = 0;
	struct __wsdd__Hello *pWsdd__Hello = malloc(sizeof(struct __wsdd__Hello));
	struct wsdd__HelloType *pWsdd__HelloType = malloc(sizeof(struct wsdd__HelloType));	
	struct soap *pSoap=malloc(sizeof(struct soap));
	soap_init(pSoap);

#if 1	
	pSoap->fsend = mysend;
#else
  pSoap->recvfd = (int)stdin;
	pSoap->sendfd = (int)socket;
#endif
	pSoap->action = CopyString("http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/Hello");
	// urn:docs-oasis-open-org:ws- dd:ns:discovery:2009:01

	soap_default___wsdd__Hello(pSoap, pWsdd__Hello);
	
	pWsdd__Hello->wsdd__Hello = pWsdd__HelloType;   
	pWsdd__HelloType->wsa5__EndpointReference.Address = CopyString("urn:uuid:98190dc2-0890-4ef8-ac9a-5940995e6119");
	pWsdd__HelloType->Types = CopyString("tds:Device");
	pWsdd__HelloType->Scopes = malloc(sizeof(struct wsdd__ScopesType));
	pWsdd__HelloType->Scopes->MatchBy = CopyString("http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/rfc3986");; 
	pWsdd__HelloType->Scopes->__item = CopyString("onvif://www.onvif.org/Profile/Streaming");
	pWsdd__HelloType->XAddrs = CopyString("444");
	pWsdd__HelloType->MetadataVersion = 0;
				   
	vErr = soap_write___wsdd__Hello(pSoap, pWsdd__Hello); 

	//soap_serialize___wsdd__Hello(pSoap, pWsdd__Hello);
	//soap_out___wsdd__Hello(pSoap, const char*, int, pWsdd__Hello, const char*);

	printf("vErr=%d, Len=%d, Buf=\n%s\n", vErr, vBufLen, pBuffer);

	
#if 1	
	if(sendto(socket, pBuffer, vBufLen, 0, (struct sockaddr*)&gMSockAddr, sizeof(gMSockAddr)) < 0)
	{
		perror("Sending datagram message error");
	}
	else
	  printf("Sending datagram message...OK\n");	
#endif
	  
	clearBuffer();
	soap_destroy(pSoap);	  
	return SOAP_OK;
}

int SendBye(int socket)
{
	int vErr = 0;
	struct __wsdd__Bye *pWsdd__Bye = malloc(sizeof(struct __wsdd__Bye));
	struct wsdd__ByeType *pWsdd__ByeType = malloc(sizeof(struct wsdd__ByeType));	
	struct soap *pSoap=malloc(sizeof(struct soap));
	soap_init(pSoap);
	pSoap->fsend = mysend;

  //pSoap->recvfd = (int)stdin;
	//pSoap->sendfd = (int)socket;

	pWsdd__Bye->wsdd__Bye = pWsdd__ByeType;   
	pWsdd__ByeType->wsa5__EndpointReference.Address = CopyString("111");
	pWsdd__ByeType->Types = CopyString("222");
	pWsdd__ByeType->Scopes = malloc(sizeof(struct wsdd__ScopesType));
	pWsdd__ByeType->Scopes->MatchBy = NULL;
	pWsdd__ByeType->Scopes->__item = CopyString("333");
	pWsdd__ByeType->XAddrs = CopyString("444");
	pWsdd__ByeType->MetadataVersion = 0;
				   
	vErr = soap_write___wsdd__Bye(pSoap, pWsdd__Bye);
   
	printf("vErr=%d, Len=%d, Buf=\n%s\n", vErr, vBufLen, pBuffer);
	
	if(sendto(socket, pBuffer, vBufLen, 0, (struct sockaddr*)&gMSockAddr, sizeof(gMSockAddr)) < 0)
	{
		perror("Sending datagram message error");
	}
	else
	  printf("Sending datagram message...OK\n");	
	  
	clearBuffer();
	soap_destroy(pSoap);	  
	return SOAP_OK;
}

int CreateMulticast(void)
{
	// http://www.tenouk.com/Module41c.html
	struct in_addr localInterface;
	
	int sd;
	
	char databuf[1024] = "Multicast test message lol!";
	int datalen = sizeof(databuf);
	
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
	  perror("Opening datagram socket error");
	  exit(1);
	}
	else
	  printf("Opening the datagram socket...OK.\n");

	memset((char *) &gMSockAddr, 0, sizeof(gMSockAddr));
	gMSockAddr.sin_family = AF_INET;
	gMSockAddr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
	gMSockAddr.sin_port = htons(MULTICAST_PORT);
	  	
	localInterface.s_addr = inet_addr(LOCAL_ADDR);
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface.s_addr, sizeof(localInterface)) < 0)
	{
	  perror("Setting local interface error");
	  exit(1);
	}
	else
	  printf("Setting the local interface...OK\n");  	
	  
	return sd;
}



// Receive Multicast request and send unicast response (Probe and Resolve)
SOAP_FMAC5 int SOAP_FMAC6 __wsdd__ProbeMatches(struct soap *pSoap, struct wsdd__ProbeMatchesType *wsdd__ProbeMatches)
{return SOAP_FAULT;}

SOAP_FMAC5 int SOAP_FMAC6 __wsdd__ResolveMatches(struct soap *pSoap, struct wsdd__ResolveMatchesType *wsdd__ResolveMatches)
{return SOAP_FAULT;}


// Unused callback function
SOAP_FMAC5 int SOAP_FMAC6 SOAP_ENV__Fault(struct soap *pSoap, char *faultcode, char *faultstring, char *faultactor, struct SOAP_ENV__Detail *detail, struct SOAP_ENV__Code *SOAP_ENV__Code, struct SOAP_ENV__Reason *SOAP_ENV__Reason, char *SOAP_ENV__Node, char *SOAP_ENV__Role, struct SOAP_ENV__Detail *SOAP_ENV__Detail)
{return SOAP_FAULT;}

SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Hello(struct soap *pSoap, struct wsdd__HelloType *wsdd__Hello)
{return SOAP_FAULT;}

SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Bye(struct soap *pSoap, struct wsdd__ByeType *wsdd__Bye)
{return SOAP_FAULT;}

SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Probe(struct soap *pSoap, struct wsdd__ProbeType *wsdd__Probe)
{return SOAP_FAULT;}

SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Resolve(struct soap *pSoap, struct wsdd__ResolveType *wsdd__Resolve)
{return SOAP_FAULT;}