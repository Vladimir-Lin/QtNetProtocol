NAME         = NetProtocol
TARGET       = $${NAME}

QT           = core
QT          -= gui
QT          += network
QT          += sql
QT          += script
QT          += Essentials

load(qt_build_config)
load(qt_module)

INCLUDEPATH += $${PWD}/../../include/$${NAME}

HEADERS     += $${PWD}/../../include/$${NAME}/netprotocol.h

include ($${PWD}/Base/Base.pri)
include ($${PWD}/DNS/DNS.pri)
include ($${PWD}/Hardware/Hardware.pri)
include ($${PWD}/Mail/Mail.pri)
include ($${PWD}/NNTP/NNTP.pri)
include ($${PWD}/P2P/P2P.pri)
include ($${PWD}/PCAP/PCAP.pri)
include ($${PWD}/Protocols/Protocols.pri)
include ($${PWD}/XML/XML.pri)

OTHER_FILES += $${PWD}/../../include/$${NAME}/headers.pri

include ($${PWD}/../../doc/Qt/Qt.pri)

TRNAME       = $${NAME}
include ($${PWD}/../../Translations.pri)

win32 {
LIBS        += -lws2_32
}
