#include <Rishon>

nNetworkInferfaceCards:: nNetworkInferfaceCards(QWidget * parent)
                      :  QComboBox             (          parent)
{
  Initialize();
}

nNetworkInferfaceCards::~nNetworkInferfaceCards(void)
{
}

void nNetworkInferfaceCards::Initialize(void)
{
  QStringList devices = Neutrino::PCAP::Devices() ;
  if (devices.size()<=0)                          {
    addItem(tr("No Network Devices"))             ;
    setEnabled(false)                             ;
  } else addItems(devices)                        ;
}
