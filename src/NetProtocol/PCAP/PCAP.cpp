/////////////////////////////////////////////////////////////////////////
/*               DO NOT INCLUDE THESE IN THE HEADER FILE               */
/////////////////////////////////////////////////////////////////////////
/***********************************************************************\
 * Mix up Qt or Rishon with PCAP header will make compiler very crazy, *
 * isolate these header file here, and do not include the following    *
 * items in the standard header file.                                  *
\***********************************************************************/

#include <QtCore>
#include <QtGui>

#define HAVE_REMOTE 1
#include <pcap.h>
#include <Packet32.h>
#include <Win32-Extensions.h>
#include <bittypes.h>
#include <ip6_misc.h>
#include <pcap-bpf.h>
#include <pcap-namedb.h>
#include <pcap-stdinc.h>

namespace Neutrino
{

namespace PCAP
{

QStringList Devices (void)
{
  QStringList devices                        ;
  pcap_if_t * allAdapters                    ;
  pcap_if_t * adapter                        ;
  char        errorBuffer [PCAP_ERRBUF_SIZE] ;

  // retrieve the adapters from the computer
  if (pcap_findalldevs_ex(
        PCAP_SRC_IF_STRING, NULL,
        &allAdapters, errorBuffer
      ) == -1 ) return devices;

  // if there are no adapters, return
  if (allAdapters == NULL) return devices;

  // print the list of adapters along with basic information about an adapter
  int crtAdapter = 0;
  for (adapter =allAdapters;
       adapter!=NULL;
       adapter =adapter->next)
  {
    devices << QString(adapter->name);
    qDebug("\n%d.%s ", ++crtAdapter, adapter->name);
    qDebug( "-- %s\n", adapter->description);
  };

  // free the adapter list
  pcap_freealldevs( allAdapters );

  return devices;
}

}

}
