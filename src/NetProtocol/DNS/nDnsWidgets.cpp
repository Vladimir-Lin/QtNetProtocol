#include <nWork>

nDnsWidget::nDnsWidget   (QWidget * parent,
                          DnsConnectivity dns,
                          Neutrino::SocketType type)
          : nTreeWidget  (          parent     ) ,
            connectivity (dns                  ) ,
            Type         (type                 ) ,
            Timeout      (3 * 60               ) ,
            DNS          (NULL                 )
{
  setAccessibleName("nDnsWidget");
  ConnectionTest = NULL;
  setConnectivity(connectivity);
};

nDnsWidget::~nDnsWidget(void)
{
};

nDnsWidget::DnsConnectivity nDnsWidget::Connectivity(void)
{
  return connectivity;
};

void nDnsWidget::setConnectivity(DnsConnectivity dns)
{
  connectivity = dns;
  Initialize();
};

int nDnsWidget::timeout(void)
{
  return Timeout;
};

void nDnsWidget::setTimeout(int tout)
{
  Timeout = tout;
};

Neutrino::SocketType nDnsWidget::socketType(void)
{
  return Type;
};

void nDnsWidget::setSocketType(Neutrino::SocketType type)
{
  Type = type;
};

void nDnsWidget::Initialize(void)
{
  setRootIsDecorated(false);
  switch (connectivity)
  {
    case nDnsWidget::Connection:
      setColumnCount(5);
    break;
    case nDnsWidget::Speed:
      setColumnCount(10);
    break;
    case nDnsWidget::Quality:
      setColumnCount(10);
    break;
    case nDnsWidget::Edit:
      setColumnCount(10);
    break;
  };
  setHeaders();
};

void nDnsWidget::setHeaders(void)
{
  QStringList headers;
  switch (connectivity)
  {
    case nDnsWidget::Connection:
      headers << tr("Dns IP");
      headers << tr("Status");
      headers << tr("Start Connection");
      headers << tr("Connected");
      headers << tr("Connection Time");
    break;
    case nDnsWidget::Speed:
    case nDnsWidget::Quality:
    case nDnsWidget::Edit:
      headers << tr("ID");
      headers << tr("Dns IP");
      headers << tr("Country");
      headers << tr("Rank");
      headers << tr("Connections");
      headers << tr("TTL");
      headers << tr("Success");
      headers << tr("Failure");
      headers << tr("Cachings");
      headers << tr("Alterings");
    break;
  };
  setHeaderLabels(headers);
};

void nDnsWidget::setTextItem(QTreeWidgetItem * item,int index)
{
  QDateTime    DnsStart ;
  QDateTime    DnsDone  ;
  long long    CTime    ;
  QString      Q;
  nDnsRecord * dns = (*DNS)[index];
  switch (connectivity)
  {
    case nDnsWidget::Connection:
      Q = dns->DnsIP;
      item->setText(0,Q);
      switch (dnsStatus[index]) {
        case -1 :
          Q = tr("Idle"      );
          item->setText(1,Q);
          item->setText(2,Q);
          item->setText(3,Q);
          item->setText(4,Q);
        break;
        case  0 :
          Q = tr("Connecting");
          item->setText(1,Q);
          DnsStart.setTime_t(dnsStart[index]/1000);
          item->setText(2,DnsStart.toString("hh:mm:ss"));
          item->setText(3,tr("Waiting"));
          item->setText(4,tr("Waiting"));
        break;
        case  1 :
          Q = tr("Success"   );
          item->setText(1,Q);
          DnsStart.setTime_t(dnsStart[index]/1000);
          item->setText(2,DnsStart.toString("hh:mm:ss"));
          DnsDone.setTime_t (dnsDone [index]/1000);
          item->setText(3,DnsDone .toString("hh:mm:ss"));
          CTime = dnsDone[index] - dnsStart[index];
          Q.sprintf("%d milliseconds",(int)CTime);
          item->setText(4,Q);
        break;
        case  2 :
          Q = tr("Timeout"   );
          item->setText(1,Q);
          DnsStart.setTime_t(dnsStart[index]/1000);
          item->setText(2,DnsStart.toString("hh:mm:ss"));
          DnsDone.setTime_t (dnsDone [index]/1000);
          item->setText(3,DnsDone .toString("hh:mm:ss"));
          item->setText(4,tr("Timeout"));
        break;
        default :
          Q = tr("Invalid");
          item->setText(1,Q);
          DnsStart.setTime_t(dnsStart[index]/1000);
          item->setText(2,DnsStart.toString("hh:mm:ss"));
          DnsDone.setTime_t (dnsDone [index]/1000);
          item->setText(3,DnsDone .toString("hh:mm:ss"));
          Q = tr("Invalid");
          item->setText(4,Q);
        break;
      };
    break;
    case nDnsWidget::Speed:
    case nDnsWidget::Quality:
    case nDnsWidget::Edit:
      Q.sprintf("%d",dns->Id);
      item->setText(0,Q);
      Q = dns->DnsIP;
      item->setText(1,Q);
      Q.sprintf("%d",dns->CountryId);
      item->setText(2,Q);
      Q.sprintf("%d",dns->Rank);
      item->setText(3,Q);
      Q.sprintf("%d",dns->Connections);
      item->setText(4,Q);
      Q.sprintf("%u",dns->ttl);
      item->setText(5,Q);
      Q.sprintf("%d",dns->success);
      item->setText(6,Q);
      Q.sprintf("%d",dns->failure);
      item->setText(7,Q);
      Q.sprintf("%d",dns->success);
      item->setText(8,Q);
      Q.sprintf("%d",dns->alterings);
      item->setText(9,Q);
    break;
  };
};

void nDnsWidget::setDnsList(QList<nDnsRecord *> & dns)
{
  QTreeWidgetItem * item;
  DNS = &dns;
  /* Build QList<QTreeWidgetItem *> */
  Items.clear();
  clear();
  dnsStatus     . clear();
  dnsConnection . clear();
  dnsStart      . clear();
  dnsDone       . clear();
  for (int i= 0;i<dns.size();i++) {
    dnsStatus     << -1;
    dnsConnection << -1;
    dnsStart      << -1;
    dnsDone       << -1;
    item = new QTreeWidgetItem();
    Items << item;
    setTextItem(item,i);
    addTopLevelItem(item);
  };
  Report();
};

void nDnsWidget::Report(void)
{
  if (NULL==DNS) return;
  for (int i=0;i<DNS->size();i++) setTextItem(Items[i],i);
  switch (connectivity)
  {
    case nDnsWidget::Connection :
      for (int i=0;i<3;i++)
        resizeColumnToContents(i);
    break;
    case nDnsWidget::Speed      :
    case nDnsWidget::Quality    :
    case nDnsWidget::Edit       :
     for (int i=0;i<10;i++)
       resizeColumnToContents(i);
    break;
  };
};

void nDnsWidget::Start(void)
{
  switch (connectivity)
  {
    case nDnsWidget::Connection :
      ConnectionTest = new nDnsConnection(this);
      ConnectionTest-> DNS  = DNS  ;
      ConnectionTest-> Type = Type ;
      QObject::connect(
        ConnectionTest , SIGNAL (Connecting(int,long long)),
        this           , SLOT   (Connecting(int,long long))
      );
      QObject::connect(
        ConnectionTest , SIGNAL (Connected (int,long long)),
        this           , SLOT   (Connected (int,long long))
      );
      QObject::connect(
        ConnectionTest , SIGNAL (Invalid   (int,long long)),
        this           , SLOT   (Invalid   (int,long long))
      );
      QObject::connect(
        ConnectionTest , SIGNAL (timeout   (int,long long)),
        this           , SLOT   (timeout   (int,long long))
      );
      QObject::connect(
        ConnectionTest , SIGNAL (Finished()),
        this           , SLOT   (Stop    ())
      );
      ConnectionTest->Start();
    break;
    case nDnsWidget::Speed      : break;
    case nDnsWidget::Quality    : break;
    case nDnsWidget::Edit       : break;
  };
};

void nDnsWidget::Stop(void)
{
  switch (connectivity)
  {
    case nDnsWidget::Connection :
    if (ConnectionTest!=NULL) {
      // delete ConnectionTest;
      ConnectionTest = NULL;
    };
    break;
    case nDnsWidget::Speed      : break;
    case nDnsWidget::Quality    : break;
    case nDnsWidget::Edit       : break;
  };
};

void nDnsWidget::Connecting(int index,long long timestamp)
{
  switch (connectivity)
  {
    case nDnsWidget::Connection :
      dnsStatus[index] = 0;
      dnsStart [index] = timestamp;
      setTextItem(Items[index],index);
    break;
    case nDnsWidget::Speed      : break;
    case nDnsWidget::Quality    : break;
    case nDnsWidget::Edit       : break;
  };
};

void nDnsWidget::Connected(int index,long long timestamp)
{
  switch (connectivity)
  {
    case nDnsWidget::Connection :
      dnsStatus[index] = 1;
      dnsDone  [index] = timestamp;
      setTextItem(Items[index],index);
    break;
    case nDnsWidget::Speed      : break;
    case nDnsWidget::Quality    : break;
    case nDnsWidget::Edit       : break;
  };
};

void nDnsWidget::Invalid(int index,long long timestamp)
{
  switch (connectivity)
  {
    case nDnsWidget::Connection :
      dnsStatus[index] = 3;
      dnsDone  [index] = timestamp;
      setTextItem(Items[index],index);
    break;
    case nDnsWidget::Speed      : break;
    case nDnsWidget::Quality    : break;
    case nDnsWidget::Edit       : break;
  };
};

void nDnsWidget::timeout(int index,long long timestamp)
{
  switch (connectivity)
  {
    case nDnsWidget::Connection :
      dnsStatus[index] = 2;
      dnsDone  [index] = timestamp;
      setTextItem(Items[index],index);
    break;
    case nDnsWidget::Speed      : break;
    case nDnsWidget::Quality    : break;
    case nDnsWidget::Edit       : break;
  };
};
