#include <nWork>

#ifdef WIN32
#include <time.h>
#include <windows.h>
#define MSLEEP(tn) Sleep(tn)
#define errno WSAGetLastError()
#endif

#ifdef MACX
#include <sys/time.h>
#include <time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <errno.h>
#define MSLEEP(tn) msleep(tn)
#endif

#ifdef LINUX
#include <netdb.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <errno.h>
#define MSLEEP(tn) msleep(tn)
#endif

#ifdef QT_SQL_LIB
nDnsRecord & nDnsRecord::operator=(QSqlQuery & query)
{
  Id          = query.value(0).toInt ();
  DnsIP       = QString::fromUtf8(query.value(1).toByteArray());
  CountryId   = query.value(2).toInt ();
  Rank        = query.value(3).toInt ();
  Connections = query.value(4).toInt ();
  ttl         = query.value(5).toUInt();
  success     = query.value(6).toInt ();
  failure     = query.value(7).toInt ();
  cachings    = query.value(8).toInt ();
  alterings   = query.value(9).toInt ();
  return *this;
};

#endif


#define GHTIMEOUT (30*1000)
#define GXTIMEOUT (3 *1000)
#define GNTIMEOUT 50

static unsigned long long mtime(void)
{
  #ifdef WIN32
  unsigned long long tt;
  QTime QT = QTime::currentTime();
  tt  = time(NULL);
  tt *= 1000;
  tt += QT.msec();
  return tt;
  #else
  struct timeval tv;
  unsigned long long tt = time(NULL);
  gettimeofday(&tv,NULL);
  tt *= 1000;
  tt += tv.tv_usec / 1000;
  return tt;
  #endif
};

static int isValidDomain(char * DOMAINNAME)
{
  char * p = DOMAINNAME;

  if (p==NULL) return 0;
  if (p[0]==0) return 0;

  /* check ascii */
  while ((*p)!=0) {
    if (!isascii((*p))) return 0;
    if ((*(unsigned char*)p)>127) return 0;
    p++;
  };
  p--;
  if ((*p)=='.') return 0;

  p = strstr(DOMAINNAME,"..");
  if (p!=NULL) return 0;

  p = DOMAINNAME;
  if (p[0]=='.') return 0;

  /* check invalid chars */
  p = DOMAINNAME;
  while ((*p)!=0) {
    if ((*p)=='*' ) return 0;
    if ((*p)==10  ) return 0;
    if ((*p)==12  ) return 0;
    if ((*p)==13  ) return 0;
    if ((*p)==27  ) return 0;
    if ((*p)=='%' ) return 0;
    if ((*p)=='\\') return 0;
    p++;
  };
  return 1;
};

/*********************************************
                  nDnsHost
 *********************************************/
nDnsHost::nDnsHost(char * hostname)
{
  HOSTNAME = NULL;
  IP.IP = 0;
  setHostname(hostname);
};

nDnsHost::nDnsHost(void)
{
  HOSTNAME = NULL;
};

nDnsHost::~nDnsHost(void)
{
  if (HOSTNAME!=NULL) delete [] HOSTNAME;
};

void nDnsHost::operator=(char * hostname)
{
  setHostname(hostname);
};

void nDnsHost::setHostname(char * hostname)
{
  struct hostent * hent;

  if (hostname        ==NULL) return;
  if (strlen(hostname)==0   ) return;
  hent = gethostbyname(hostname);
  if (hent!=NULL && hent->h_addr_list!=NULL && hent->h_addr_list[0]!=NULL) {
    unsigned char * p = (unsigned char *)hent->h_addr_list[0];
    if (p!=NULL) IP.setIpAddress(p);
  };

  if (HOSTNAME!=NULL) delete [] HOSTNAME;
  HOSTNAME = strdup(hostname);
};

/*********************************************
                    nDns
 *********************************************/
#define DNS_HDR_LEN      12
#define DNS_FLAG_QR      0x8000
#define DNS_FLAG_AA      0x0400
#define DNS_FLAG_TC      0x0200
#define DNS_FLAG_RD      0x0100
#define DNS_FLAG_RA      0x0080
#define DNS_RCODE_MASK   0x000F
#define DNS_OPCODE_MASK  0x7800

#define DNS_RRTYPE_A      1
#define DNS_RRTYPE_NS     2
#define DNS_RRTYPE_CNAME  5
#define DNS_RRTYPE_SOA    6
#define DNS_RRTYPE_WKS   11
#define DNS_RRTYPE_PTR   12
#define DNS_RRTYPE_HINFO 13
#define DNS_RRTYPE_MX    15

#define DNS_RRCLASS_IN    1
#define DNS_RRCLASS_CS    2
#define DNS_RRCLASS_CH    3
#define DNS_RRCLASS_HS    4

#define QFIXEDSZ          4
#define INDIR_MASK        0xC0

typedef struct {
  unsigned short dns_id;         /* client query ID number */
  unsigned short dns_flags;      /* qualify contents <see below> */
  unsigned short dns_q_count;    /* number of questions */
  unsigned short dns_rr_count;   /* number of answer RRs */
  unsigned short dns_auth_count; /* number of authority RRs  */
  unsigned short dns_add_count;  /* number of additional RRs */
} DNSHEADER;

nDns::nDns    (QObject * parent)
    : QObject (          parent)
{
  firstDns     = true;
  working      = false;
  startUdp     = mtime();
  stop_ns      = false;
  isReading    = false;
  Mode         = DNS_RRTYPE_MX;
  State        =  0;
  CNAME[0]     =  0;
  IncomingData =  0;
  OutgoingData =  0;
  crlf         = false;
  caching      = true ;
  dns_socket   = -1;
  socket_flags =  0;
  datasize     =  0;
  state        =  0;
  Id           = -1;
  IpUsed       . clear();
  dnsIp        . clear();
  MXHOST       . clear();
  MX           . clear();
  dnsStart     = mtime();
  dnsConnected = mtime();
  dnsAnswer    = mtime();
};

nDns::~nDns(void)
{
  IpUsed . clear();
  dnsIp  . clear();
  MXHOST . clear();
  MX     . clear();
  closeSocket();
};

bool nDns::clear(void)
{
  working      = false         ;
  IpUsed       . clear()       ;
  dnsIp        . clear()       ;
  MXHOST       . clear()       ;
  MX           . clear()       ;
  closeSocket();
  startUdp     = mtime()       ;
  stop_ns      = false         ;
  isReading    = false         ;
  Mode         = DNS_RRTYPE_MX ;
  State        = 0             ;
  CNAME[0]     = 0             ;
  IncomingData = 0             ;
  OutgoingData = 0             ;
  crlf         = false         ;
  dns_socket   = -1            ;
  socket_flags =  0            ;
  datasize     =  0            ;
  state        =  0            ;
  return true;
};

unsigned long long nDns::TTC(void)
{
  return dnsConnected - dnsStart;
};

unsigned long long nDns::TTQ(void)
{
  return dnsAnswer - dnsStart;
};

bool nDns::isAddressQuery(void)
{
  return (DNS_RRTYPE_A==Mode);
};

bool nDns::QueryHost(void)
{
  State    = 101;
  goNext   = false;
  stop_ns  = false;
  MXHOST   . clear();
  
  memset(ERR      , 0,8192);
  memset(RRNAME   , 0, 512);
  memset(CNAME    , 0, 512);
  memset(HOSTNAME , 0,1024);
  
  strcpy(HOSTNAME , (char *)(const char *)MXTARGET.toLower().toUtf8());
  setupTime();
  
  if (!isValidDomain(HOSTNAME)) return false;

  return true;
};

static int hostLookup = 0;
static int hostDone = 0;
#define MAXLOOKUP 512

QDateTime hostStart;

void nDns::setHost(QString NSHOST)
{
  caching = false;
  while (hostLookup>MAXLOOKUP) {
    qApp->processEvents();
    #ifdef WIN32
    Sleep(10);
    #else
    usleep(10*1000);
    #endif
  };
  hostLookup++;
  QHostInfo::lookupHost(NSHOST.toLower(),this,SLOT(HostInfo(QHostInfo)));
};

void nDns::setNs(QString NSHOST)
{
  caching = false;
  while (hostLookup>MAXLOOKUP) {
    qApp->processEvents();
    #ifdef WIN32
    Sleep(10);
    #else
    usleep(10*1000);
    #endif
  };
  hostLookup++;
  QHostInfo::lookupHost(NSHOST.toLower(),this,SLOT(NsInfo(QHostInfo)));
};

void nDns::HostInfo(QHostInfo HI)
{
  QList<QHostAddress> HAL = HI.addresses();
  QHostAddress HA;
  
  hostDone++;
  int dt = hostStart.secsTo(QDateTime::currentDateTime());
  printf(
    "HostLookup: %d , %d , %3.3f\n",
    hostLookup,
    hostDone,
    ((float)hostDone)/((float)dt)
  );
  
  hostLookup--;
  
  if (HI.error()==QHostInfo::NoError && HAL.count()>0) {
    if (!HAL[0].isNull()) {
      Host = HAL[0];
      char ADDR[512];
      strcpy(ADDR,(char *)(const char *)Host.toString().toUtf8());
      if (firstDns) dnsStart = mtime();
      connectToHost(ADDR,53);
      State = 103;
      setupTime();
    };
  } else {
    State = 102;
    if (MXHOST.count()==0) stop_ns = true;
  };
};

void nDns::NsInfo(QHostInfo HI)
{
  QList<QHostAddress> HAL = HI.addresses();
  QHostAddress HA;
  
  hostDone++;
  int dt = hostStart.secsTo(QDateTime::currentDateTime());
  printf(
    "HostLookup: %d , %d , %3.3f\n",
    hostLookup,
    hostDone,
    ((float)hostDone)/((float)dt)
  );
  
  hostLookup--;
  
  if (HI.error()==QHostInfo::NoError && HAL.count()>0) {
    if (!HAL[0].isNull()) {
      Host = HAL[0];
      char ADDR[512];
      strcpy(ADDR,(char *)(const char *)Host.toString().toUtf8());
      if (firstDns) dnsStart = mtime();
      connectToHost(ADDR,53);
      State = 103;
      setupTime();
    };
  } else {
    State = 102;
    if (MXHOST.count()==0) stop_ns = true;
  };
};

void nDns::setupTime(void)
{
  time_out = mtime();
};

bool nDns::timeout(int ms)
{
  return ((time_out+ms)<=mtime());
};

int nDns::PutQ(char * HOST,char * QDATA)
{
  int i;
  int j = 0;
  int k = 0;

  for (i=0; *(HOST+i);i++)  {
    char c = *(HOST+i);
    if (c=='.')  {
      *(QDATA+j) = k;
      k = 0;
      j = i + 1;
    } else {
      *(QDATA+i+1) = c;
      k++;
    }
  }

  *(QDATA+j)   = k;
  *(QDATA+i+1) = 0;

  return i+1;
};

unsigned short nDns::getshort(char *msgp)
{
  register unsigned char *p = (unsigned char *)msgp;
  register unsigned short u;

  u = *p++ << 8;

  return ((unsigned short)(u | *p));
}

unsigned long nDns::getlong(char   *msgp)
{
  register unsigned char *p = (unsigned char   *)msgp;
  register unsigned long u;
  
  u  = *p++; u <<= 8;
  u |= *p++; u <<= 8;
  u |= *p++; u <<= 8;
  
  return (u | *p);
}

int nDns::DnExpand(char *msg,char *eomorig,char *comp_dn,char *exp_dn,int length)
{
  char * cp           ;
  char * dn           ;
  char * eom          ;
  int    n            ;
  int    c            ;
  int    len     = -1 ;
  int    checked =  0 ;

  dn  = exp_dn              ;
  cp  = comp_dn             ;
  eom = exp_dn + length - 1 ;
  
  while ((n=*cp++)!=0) {
    switch   (n   &   INDIR_MASK)   {
      case   0:
        if (dn   !=   exp_dn)   {
          if   (dn   >=   eom) return (-1);
          *dn++   =   '.';
        }
        if (dn+n>=eom) return   (-1);
        checked   +=   n   +   1;
        while (--n >= 0)   {
          if ((c = *cp++) == '.')   {
            if   (dn+n+1   >=   eom) return (-1);
            *dn++   =   '\\';
          }
          *dn++   =   c;
          if   (cp   >=   eomorig) return(-1);
        }
      break;
      case   INDIR_MASK:
        if (len < 0)  len = cp - comp_dn + 1;
        cp   =   msg   +   (((n   &   0x3f)   <<   8)   |   (*cp   &   0xff));
        if   (cp < msg || cp >= eomorig) return(-1);
        checked   +=   2;
        if (checked >= eomorig - msg) return (-1);
      break;
      default:
      return (-1);
    }
  }
  *dn = '\0';
  if (len < 0) len = cp - comp_dn;
  return (len);
};

int nDns::dnsPacket(char * buffer,char * hostname)
{
  int         LEN;
  DNSHEADER * pDNS;

  pDNS = (DNSHEADER *)buffer;

  pDNS->dns_id         = htons ( 0xDEAD      );
  pDNS->dns_flags      = htons ( DNS_FLAG_RD );
  pDNS->dns_q_count    = htons (      1      );
  pDNS->dns_rr_count   = 0;
  pDNS->dns_auth_count = 0;
  pDNS->dns_add_count  = 0;

  LEN = PutQ(hostname,&buffer[DNS_HDR_LEN]);
  LEN += DNS_HDR_LEN;

  buffer[LEN++] = 0;
  buffer[LEN++] = 0;
  buffer[LEN  ] = DNS_RRTYPE_MX;
  buffer[LEN+1] = 0;
  buffer[LEN+2] = DNS_RRCLASS_IN;
  buffer[LEN+3] = 0;
  LEN += 4;

  return LEN;
};

int nDns::dnsPacket(char * buffer,char * hostname,int dnstype)
{
  int         LEN;
  DNSHEADER * pDNS;

  pDNS = (DNSHEADER *)buffer;

  pDNS->dns_id         = htons(0xDEAD);
  pDNS->dns_flags      = htons(DNS_FLAG_AA);
  pDNS->dns_q_count    = htons(   1   );
  pDNS->dns_rr_count   = 0;
  pDNS->dns_auth_count = 0;
  pDNS->dns_add_count  = 0;

  LEN = PutQ(hostname,&buffer[DNS_HDR_LEN]);
  LEN += DNS_HDR_LEN;

  buffer[LEN++] = 0;
  buffer[LEN++] = 0;
  buffer[LEN  ] = dnstype;
  buffer[LEN+1] = 0;
  buffer[LEN+2] = DNS_RRCLASS_IN;
  buffer[LEN+3] = 0;
  LEN += 4;

  return LEN;
};

bool nDns::ParseMX(void)
{
  int             LEN = 0;
  int             CNT = 0;
  int             QCNT,RRCNT,ARCNT,ADCNT,i;
  unsigned short  DEAD;
  DNSHEADER     * pDNS;
  unsigned int  * p;
  char          * pp;
  char          * np;
  char          * eom;
  char            name[1024];
  char          * RESULT = ERR;

  MX.clear();
  if (dataLen<12) return false;

  pDNS  = (DNSHEADER *)RESULT;
  CNT   = *(int *)&RESULT[DNS_HDR_LEN];
  p     = (unsigned int *)&RESULT[DNS_HDR_LEN];
  pp    = (char         *)&RESULT[DNS_HDR_LEN];

  LEN   = DNS_HDR_LEN;
  if (LEN>dataLen) return false;
  DEAD  = ntohs(pDNS->dns_id        );
  QCNT  = ntohs(pDNS->dns_q_count   );
  RRCNT = ntohs(pDNS->dns_rr_count  );
  ARCNT = ntohs(pDNS->dns_auth_count);
  ADCNT = ntohs(pDNS->dns_add_count );

  if (DEAD!=0xDEAD) return false;
  if (RRCNT==0 && ARCNT==0 && ADCNT==0) return false;
  if (QCNT==0) return false;
  
  memset(name,0,1024);
  np  = name;
  eom = (char*)&RESULT[dataLen];
  for (i=0;i<QCNT;i++) {
    int N;
    N = DnExpand((char *)RESULT,eom,(char *)pp,(char *)name,127);
    if  (N<0) return false;
    pp  += N + QFIXEDSZ;
    LEN += N + QFIXEDSZ;
    if (LEN>dataLen) return false;
    if (i==0) strcpy(RRNAME,name);
  }

  CNAME[0] = 0;

  if (RRCNT>0) {
    int ii,rtype,rclass,pref;
    for (i=0,ii=0;i<RRCNT;i++) {
      int N,j,rdLength;
      if ((N=DnExpand(RESULT,eom,pp,name,127))<0) return false;
      LEN+= N;
      if (LEN>dataLen) return false;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN+= 10;
      if (LEN>dataLen) return false;
      switch (rtype) {
        case DNS_RRTYPE_CNAME:
          N    = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && strlen(CNAME)==0) strcpy(CNAME,name);
          LEN += N;
          if (LEN>dataLen) return 0;
        break;
        case DNS_RRTYPE_MX:
          pref = getshort(pp); pp  += 2;
          LEN += 2;
          if (LEN>dataLen) return false;
          N = DnExpand(RESULT,eom,pp,name,127);
          if (N>0) {
            if (isValidDomain(name)) {
			  QString NA = name;
			  MX << NA.toLower();
			};
            pp  += N;
            LEN += N;
            if (LEN>dataLen) return false;
          };
        break;
        case DNS_RRTYPE_NS:
          N = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && !goNext && isValidDomain(name)) {
            QString NA = name;
		    MXHOST << NA.toLower();
          };
		  LEN += N;
          if (LEN>dataLen) return false;
        break;
        default:
          pp += rdLength;
          LEN+= rdLength;
          if (LEN>dataLen) return false;
      };
    }
  };

  if (MX.count()>0) return true;

  if (ARCNT>0) {
    int ii,rtype,rclass,pref;
    for (i=0,ii=0;i<ARCNT;i++) {
      int N,j,rdLength;
      if ((N=DnExpand(RESULT,eom,pp,name,127))<0) return false;
      LEN+= N;
      if (LEN>dataLen) return false;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN+= 10;
      if (LEN>dataLen) return false;
      switch (rtype) {
        case DNS_RRTYPE_MX:
          pref = getshort(pp); pp  += 2;
          LEN += 2;
          if (LEN>dataLen) return false;
          N = DnExpand(RESULT,eom,pp,name,127);
          if (N>0) {
            if (isValidDomain(name)) {
			  QString NA = name;
			  MX << NA.toLower();
            };
			pp  += N;
            LEN += N;
            if (LEN>dataLen) return false;
          };
        break;
        case DNS_RRTYPE_NS:
          N = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && !goNext && isValidDomain(name)) {
		    QString NA = name;
		    MXHOST << NA.toLower();
          };
		  LEN += N;
          if (LEN>dataLen) return false;
        break;
        default:
          pp += rdLength;
          LEN+= rdLength;
          if (LEN>dataLen) return false;
      };
    }
  };

  if (MX.count()>0) return true;

  if (ADCNT>0) {
    int ii,rtype,rclass,pref;
    for (i=0,ii=0;i<ADCNT;i++) {
      int N,j,rdLength;
      if ((N=DnExpand(RESULT,eom,pp,name,127))<0) return false;
      LEN+= N;
      if (LEN>dataLen) return false;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN+= 10;
      if (LEN>dataLen) return false;
      switch (rtype) {
        case DNS_RRTYPE_MX:
          pref = getshort(pp); pp  += 2;
          LEN += 2;
          if (LEN>dataLen) return false;
          N = DnExpand(RESULT,eom,pp,name,127);
          if (N>0) {
            if (isValidDomain(name)) {
			  QString NA = name;
			  MX << NA.toLower();
            };
            pp  += N;
            LEN += N;
            if (LEN>dataLen) return false;
          };
        break;
        case DNS_RRTYPE_NS:
          N = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && !goNext && isValidDomain(name)) {
		    QString NA = name;
		    MXHOST << NA.toLower();
          };
          LEN += N;
          if (LEN>dataLen) return false;
        break;
        default:
          pp += rdLength;
          LEN+= rdLength;
          if (LEN>dataLen) return false;
      };
    }
  };

  return (MX.count()>0);
};

bool nDns::ParseA(void)
{
  int                LEN = 0;
  int                CNT = 0;
  int                QCNT,RRCNT,ARCNT,ADCNT,i;
  int                AA = 0;
  unsigned short     DEAD;
  DNSHEADER        * pDNS;
  unsigned int     * p;
  char             * pp;
  char             * RESULT = ERR;
  char             * np;
  char             * eom;
  char               name[1024];
  unsigned char      ipaddress[4];
  unsigned long long ipa;
  QString            IP;

  CNAME[0] = 0;

  if (dataLen<=12) return false;

  pDNS  = (DNSHEADER *)RESULT;
  CNT   = *(int *)&RESULT[DNS_HDR_LEN];
  p     = (unsigned int *)&RESULT[DNS_HDR_LEN];
  pp    = (char         *)&RESULT[DNS_HDR_LEN];

  LEN   = DNS_HDR_LEN;
  if (LEN>dataLen) return false;
  DEAD  = ntohs(pDNS->dns_id);
  QCNT  = ntohs(pDNS->dns_q_count   );
  RRCNT = ntohs(pDNS->dns_rr_count  );
  ARCNT = ntohs(pDNS->dns_auth_count);
  ADCNT = ntohs(pDNS->dns_add_count );

  if (DEAD!=0xDEAD) return false;
  if (RRCNT==0 && ARCNT==0 && ADCNT==0) return false;
  if (QCNT==0) return false;

  memset(name,0,1024);
  np  = name;
  eom = (char*)&RESULT[dataLen];
  for (i=0;i<QCNT;i++) {
    int N;
    N = DnExpand((char *)RESULT,eom,(char *)pp,(char *)name,127);
    if  (N<0) return false;
    pp  += N + QFIXEDSZ;
    LEN += N + QFIXEDSZ;
    if (LEN>dataLen) return false;
    if (i==0) strcpy(RRNAME,name);
  }

  if (RRCNT>0) {
    int ii,rtype,rclass;
    for (i=0,ii=0;i<RRCNT;i++) {
      int N,j,rdLength;
      if ((N=DnExpand(RESULT,eom,pp,name,127))<0) return false;
      LEN += N;
      if (LEN>dataLen) return false;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN+= 10;
      if (LEN>dataLen) return false;
      switch (rtype) {
        case DNS_RRTYPE_A:
          ipa = getlong(pp);
          memcpy(ipaddress,&ipa,4);
          LEN   += 4;
          IP.sprintf("%d.%d.%d.%d",ipaddress[3],ipaddress[2],ipaddress[1],ipaddress[0]);
          MX << IP;
          AA++;
          if (LEN>dataLen) return false;
        break;
        case DNS_RRTYPE_NS:
          N = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && !goNext && isValidDomain(name)) {
            QString NA = name;
            MXHOST << NA.toLower();
          };
          LEN += N;
          if (LEN>dataLen) return false;
        break;
        case DNS_RRTYPE_CNAME:
          N = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && strlen(CNAME)==0) strcpy(CNAME,name);
          LEN += N;
          if (LEN>dataLen) return false;
        break;
      };
      pp += rdLength;
    }
  };

  if (MX.count()>0) return true;

  if (ARCNT>0) {
    int ii,rtype,rclass;
    for (i=0,ii=0;i<ARCNT;i++) {
      int N,j,rdLength;
      if ((N=DnExpand(RESULT,eom,pp,name,127))<0) return false;
      LEN     += N;
      if (LEN>dataLen) return false;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN     += 10;
      if (LEN>dataLen) return false;
      switch (rtype) {
        case DNS_RRTYPE_NS:
          N    = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && !goNext && isValidDomain(name)) {
            QString NA = name;
		    MXHOST << NA.toLower();
          };
          LEN += N;
          if (LEN>dataLen) return false;
        break;
        case DNS_RRTYPE_CNAME:
          N    = DnExpand(RESULT,eom,pp,name,127);
          if (N>0) strcpy(CNAME,name);
          LEN += N;
          if (LEN>dataLen) return false;
        break;
      };
      pp += rdLength;
    }
  };

  if (MX.count()>0) return true;

  if (ADCNT>0) {
    int ii,rtype,rclass;
    for (i=0,ii=0;i<ADCNT;i++) {
      int N,j,rdLength;
      if ((N=DnExpand(RESULT,eom,pp,name,127))<0) return false;
      LEN     += N;
      if (LEN>dataLen) return false;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN     += 10;
      if (LEN>dataLen) return false;
      switch (rtype) {
        case DNS_RRTYPE_NS:
          N    = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && !goNext && isValidDomain(name)) {
            QString NA = name;
		    MXHOST << NA.toLower();
          };
          LEN += N;
          if (LEN>dataLen) return false;
        break;
        case DNS_RRTYPE_CNAME:
          N    = DnExpand(RESULT,eom,pp,name,127);
          if (N>0) strcpy(CNAME,name);
          LEN += N;
          if (LEN>dataLen) return false;
        break;
      };
      pp += rdLength;
    }
  };

  if (MX.count()>0) return true;
  if (AA==0 && strlen(CNAME)>0) AA=-1;

  return (AA>0);
};

bool nDns::ParseNS(void)
{
  int                LEN = 0;
  int                CNT = 0;
  int                QCNT,RRCNT,ARCNT,ADCNT,i;
  int                AA = 0;
  unsigned short     DEAD;
  DNSHEADER        * pDNS;
  unsigned int     * p;
  char             * pp;
  char             * RESULT = ERR;
  char             * np;
  char             * eom;
  char               name[1024];
  unsigned char      ipaddress[4];
  unsigned long long ipa;
  QString            IP;

  CNAME[0] = 0;

  if (dataLen<=12) return false;

  pDNS  = (DNSHEADER *)RESULT;
  CNT   = *(int *)&RESULT[DNS_HDR_LEN];
  p     = (unsigned int *)&RESULT[DNS_HDR_LEN];
  pp    = (char         *)&RESULT[DNS_HDR_LEN];

  LEN   = DNS_HDR_LEN;
  if (LEN>dataLen) return false;
  DEAD  = ntohs(pDNS->dns_id);
  QCNT  = ntohs(pDNS->dns_q_count   );
  RRCNT = ntohs(pDNS->dns_rr_count  );
  ARCNT = ntohs(pDNS->dns_auth_count);
  ADCNT = ntohs(pDNS->dns_add_count );

  if (DEAD!=0xDEAD) return false;
  if (RRCNT==0 && ARCNT==0 && ADCNT==0) return false;
  if (QCNT==0) return false;
  
  memset(name,0,1024);
  np  = name;
  eom = (char*)&RESULT[dataLen];
  for (i=0;i<QCNT;i++) {
    int N;
    N = DnExpand((char *)RESULT,eom,(char *)pp,(char *)name,127);
    if  (N<0) return false;
    pp  += N + QFIXEDSZ;
    LEN += N + QFIXEDSZ;
    if (LEN>dataLen) return false;
    if (i==0) strcpy(RRNAME,name);
  }

  if (RRCNT>0) {
    int ii,rtype,rclass;
    for (i=0,ii=0;i<RRCNT;i++) {
      int N,j,rdLength;
      if ((N=DnExpand(RESULT,eom,pp,name,127))<0) return false;
      LEN += N;
      if (LEN>dataLen) return false;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN+= 10;
      if (LEN>dataLen) return false;
      switch (rtype) {
        case DNS_RRTYPE_A:
          ipa = getlong(pp);
          memcpy(ipaddress,&ipa,4);
          LEN   += 4;
          IP.sprintf("%d.%d.%d.%d",ipaddress[3],ipaddress[2],ipaddress[1],ipaddress[0]);
          MX << IP;
          AA++;
          if (LEN>dataLen) return false;
        break;
        case DNS_RRTYPE_NS:
          N = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && !goNext && isValidDomain(name)) {
            QString NA = name;
            MXHOST << NA.toLower();
          };
          LEN += N;
          if (LEN>dataLen) return false;
        break;
        case DNS_RRTYPE_CNAME:
          N = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && strlen(CNAME)==0) strcpy(CNAME,name);
          LEN += N;
          if (LEN>dataLen) return false;
        break;
      };
      pp += rdLength;
    }
  };

  if (MXHOST.count()>0) return true;

  if (ARCNT>0) {
    int ii,rtype,rclass;
    for (i=0,ii=0;i<ARCNT;i++) {
      int N,j,rdLength;
      if ((N=DnExpand(RESULT,eom,pp,name,127))<0) return false;
      LEN     += N;
      if (LEN>dataLen) return false;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN     += 10;
      if (LEN>dataLen) return false;
      switch (rtype) {
        case DNS_RRTYPE_NS:
          N    = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && !goNext && isValidDomain(name)) {
            QString NA = name;
		    MXHOST << NA.toLower();
          };
          LEN += N;
          if (LEN>dataLen) return false;
        break;
        case DNS_RRTYPE_CNAME:
          N    = DnExpand(RESULT,eom,pp,name,127);
          if (N>0) strcpy(CNAME,name);
          LEN += N;
          if (LEN>dataLen) return false;
        break;
      };
      pp += rdLength;
    }
  };

  if (MXHOST.count()>0) return true;

  if (ADCNT>0) {
    int ii,rtype,rclass;
    for (i=0,ii=0;i<ADCNT;i++) {
      int N,j,rdLength;
      if ((N=DnExpand(RESULT,eom,pp,name,127))<0) return false;
      LEN     += N;
      if (LEN>dataLen) return false;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN     += 10;
      if (LEN>dataLen) return false;
      switch (rtype) {
        case DNS_RRTYPE_NS:
          N    = DnExpand(RESULT,eom,pp,name,127);
          if (N>0 && !goNext && isValidDomain(name)) {
            QString NA = name;
		    MXHOST << NA.toLower();
          };
          LEN += N;
          if (LEN>dataLen) return false;
        break;
        case DNS_RRTYPE_CNAME:
          N    = DnExpand(RESULT,eom,pp,name,127);
          if (N>0) strcpy(CNAME,name);
          LEN += N;
          if (LEN>dataLen) return false;
        break;
      };
      pp += rdLength;
    }
  };

  if (MXHOST.count()>0) return true;
  if (AA==0 && strlen(CNAME)>0) AA=-1;

  return (MXHOST.count()>0);
};

bool nDns::isMore(void)
{
  int             LEN = 0;
  int             QCNT,RRCNT,ARCNT,ADCNT,i;
  unsigned short  DEAD;
  DNSHEADER     * pDNS;
  unsigned int  * p;
  char          * pp;
  char          * np;
  char          * eom;
  char          * RESULT = ERR;
  char            name [1024];
  
  if (dataLen<DNS_HDR_LEN) return true;

  pDNS  = (DNSHEADER    *)  RESULT;
  p     = (unsigned int *)& RESULT [ DNS_HDR_LEN ];
  pp    = (char         *)& RESULT [ DNS_HDR_LEN ];
  LEN   = DNS_HDR_LEN;
  DEAD  = ntohs ( pDNS->dns_id         );
  QCNT  = ntohs ( pDNS->dns_q_count    );
  RRCNT = ntohs ( pDNS->dns_rr_count   );
  ARCNT = ntohs ( pDNS->dns_auth_count );
  ADCNT = ntohs ( pDNS->dns_add_count  );

  if (DEAD!=0xDEAD) return false;
  if (RRCNT==0 && ARCNT==0 && ADCNT==0) return false;
  if (0==QCNT) return false;

  memset(name,0,1024);
  np  = name;
  eom = (char*)&RESULT[dataLen];

  for (i=0;i<QCNT;i++) {
    int N;
    N = DnExpand((char *)RESULT,eom,(char *)pp,(char *)name,511);
    if (N<0) return true;
    pp  += (N + QFIXEDSZ);
    LEN += (N + QFIXEDSZ);
    if (LEN>dataLen) return true;
  }

  if (RRCNT>0) {
    int ii,rtype,rclass;
    for (i=0,ii=0;i<RRCNT;i++) {
      int N,j,rdLength;
      N        = DnExpand(RESULT,eom,pp,name,127);
      if (N<0) return true;
      LEN     += N;
      if (LEN>dataLen) return true;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN     += 10;
      if (LEN>dataLen) return true;
      LEN     += rdLength;
      if (LEN>dataLen) return true;
      pp += rdLength;
    }
  };

  if (ARCNT>0) {
    int ii,rtype,rclass;
    for (i=0,ii=0;i<ARCNT;i++) {
      int N,j,rdLength;
      N        = DnExpand(RESULT,eom,pp,name,511);
      if (N<0) return true;
      LEN     += N;
      if (LEN>dataLen) return true;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN     += 10;
      if (LEN>dataLen) return true;
      LEN     += rdLength;
      if (LEN>dataLen) return true;
      pp      += rdLength;
    }
  };

  if (ADCNT>0) {
    int ii,rtype,rclass;
    for (i=0,ii=0;i<ADCNT;i++) {
      int N,j,rdLength;
      N        = DnExpand(RESULT,eom,pp,name,511);
      if (N<0) return true;
      LEN     += N;
      if (LEN>dataLen) return true;
      pp      += N;
      rtype    = getshort(pp); pp+=2; /* Type  */
      rclass   = getshort(pp); pp+=2; /* Class */
      j        = getlong (pp); pp+=4; /* TTL   */
      rdLength = getshort(pp); pp+=2; /* RDLENGTH */
      LEN     += 10;
      if (LEN>dataLen) return true;
      LEN     += rdLength;
      if (LEN>dataLen) return true;
      pp += rdLength;
    }
  };

  return false;
};

void nDns::addIncoming(unsigned long long len)
{
  IncomingData += len;
};

void nDns::addOutgoing(unsigned long long len)
{
  OutgoingData += len;
};

bool nDns::connectToHost(char * MMXX,int Port)
{
  struct sockaddr_in dest;
  char               ADDR[256];
  int                rtcode;

  nDnsHost HOST (MMXX);
  
  if (!HOST.IP.isValid()) return false;

  state    = 0;
  datasize = 0;
  HOST.IP.toString(ADDR);
  
  memset(&dest,0,sizeof(dest));
  dest.sin_family = PF_INET;
  dest.sin_port   = htons(Port);
  #ifdef WIN32
  unsigned long IA = inet_addr(ADDR);
  memcpy(&dest.sin_addr,&IA,sizeof(unsigned long));
  #else
  inet_aton(ADDR,&dest.sin_addr);
  #endif

  #ifdef WIN32
  unsigned long nNonBlocking = 1;
  if (ioctlsocket(dns_socket,FIONBIO,&nNonBlocking)==SOCKET_ERROR) return false;
  rtcode = ::connect(dns_socket, (struct sockaddr*)&dest, sizeof(dest));
  if (rtcode<0) {
//    if (errno!=EINPROGRESS) return false;
  };
  #else
  socket_flags = ::fcntl(dns_socket, F_GETFL, 0);
  if (socket_flags < 0) return false;
  if (::fcntl(dns_socket, F_SETFL, socket_flags | O_NONBLOCK) < 0) return false;
  rtcode = ::connect(dns_socket, (struct sockaddr*)&dest, sizeof(dest));
  if (rtcode<0) {
    if (errno!=EINPROGRESS) return false;
  };
  #endif

  return true;
};

int nDns::connectState(void)
{
  fd_set         rd,wr;
  struct timeval tv;
  int            rtcode;
  int            err;
  #ifdef WIN32
  #else
  socklen_t      len;
  #endif

  if (dns_socket<0) return 103;

  FD_ZERO(&rd);
  FD_ZERO(&wr);
  FD_SET(dns_socket,&rd);
  FD_SET(dns_socket,&wr);

  tv.tv_sec  = 0;
  tv.tv_usec = 200;

  rtcode = select(dns_socket+1, &rd, &wr, NULL, &tv);

  if (rtcode  < 0) return false; else
  if (rtcode == 0) {
    return 103;
  } else {
    if (FD_ISSET (dns_socket,&rd) || FD_ISSET(dns_socket,&wr)) {
      #ifdef WIN32
      unsigned long nNonBlocking = 0;
      if (ioctlsocket(dns_socket,FIONBIO,&nNonBlocking)==SOCKET_ERROR) return 102;
      #else
      len = sizeof(err);
      if (::getsockopt(dns_socket,SOL_SOCKET,SO_ERROR,&err,&len)<0) return false;
      if (::fcntl(dns_socket, F_SETFL, socket_flags)<0) return 102;
      #endif
    } else {
      return 102;
    };
  };

  state    = 1;
  datasize = 0;

  return 107;
};

bool nDns::createSocket(void)
{
  dns_socket = socket(PF_INET,SOCK_DGRAM,0);
  if (dns_socket<0) {
    printf("Create Socket Error\n");
    return false;
  };
  return true;
};

bool nDns::closeSocket(void)
{
  bool cs = false;
  firstDns = false;
  if (dns_socket<0) return false;
  #ifdef WIN32
  cs = ( ::closesocket(dns_socket) <0 )  ? false : true;
  #else
  cs = ( ::close(dns_socket) <0 )  ? false : true;
  #endif
  dns_socket = -1;
  state      =  0;
  return cs;
};

bool nDns::isValid(void)
{
  return (dns_socket>=0);
};

int nDns::Write(char * DATA,int len)
{
  int rr;
  if (dns_socket<0) return -1;
  #ifdef WIN32
  rr = ::send(dns_socket,DATA,len,0);
  #else
  rr = ::send(dns_socket,DATA,len,MSG_DONTWAIT);
  #endif
  return rr;
};

int nDns::Read(char * DATA,int len)
{
  if (dns_socket<0) return -1;
  #ifdef WIN32
  return ::recv(dns_socket,DATA,len,0);
  #else
  return ::recv(dns_socket,DATA,len,MSG_DONTWAIT);
  #endif
//  return ::recv(dns_socket,DATA,len,0);
};

bool nDns::uForReadyRead(unsigned long long TIMEOUT)
{
  fd_set         rd;
  struct timeval tv;
  int            rtcode;
  
  FD_ZERO (&rd);
  FD_SET  (dns_socket,&rd);

  tv.tv_sec  = TIMEOUT/1000000;
  tv.tv_usec = TIMEOUT%1000000;

  rtcode = select(dns_socket+1, &rd, NULL, NULL, &tv);
  if (rtcode  < 0) return false; else
  if (rtcode == 0) return false; else {
    if (!FD_ISSET (dns_socket,&rd)) return false;
  };

  return true;
};

bool nDns::RUN(void)
{
  if (State>0 && (mtime()-startUdp)>(1000*60*6)) State = 999;

  bool    parseMX ;
  int     nLength ;
  int     mark    ;
  QString MSG     ;
  
//  printf("DNS[%s,%8d,%4d]=%4d\n",(Mode==DNS_RRTYPE_A) ? "Address":"MxTable",Id,DnsId,State);
  
  switch (State) {
    case   0:
      startUdp = mtime();
      setupTime();
      if (QueryHost() && dnsIp.count()>0) {
        int xid = rand()%dnsIp.count();
        IpUsed << dnsIp[xid];
        Host.setAddress(dnsIp[xid]);
      } else
        return false;
    break;
    case 101:
//      if (!timeout(200)) break;
//      if (!timeout(100)) break;
//      if (!timeout(300)) break;
      createSocket();
      State = 102;
    break;
    case 102:
      if (!isValid()) State = 101; else
      if (MXHOST.count()>0) {
          switch (Mode) {
            case DNS_RRTYPE_A     : State = 802; break;
//            case DNS_RRTYPE_NS    : State = 401; break;
//            case DNS_RRTYPE_CNAME : State = 201; break;
            case DNS_RRTYPE_MX    : State = 801; break;
          };
      } else {
        char ADDR[512];
        strcpy(ADDR,(char *)(const char *)Host.toString().toUtf8().constData());
        connectToHost(ADDR,53);
        State = 103;
        setupTime();
      };
    break;
    case 103:
      switch (connectState()) {
        case 101:
        break;
        case 102:
          if (timeout(GHTIMEOUT*4)) {
            closeSocket();
            State = 101;
          };
        break;
        case 103:
          firstDns = false;
          State    = 101;
        break;
        case 104:
        break;
        case 105:
        break;
        case 106:
        break;
        case 107:
          switch (Mode) {
            case DNS_RRTYPE_A     : State = 301; break;
            case DNS_RRTYPE_NS    : State = 401; break;
            case DNS_RRTYPE_CNAME : State = 201; break;
            case DNS_RRTYPE_MX    : State = 201; break;
          };
          setupTime();
          if (firstDns) dnsConnected = mtime();
        break;
      };
    break;
    case 201:
      if (!isValid() || !isWriting) State = 101; else {
        memset(ERR,0,8192);
        nLength = dnsPacket(ERR,HOSTNAME);
        if (nLength>0) {
          Write(ERR,nLength);
          State     = 202;
          dataLen   = 0;
          dataCount = 0;
          setupTime();
          addOutgoing(nLength);
        } else State = 205;
      };
    break;
    case 202:
      if (!isValid()) State = 101; else {
        if (isReading) {
          setupTime();
          dataCount++;
          if (dataCount>=5) State = 203;
        } else
        if (dataLen>0) State = 204; else
        if (timeout(GHTIMEOUT)) State = 205;
      };
    break;
    case 203:
      if (!timeout(400)) break;
      if (!isValid()) State = 101; else {
        nLength = Read(&ERR[dataLen],4096);
        if (nLength>0) {
          if (!isMore()) State = 204;
                    else State = 202;
          dataLen += nLength;
          addIncoming(nLength);
        } else {
          if (dataLen>0) {
            if (!isMore()) State = 204;
                      else State = 202;
          } else State = 205;
        };
      };
    break;
    case 204:
      parseMX = ParseMX();
      if (parseMX && MX.count()>0) {
        emit setDns(MXTARGET,MX[0]);
        qApp->processEvents();
        dnsAnswer = mtime();
        State = 999;
      } else
      if (strlen(CNAME)>0) {
        State = 205;
      } else
      if (MXHOST.count()>0) {
        closeSocket();
        State = 999; /* This is fast mode */
//        State = 101; /* This is original mode */
      } else State = 205;
    break;
    case 205:
      for (nLength=0,mark=-1;mark<0 && nLength<dnsIp.count();nLength++)
        if (!IpUsed.contains(dnsIp[nLength])) mark = nLength;
      if (mark>=0) {
        Host.setAddress(dnsIp[mark]);
        IpUsed << dnsIp[mark];
        closeSocket();
        State = 101;
      } else {
        if (goNext) { State = 999; } else {
          /* DNS_RRTYPE_NS => Switch To NS Mode */
          IpUsed  . clear      (        ) ;
          Host    . setAddress (dnsIp[0]) ;
          IpUsed << dnsIp[0]              ;
          closeSocket();
          State   = 101;
          Mode    = DNS_RRTYPE_NS;
          goNext  = false;
        };
      };
    break;
    case 301:
      memset(ERR,0,8192);
      if (strlen(CNAME)>0) nLength = dnsPacket(ERR,CNAME   ,DNS_RRTYPE_A);
                      else nLength = dnsPacket(ERR,HOSTNAME,DNS_RRTYPE_A);
      if (nLength>0) {
        Write(ERR,nLength);
        setupTime ()         ;
        State     = 302      ;
        dataLen   = 0        ;
        dataCount = 0        ;
        addOutgoing(nLength) ;
      } else { State = 999; };
    break;
    case 302:
      if (!isValid()) State = 101; else {
        if (isReading) {
          setupTime();
          dataCount++;
          if (dataCount>=5) State = 303;
        } else
        if (dataLen>0         ) State = 304; else
        if (timeout(GHTIMEOUT)) State = 305;
      };
    break;
    case 303:
      if (!timeout(400)) break;
      if (!isValid()) State = 101; else {
        nLength = Read(&ERR[dataLen],4096);
        if (nLength>0) {
          if (!isMore()) State = 304;
                    else State = 302;
          dataLen += nLength;
          addIncoming(nLength);
        } else {
          if (dataLen>0) {
            if (!isMore()) State = 304;
                      else State = 302;
          } else State = 305;
        };
      };
    break;
    case 304:
      if (ParseA() && MX.count()>0) {
//        QHostInfo HI = QHostInfo::fromName(MX[0]);
        emit setIpAddress(MXTARGET,MX[0]);
        qApp->processEvents();
        State = 999;
      } else
      if (strlen(CNAME)>0) {
        closeSocket();
        State = 101;
      } else
      if (MXHOST.count()>0) {
//        State = 802; /* This is original mode */
        State = 999; /* This is fast mode */
      } else {
        State = 999;
      };
    break;
    case 305:
      for (nLength=0,mark=-1;mark<0 && nLength<dnsIp.count();nLength++)
        if (!IpUsed.contains(dnsIp[nLength])) mark = nLength;
      if (mark>=0) {
        Host    . setAddress(dnsIp[mark]);
        IpUsed << dnsIp[mark];
        closeSocket();
        State = 101;
      } else {
//        QHostInfo HI = QHostInfo::fromName(MXTARGET);
//        QList<QHostAddress> HAL = HI.addresses();
//        if (HAL.count()>0) {
//          emit setIpAddress(MXTARGET,HAL[0].toString());
//          qApp->processEvents();
//          while (HAL.count()>0) {
//            if (!HAL[0].isNull()) MX << HAL[0].toString();
//            HAL.removeAt(0);
//          };
//        };
        State = 999;
      };
    break;
    case 401:
      if (!isValid() || !isWriting) {
        State = 101;
      } else {
        nLength = dnsPacket(ERR,HOSTNAME,DNS_RRTYPE_NS);
        if (nLength>0) {
          Write(ERR,nLength)    ;
          State     = 402       ;
          dataLen   = 0         ;
          dataCount = 0         ;
          setupTime   (       ) ;
          addOutgoing (nLength) ;
        } else {
          MX << MXTARGET;
          emit setDns(MXTARGET,MX[0]);
          qApp->processEvents();
          Mode  = DNS_RRTYPE_MX;
          State = 999;
        };
      };
    break;
    case 402:
      if (!isValid()) { State = 101; } else {
        if (isReading) {
          setupTime();
          dataCount++;
          if (dataCount>=5) State = 403;
        } else
        if (dataLen>0) State = 404; else {
          if (timeout(GHTIMEOUT)) State = 405;
        };
      };
    break;
    case 403:
      if (!isValid()) { State = 101; } else {
        nLength = Read(&ERR[dataLen],4096);
        if (nLength>0) {
          if (!isMore()) State = 404;
                    else State = 402;
          dataLen += nLength;
          addIncoming(nLength);
        } else {
          if (dataLen>0) State = 404;
                    else State = 405;
        };
      };
    break;
    case 404:
      if (ParseNS() && MXHOST.count()>0) {
        goNext = true;
        closeSocket();
        Mode   = DNS_RRTYPE_MX;
        State  = 101;
      } else { State = 405; };
    break;
    case 405:
      for (nLength=0,mark=-1;mark<0 && nLength<dnsIp.count();nLength++)
        if (!IpUsed.contains(dnsIp[nLength])) mark = nLength;
      if (mark>=0) {
        Host.setAddress(dnsIp[mark]);
        IpUsed << dnsIp[mark];
        closeSocket();
        State = 101;
      } else {
        MX << MXTARGET;
        emit setDns(MXTARGET,MX[0]);
        qApp->processEvents();
        Mode  = DNS_RRTYPE_MX;
        MX.clear();
        State = 999;
      };
    break;
    case 501:
    break;
    case 801:
//     if (hostLookup>MAXLOOKUP) {
//     } else {
//       if (MXHOST.size()>0) {
//         State = 901;
//         setHost(MXHOST[0]);
//         MXHOST.removeAt(0);
//       } else {
         State = 999;
//       };
//     };
    break;
    case 802:
//     if (hostLookup>MAXLOOKUP) {
//     } else {
//       if (MXHOST.size()>0) {
//         State = 901;
//         setNs(MXHOST[0]);
//         MXHOST.removeAt(0);
//       } else {
         State = 999;
//       };
//     };
    break;
    case 901:
    break;
    case 911:
    return false;
    case 999:
      closeSocket();
      State = 911;
    return false;
  };
  return true;
};

/////////////////////////////////////////
nDnsEngine:: nDnsEngine (QObject *        parent)
          :  QThread    (                 parent) ,
             Timeout    (3*60                   ) ,
             NetworkType(Neutrino::UDP          ) ,
             Mode       (nDnsEngine::DnsQuery   ) ,
             Cache      (nDnsEngine::DirectQuery)
{
};

nDnsEngine::~nDnsEngine (void)
{
};

void nDnsEngine::run(void)
{
};

/////////////////////////////////////////

#define GETREADING 2000

#ifdef WIN32
#define SOCKET_PS   10
#else
#define SOCKET_PS    1
#endif

nDnsQuery::nDnsQuery(QObject * parent)
         : QThread  (          parent)
{
  working       = false    ;
  mlock         = false    ;
  Mode          = DnsQuery ;
  maxDns        = 128      ;
  connectings   = 0        ;
  waitings      = 0        ;
  socketFailure = 0        ;
};

nDnsQuery::~nDnsQuery(void)
{
};

bool nDnsQuery::isLock(void)
{
  return mlock;
};

void nDnsQuery::Unlock(void)
{
  mlock = false;
};

void nDnsQuery::Lock(void)
{
  mlock = true;
};

void nDnsQuery::Wait(void)
{
  while (mlock) {
    qApp->processEvents();
    msleep(50);
  };
};

void nDnsQuery::WaitVacancy(void)
{
  while (Query.size()>=maxDns) {
    qApp->processEvents();
    msleep(200);
  };
};

void nDnsQuery::setDnsRecord(QList<nDnsRecord *> & dns)
{
  nDnsRecord * D;
  Wait();
  Lock();
  DNS.clear();
  for (int i=0;i<dns.size();i++) {
    D = new nDnsRecord();
    *D = *(dns[i]);
    DNS << D;
  };
  Unlock();
};

void nDnsQuery::appendQuery(QueryType Type,QString hostname)
{
  nDnsRecord * d = Pick();
  nDns       * q = new nDns(this);
  
  q->State  = 0        ;
  q->DnsId  = d ->Id   ;
  q->dnsIp << d->DnsIP ;
  
  switch (Type) {
    case DnsAddress :
      q->Mode     = DNS_RRTYPE_A  ;
      q->MXTARGET = hostname      ;
    break;
    case DnsMX      :
      q->Mode     = DNS_RRTYPE_MX ;
      q->MXTARGET = hostname      ;
    break;
  };
  
  WaitVacancy();
  Wait();
  Lock();
  Query << q;
  Unlock();
};

void nDnsQuery::appendQuery(QueryType Type,QString hostname,int HostId)
{
  nDnsRecord * d = Pick();
  nDns       * q = new nDns(this);
  
  q->State  = 0        ;
  q->DnsId  = d ->Id   ;
  q->dnsIp << d->DnsIP ;
  
  switch (Type) {
    case DnsAddress :
      q->Mode     = DNS_RRTYPE_A  ;
      q->MXTARGET = hostname      ;
      q->Id       = HostId        ;
    break;
    case DnsMX      :
      q->Mode     = DNS_RRTYPE_MX ;
      q->MXTARGET = hostname      ;
      q->Id       = HostId        ;
    break;
  };
  
  WaitVacancy();
  Wait();
  Lock();
  Query << q;
  Unlock();
};

void nDnsQuery::appendQuery(QueryType Type,int DnsId,QString hostname)
{
  nDnsRecord * d = Pick(DnsId);
  nDns       * q = new nDns(this);
  
  q->State  = 0        ;
  q->DnsId  = d ->Id   ;
  q->dnsIp << d->DnsIP ;
  
  switch (Type) {
    case DnsAddress :
      q->Mode     = DNS_RRTYPE_A  ;
      q->MXTARGET = hostname      ;
    break;
    case DnsMX      :
      q->Mode     = DNS_RRTYPE_MX ;
      q->MXTARGET = hostname      ;
    break;
  };
  
  WaitVacancy();
  Wait();
  Lock();
  Query << q;
  Unlock();
};

bool nDnsQuery::isTimeout(int dt)
{
  if ((tmark+dt)<mtime()) return true;
  return false;
};

nDnsRecord * nDnsQuery::Pick(void)
{
  if (0>=DNS.size()) return NULL;
  nDnsRecord * d = DNS[rand()%DNS.size()];
  return d;
};

nDnsRecord * nDnsQuery::Pick(int Id)
{
  if (0>=DNS.size()) return NULL;
  nDnsRecord * d = NULL;
  int          m = -1;
  for (int i=0;m<0&&i<DNS.size();i++)
    if (Id==DNS[i]->Id) m = i;
  if (0<=m) d = DNS[m];
  return d;
};

void nDnsQuery::getReading(void)
{
  fd_set         rd;
  fd_set         wr;
  fd_set         ep;
  struct timeval tv;
  int            rtcode;
  int            err;
  int            n;
  int            c = 0;
  int            socket_max = -1;

  connectings   = 0;
  waitings      = 0;
  socketFailure = 0;
  
  if (0>=Query.size()) return;
  
  FD_ZERO(&rd);
  FD_ZERO(&wr);
  FD_ZERO(&ep);
  
  for (n=0;n<Query.size();n++) if (Query[n]->dns_socket>=0 && Query[n]->State>102) {
    FD_SET(Query[n]->dns_socket,&rd);
    FD_SET(Query[n]->dns_socket,&wr);
    FD_SET(Query[n]->dns_socket,&ep);
    if (socket_max<Query[n]->dns_socket) socket_max = Query [n]->dns_socket;
    c++;
//    waitings++;
  } else {
    Query[n]->isReading = false;
    Query[n]->isWriting = false;
    if (Query[n]->State> 101 && Query[n]->dns_socket<0) socketFailure++;
    if (Query[n]->State> 101) waitings++;
  };
  
  if (c<=0) return;
  
  tv.tv_sec  = 0;
  tv.tv_usec = GETREADING;
  
  select(socket_max+1, &rd, &wr, &ep, &tv);

  for (n=0;n<Query.size();n++) if (Query[n]->dns_socket>=0) {
    if (FD_ISSET(Query[n]->dns_socket,&rd)) Query [n]->isReading = true  ;
                                       else Query [n]->isReading = false ;
    if (FD_ISSET(Query[n]->dns_socket,&wr)) Query [n]->isWriting = true  ;
                                       else Query [n]->isWriting = false ;
    if (Query[n]->isReading || Query[n]->isWriting) connectings++;
  } else {
    Query [n]->isReading = false;
    Query [n]->isWriting = false;
  };
};

void nDnsQuery::processRecord(void)
{
  nDns * ghd;
  switch (Query[dnsIndex]->Mode) {
    case DNS_RRTYPE_A:
      if (Query[dnsIndex]->MX.count()>0) {
        switch (Mode) {
          case DnsQuery:
            ghd = Query.takeAt(dnsIndex);
            if (ghd!=NULL) emit dnsSuccess(ghd);
          break;
          case DnsSpeed:
            ghd = Query.takeAt(dnsIndex);
            if (ghd!=NULL) {
              emit dnsSpeed(ghd->DnsId,ghd->TTC(),ghd->TTQ(),ghd->caching);
              ghd->clear();
              delete ghd;
            };
          break;
        };
      } else {
        switch (Mode) {
          case DnsQuery:
            ghd = Query.takeAt(dnsIndex);
            if (ghd!=NULL) emit dnsFailure(ghd);
          break;
          case DnsSpeed:
            ghd = Query.takeAt(dnsIndex);
            if (ghd!=NULL) {
              emit dnsFailure(ghd->DnsId);
              ghd->clear();
              delete ghd;
            };
          break;
        };
      };
    break;
    case DNS_RRTYPE_CNAME:
    case DNS_RRTYPE_NS:
      if (Query[dnsIndex]->MX.count()==0) { ; };
      ghd = Query.takeAt(dnsIndex);
      if (ghd!=NULL) {
        ghd->clear();
        delete ghd;
      };
    break;
    case DNS_RRTYPE_MX:
      if (Query[dnsIndex]->MX.count()>0) {
        switch (Mode) {
          case DnsQuery:
            ghd = Query.takeAt(dnsIndex);
            if (ghd!=NULL) emit dnsSuccess(ghd);
          break;
          case DnsSpeed:
            ghd = Query.takeAt(dnsIndex);
            if (ghd!=NULL) {
              emit dnsSpeed(ghd->DnsId,ghd->TTC(),ghd->TTQ(),ghd->caching);
              ghd->clear();
              delete ghd;
            };
          break;
        };
      } else {
        switch (Mode) {
          case DnsQuery:
            ghd = Query.takeAt(dnsIndex);
            if (ghd!=NULL) emit dnsFailure(ghd);
          break;
          case DnsSpeed:
            ghd = Query.takeAt(dnsIndex);
            if (ghd!=NULL) {
              emit dnsFailure(ghd->DnsId);
              ghd->clear();
              delete ghd;
            };
          break;
        };
      };
    break;
  };
};

void nDnsQuery::run(void)
{
  if (working) return;
  
  working      = true;
  
  lastUpdate   = QDateTime::currentDateTime();
  socketTime   = QDateTime::currentDateTime();
  tmark        = mtime();
  dnsIndex     = 0;
  IncomingData = 0;
  OutgoingData = 0;
  
  Query . clear ();
  
  hostStart = QDateTime::currentDateTime();
  
  while (working) {
    qApp->processEvents();
    if (Query.size()>0) {
      Wait();
      Lock();
      for (int nnn=0;nnn<16;nnn++) {
        for (dnsIndex=0;dnsIndex<Query.size();dnsIndex++)
          if (0==Query[dnsIndex]->State) Query[dnsIndex]->RUN();
        getReading();
        for (dnsIndex=0;dnsIndex<Query.size();) {
          bool retcode,doit,time_delay;
          time_delay = false;
          switch (Query[dnsIndex]->State) {
            case 101:
              if (!isTimeout(SOCKET_PS)) time_delay = true;
                                    else tmark      = mtime();
            break;
            case 102:
              if (hostLookup>=MAXLOOKUP && Query[dnsIndex]->MXHOST.count()>0) time_delay = true;
            break;
            case 501:
              Query[dnsIndex]->State = 999;
            break;
          };
          if (time_delay) {
            dnsIndex++; 
          } else {
            retcode = Query[dnsIndex]->RUN();
            IncomingData += Query[dnsIndex]->IncomingData;
            OutgoingData += Query[dnsIndex]->OutgoingData;
            Query[dnsIndex]->IncomingData = 0;
            Query[dnsIndex]->OutgoingData = 0;
            if (retcode) dnsIndex++; else processRecord();
            qApp->processEvents();
          };
        };
      };
      Unlock();
    };
    if (0>=Query.size()) MSLEEP(200);
                    else MSLEEP( 25);
    if ((maxDns-Query.size())>16) emit requestQuery();
    
  };
  
};

////////////////////////////////////////////
nDnsConnection:: nDnsConnection (QObject * parent)
              :  QThread        (          parent) ,
                 Timeout        (3*60            ) ,
                 working        (false           ) ,
                 DNS            (NULL            )
{
};

nDnsConnection::~nDnsConnection (void)
{
};

void nDnsConnection::Start(void)
{
  if (NULL==DNS) {
    working = false;
    return;
  };
  if (working) return;
  start();
};

void nDnsConnection::Stop(void)
{
  working = false;
};

bool nDnsConnection::isTimeout(int index)
{
  long long to = Timeout * 1000;
  long long tn = nTime::timestamp();
  tn -= timestamps[index];
  if (tn>to) return true;
  return false;
};

void nDnsConnection::ConnectToHost(int index)
{
  struct sockaddr_in dest;
  char               ADDR[256];
  int                rtcode;

  timestamps[index] = nTime::timestamp();
  emit Connecting(index,timestamps[index]);
  qApp->processEvents();

  lastSocket = timestamps[index];

  strcpy(
    ADDR,
    (char *)(const char *)
    (*DNS)[index]->DnsIP.toUtf8().constData()
  );
  memset(&dest,0,sizeof(dest));
  dest.sin_family = PF_INET;
  dest.sin_port   = htons(53);
  #ifdef WIN32
  unsigned long IA = inet_addr(ADDR);
  memcpy(
    &dest.sin_addr,
    &IA,
    sizeof(unsigned long)
  );
  #else
  inet_aton(ADDR,&dest.sin_addr);
  #endif
  #ifdef WIN32
  unsigned long nNonBlocking = 1;
  if (ioctlsocket(
        Sockets[index],
        FIONBIO,
        &nNonBlocking
      )==SOCKET_ERROR) {
    emit Invalid(index,nTime::timestamp());
    qApp->processEvents();
    Steps[index] = 103;
    return;
  };
  rtcode = ::connect(
               Sockets[index],
               (struct sockaddr*)&dest,
               sizeof(dest)
           );
  if (rtcode<0) {
    if (errno!=WSAEINPROGRESS) {
      emit Invalid(index,nTime::timestamp());
      qApp->processEvents();
      Steps[index] = 103;
      return;
    };
  };
  #else
  socket_flags = ::fcntl(
                     Sockets[index],
                     F_GETFL,
                     0
                 );
  if (socket_flags < 0) {
    emit Invalid(index,nTime::timestamp());
    qApp->processEvents();
    Steps[index] = 103;
    return;
  };
  if (::fcntl(
          Sockets[index],
          F_SETFL,
          socket_flags | O_NONBLOCK
      ) < 0) {
    emit Invalid(index,nTime::timestamp());
    qApp->processEvents();
    Steps[index] = 103;
    return;
  };
  rtcode = ::connect(
               Sockets[index],
               (struct sockaddr*)&dest,
               sizeof(dest)
            );
  if (rtcode<0) {
    if (errno!=EINPROGRESS) {
      emit Invalid(index,nTime::timestamp());
      qApp->processEvents();
      Steps[index] = 103;
      return;
    };
  };
  #endif
  Steps[index] = 102;
};

void nDnsConnection::ConnectState(int index)
{
  fd_set         rd,wr;
  struct timeval tv;
  int            rtcode;
  int            err;
  #ifdef WIN32
  #else
  socklen_t      len;
  #endif

  if (Sockets[index]<0) {
    emit Invalid(index,nTime::timestamp());
    qApp->processEvents();
    Steps[index] = 999;
    return;
  };

  FD_ZERO(&rd);
  FD_ZERO(&wr);
  FD_SET(Sockets[index],&rd);
  FD_SET(Sockets[index],&wr);

  tv.tv_sec  = 0;
  tv.tv_usec = 200;

  rtcode = select(
             Sockets[index]+1,
             &rd,
             &wr,
             NULL,
             &tv
           );

  if (rtcode<=0) {
    if (isTimeout(index)) {
      Steps[index] = 103;
      emit timeout(index,nTime::timestamp());
      qApp->processEvents();
    };
  } else {
    if (FD_ISSET(Sockets[index],&rd) ||
        FD_ISSET(Sockets[index],&wr)) {
      #ifdef WIN32
      unsigned long nNonBlocking = 0;
      if (::ioctlsocket(
            Sockets[index],
            FIONBIO,
            &nNonBlocking
            )==SOCKET_ERROR) {
        emit Invalid(index,nTime::timestamp());
        qApp->processEvents();
        Steps[index] = 103;
        return;
      };
      #else
      len = sizeof(err);
      if (::getsockopt(
            Sockets[index],
            SOL_SOCKET,
            SO_ERROR,
            &err,
            &len
            )<0) {
        emit Invalid(index,nTime::timestamp());
        qApp->processEvents();
        Steps[index] = 103;
        return;
      };
      if (::fcntl(
            Sockets[index],
            F_SETFL,
            socket_flags
            )<0) {
        emit Invalid(index,nTime::timestamp());
        qApp->processEvents();
        Steps[index] = 103;
        return;
      };
      #endif
      emit Connected(index,nTime::timestamp());
      qApp->processEvents();
      Steps[index] = 103;
    } else {
      if (isTimeout(index)) {
        Steps[index] = 103;
        emit timeout(index,nTime::timestamp());
        qApp->processEvents();
      };
    };
  };
};

void nDnsConnection::CloseSocket(int index)
{
  if (Sockets[index]<0) {
    Steps[index] = 999;
    return;
  };
  #ifdef WIN32
  ::closesocket (Sockets[index]);
  #else
  ::close       (Sockets[index]);
  #endif
  Sockets[index] =  -1;
  Steps  [index] = 999;
};

void nDnsConnection::doProcess(int index)
{
  switch (Steps[index]) {
    case 101:
      if ((nTime::timestamp()-lastSocket)>10)
        ConnectToHost(index);
    break;
    case 102:
      ConnectState(index);
    break;
    case 103:
      CloseSocket(index);
    break;
    default:
    break;
  };
};

void nDnsConnection::run(void)
{
  int i,count;
  if (NULL==DNS) {
    working = false;
    return;
  };
  if (working) return;
  working = true;
  for (i=0;i<DNS->size();i++) {
    switch (Type) {
      case Neutrino::UDP:
        Sockets    << socket(PF_INET,SOCK_DGRAM ,0);
      break;
      case Neutrino::TCP:
        Sockets    << socket(PF_INET,SOCK_STREAM,0);
      break;
    };
    Steps      << 101;
    timestamps << nTime::timestamp();
  };
  lastSocket = nTime::timestamp();
//  for (i=0;i<DNS->size();i++) ConnectToHost(i);
  while (working) {
    for (i=0,count=0;i<DNS->size();i++) {
      if (999!=Steps[i]) count++;
      doProcess(i);
    };
    qApp->processEvents();
    msleep(1);
    if (0==count) working = false;
  };
  emit Finished();
};

////////////////////////////////////////////
#ifdef QT_SQL_LIB

nDnsSql::nDnsSql(QObject * parent)
       : QObject(         parent)
{
};

nDnsSql::~nDnsSql(void)
{
};

void nDnsSql::dnsSuccess(nDns * dns)
{
  nDnsRecord * d = dnsFind(dns->DnsId);
  if (NULL==d) return;
  unsigned long long ttl = d->ttl;
  ttl *= d   -> success;
  ttl += dns -> TTQ();
  d->success++;
  ttl /= d->success;
  d->ttl = (unsigned int)ttl;
  if (dns->caching) d->cachings  ++;
               else d->alterings ++;
};

void nDnsSql::dnsFailure(nDns * dns)
{
  nDnsRecord * d = dnsFind(dns->DnsId);
  if (NULL==d) return;
  d->failure++;
};

nDnsRecord * nDnsSql::dnsFind(int dnsId)
{
  for (int i=0;i<DNS.size();i++)
    if (dnsId==DNS[i]->Id) return DNS[i];
  return NULL;
};

QString nDnsSql::rIndex(QString hostname)
{
  QString r = "";
  for (int i=0;i<hostname.length();i++) r += hostname.at(hostname.length()-i-1);
  return r;
};

bool nDnsSql::isExists(void)
{
  bool queryrt = false;
  {
    QStringList tables;
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    if (db.isValid()) queryrt = true;
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (queryrt && db.open()) {
      tables = db.tables();
      db.close();
    } else queryrt = false;
    if (queryrt) {
      if (queryrt && !tables.contains(PrefixName + "dns")) queryrt = false;
    };
  };
  QSqlDatabase::removeDatabase("QMYSQL");
  return queryrt;
};

bool nDnsSql::CreateTables(void)
{
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (db.open()) {
      QString Q;
      QSqlQuery query;
      Q = "create table " + PrefixName + "dns ("                                   +
          "id integer primary key auto_increment,"                                 +
          "ip varchar(20) default '',"                                             +
          "countryid integer default -1,"                                          +
          "rank integer default 65536,"                                            +
          "ttl int unsigned default 100000000,"                                    +
          "connections integer default 4,"                                         +
          "success integer default 0,"                                             +
          "failure integer default 0,"                                             +
          "cachings integer default 0,"                                            +
          "alterings integer default 0,"                                           +
          "ltime timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP," +
          "foreign key (countryid) references " + PrefixName + "country (id)"      +
          ");";
      queryrt = query.exec(Q);
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return queryrt;
};

bool nDnsSql::Initialize(void)
{
  if (!isExists()) CreateTables();
  return false;
};

bool nDnsSql::LoadAll(void)
{
  DNS . clear();
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (db.open()) {
      QString      Q      ;
      QSqlQuery    query  ;
      nDnsRecord * RECORD ;
      nIpAddress * ip     ;
      Q = "select id,ip,countryid,rank,ttl,connections,success,failure,cachings,alterings from " + PrefixName + "dns order by ip,id;";
      queryrt = query.exec(Q);
      if (queryrt && query.size()>0) {
        while (query.next()) {
          RECORD = new nDnsRecord();
          RECORD -> Id          = query.value(0).toInt  (                            );
          RECORD -> DnsIP       = QString::fromUtf8     (query.value(1).toByteArray());
          RECORD -> CountryId   = query.value(2).toInt  (                            );
          RECORD -> Rank        = query.value(3).toInt  (                            );
          RECORD -> ttl         = query.value(4).toUInt (                            );
          RECORD -> Connections = query.value(5).toInt  (                            );
          RECORD -> success     = query.value(6).toInt  (                            );
          RECORD -> failure     = query.value(7).toInt  (                            );
          RECORD -> cachings    = query.value(8).toInt  (                            );
          RECORD -> alterings   = query.value(9).toInt  (                            );
          DNS << RECORD;
        };
      };
      IPes.clear();
      IpCountry.clear();
      Q = "select ipv4id,ipaddress,netmask from " + PrefixName + "ipaddress order by ipv4id asc;";
      queryrt = query.exec(Q);
      if (queryrt && query.size()>0) {
        while (query.next()) {
          ip    = new nIpAddress();
          ip   -> ID = query.value(0).toInt();
          ip   -> setIpBlock(query.value(1).toUInt(),query.value(2).toUInt());
          IPes << ip;
        };
      };
      Q = "select ipv4id,countryid from " + PrefixName + "ipcountry order by ipv4id asc;";
      queryrt = query.exec(Q);
      if (queryrt && query.size()>0) {
        while (query.next()) {
          IpCountry << query.value(1).toInt();
        };
      };
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return queryrt;
};

bool nDnsSql::LoadByTTL(void)
{
  DNS . clear();
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (db.open()) {
      QString      Q      ;
      QSqlQuery    query  ;
      nDnsRecord * RECORD ;
      Q = "select id,ip,countryid,rank,ttl,connections,success,failure,cachings,alterings from " + PrefixName + "dns where rank<100 order by ttl asc;";
      queryrt = query.exec(Q);
      if (queryrt && query.size()>0) {
        while (query.next()) {
          RECORD = new nDnsRecord();
          RECORD -> Id          = query.value(0).toInt  (                            );
          RECORD -> DnsIP       = QString::fromUtf8     (query.value(1).toByteArray());
          RECORD -> CountryId   = query.value(2).toInt  (                            );
          RECORD -> Rank        = query.value(3).toInt  (                            );
          RECORD -> ttl         = query.value(4).toUInt (                            );
          RECORD -> Connections = query.value(5).toInt  (                            );
          RECORD -> success     = query.value(6).toInt  (                            );
          RECORD -> failure     = query.value(7).toInt  (                            );
          RECORD -> cachings    = query.value(8).toInt  (                            );
          RECORD -> alterings   = query.value(9).toInt  (                            );
          DNS << RECORD;
        };
      };
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return queryrt;
};

bool nDnsSql::SaveTTL(void)
{
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (db.open()) {
      QString      Q      ;
      QSqlQuery    query  ;
      nDnsRecord * RECORD ;
      for (int i=0;i<DNS.size();i++) {
        Q = "update " + PrefixName + "dns set ttl=:TTL,success=:SUCCESS,failure=:FAILURE,countryid=:CID,cachings=:CACHE,alterings=:ALTERS where id=:ID;";
        query.prepare(Q);
        query.bindValue(":ID"     ,DNS[i]->Id       );
        query.bindValue(":CID"    ,DNS[i]->CountryId);
        query.bindValue(":TTL"    ,DNS[i]->ttl      );
        query.bindValue(":SUCCESS",DNS[i]->success  );
        query.bindValue(":FAILURE",DNS[i]->failure  );
        query.bindValue(":CACHE"  ,DNS[i]->cachings );
        query.bindValue(":ALTERS" ,DNS[i]->alterings);
        query.exec();
      };
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return queryrt;
};

int nDnsSql::LoadMX(QList<int> & MxId,QStringList & Hosts,int Packet)
{
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (db.open()) {
      int          total = 0 ;
      QString      Q         ;
      QString      S         ;
      QSqlQuery    query     ;
//      Q = "select count(*) from " + PrefixName + "mailhosts where validate=1;";
//      if (query.exec(Q) && query.size()>0 && query.next()) total = query.value(0).toInt();
//      S.sprintf("%d,%d;",total>0 ? (rand()%total):0,Packet);
      S.sprintf("%d,%d;",0,Packet);
      Q = "select id,name from " + PrefixName + "mailhosts where validate=1 limit " + S;
      if (query.exec(Q) && query.size()>0) {
        while (query.next()) {
          MxId  << query.value(0).toInt();
          Hosts << QString::fromUtf8(query.value(1).toByteArray());
        };
      };
      for (int n=0;n<MxId.size();n++) {
        Q = "update " + PrefixName + "mailhosts set validate=2 where id=:ID;";
        query.prepare(Q);
        query.bindValue(":ID",MxId[n]);
        query.exec();
      };
//      if (total>400000) {
//        Q = "update " + PrefixName + "mailhosts set validate=0 where validate=1 limit 100000;";
//        query.exec(Q);
//      } else
//      if (total<100000) {
//        Q = "update " + PrefixName + "mailhosts set validate=1 where validate=0 limit 100000;";
//        query.exec(Q);
//      };
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return MxId.size();
};

int nDnsSql::SaveMX(nDns * dns)
{
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (db.open() && dns->MX.size()>0) {
      QString      Q      ;
      QString      S      ;
      QSqlQuery    query  ;
      Q = "update " + PrefixName + "mailhosts set validate=3 where id=:ID;";
      query.prepare(Q);
      query.bindValue(":ID",dns->Id);
      query.exec();
      for (int n=0;n<dns->MX.size();n++) {
        Q = "insert into " + PrefixName + "mxtable (hostid,mx) values (:HOSTID,:MX);";
        query.prepare(Q);
        query.bindValue(":HOSTID",dns->Id   );
        query.bindValue(":MX"    ,dns->MX[n]);
        query.exec();
        Q = "insert into " + PrefixName + "hostnames (url,rindex) values (:URL,:RINDEX);";
        query.prepare(Q);
        query.bindValue(":URL"   ,       dns->MX[n] .toUtf8());
        query.bindValue(":RINDEX",rIndex(dns->MX[n]).toUtf8());
        query.exec();
      };
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return dns->Id;
};

int nDnsSql::MarkMX(nDns * dns,int validate)
{
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (db.open()) {
      QString      Q      ;
      QString      S      ;
      QSqlQuery    query  ;
      Q = "update " + PrefixName + "mailhosts set validate=:VALIDATE where id=:ID;";
      query.prepare(Q);
      query.bindValue(":ID"      ,dns->Id );
      query.bindValue(":VALIDATE",validate);
      if (query.exec()) {
//        printf("Mark %d = %d\n",dns->Id,validate);
        for (int n=0;n<dns->MXHOST.size();n++) {
          Q = "insert into " + PrefixName + "hostnames (url,rindex) values (:URL,:RINDEX);";
          query.prepare(Q);
          query.bindValue(":URL"   ,       dns->MXHOST[n] .toUtf8());
          query.bindValue(":RINDEX",rIndex(dns->MXHOST[n]).toUtf8());
          query.exec();
        };
      };
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return dns->Id;
};

int nDnsSql::SaveNS(nDns * dns)
{
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (db.open()) {
      QString      Q      ;
      QString      S      ;
      QSqlQuery    query  ;
      for (int n=0;n<dns->MXHOST.size();n++) {
        Q = "insert into " + PrefixName + "hostnames (url,rindex) values (:URL,:RINDEX);";
        query.prepare(Q);
        query.bindValue(":URL"   ,       dns->MXHOST[n] .toUtf8());
        query.bindValue(":RINDEX",rIndex(dns->MXHOST[n]).toUtf8());
        query.exec();
      };
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return dns->Id;
};

int nDnsSql::LoadHost(QList<int> & HostId,QStringList & Hosts,int MXs)
{
  HostId . clear ();
  Hosts  . clear ();
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (db.open()) {
      QString      Q      ;
      QString      S      ;
      QSqlQuery    query  ;
      S.sprintf("0,%d;",MXs);
      Q = "select id,url from hostnames where validate=0 order by id limit " + S;
      if (query.exec(Q) && query.size()>0) {
        while (query.next()) {
          HostId <<                   query.value(0).toInt();
          Hosts  << QString::fromUtf8(query.value(1).toByteArray());
        };
      };
      if (HostId.size()>0) {
        for (int i=0;i<HostId.size();i++) {
          Q = "update hostnames set validate=9 where id=:ID;";
          query.prepare   (Q              );
          query.bindValue (":ID",HostId[i]);
          query.exec      (               );
        };
      };
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return HostId.size();
};

int nDnsSql::SaveHost(QString hostname,QStringList ipaddress)
{
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (ipaddress.size()>0 && db.open()) {
      QString      Q      ;
      QString      S      ;
      QSqlQuery    query  ;
      int          ID = -1;
      Q = "select id from " + PrefixName + "hostnames where url=:URL;";
      query.prepare(Q);
      query.bindValue(":URL",hostname.toUtf8());
      if (query.exec() && query.size()>0 && query.next()) ID = query.value(0).toInt();
      if (ID>=0) {
        Q = "update " + PrefixName + "hostnames set validate=2 where id=:ID;";
        query.prepare(Q);
        query.bindValue(":ID",ID);
        if (query.exec()) {
          for (int i=0;i<ipaddress.size();i++) {
            Q = "insert into " + PrefixName + "ipmap (hostid,ipaddress) values (:HOSTID,:IPADDRESS);";
            query.prepare(Q);
            query.bindValue(":HOSTID"   ,ID                   );
            query.bindValue(":IPADDRESS",ipaddress[i].toUtf8());
            query.exec();
          };
        };
      };
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return ipaddress.size();
};

int nDnsSql::MarkHost(int HostId,int validate)
{
  bool  queryrt = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QMYSQL");
    db . setDatabaseName (dbName  );
    db . setHostName     (Hostname);
    db . setUserName     (Username);
    db . setPassword     (Password);
    if (db.open()) {
      QString      Q      ;
      QString      S      ;
      QSqlQuery    query  ;
      Q = "update " + PrefixName + "hostnames set validate=:VALIDATE where id=:ID;";
      query.prepare(Q);
      query.bindValue(":ID"      ,HostId  );
      query.bindValue(":VALIDATE",validate);
      query.exec();
      db.close();
    } else {
      #ifdef MANAUL_DEBUG
      QSqlError err = db.lastError();
      qDebug(err.text().toUtf8().constData());
      #endif
    };
  };
  return HostId;
};

bool LoadDns(nSqlSettings & SQL,QList<nDnsRecord *> & dns)
{
  {
    QSqlDatabase db = SQL.db();
    if (SQL.Open(db)) {
      QSqlQuery query;
      if (query.exec(SQL.SqlCommand)) {
        nDnsRecord * IP;
        while (query.next()) {
           IP  = new nDnsRecord();
          *IP  = query;
          dns << IP;
        };
      } else
      SQL.Close(db);
    };
  };
  QSqlDatabase::removeDatabase(SQL.SqlDevice());
  return dns.count()>0;
};

#endif
