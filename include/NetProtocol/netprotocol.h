/****************************************************************************
 *                                                                          *
 * Copyright (C) 2001 ~ 2016 Neutrino International Inc.                    *
 *                                                                          *
 * Author : Brian Lin <lin.foxman@gmail.com>, Skype: wolfram_lin            *
 *                                                                          *
 ****************************************************************************/

#ifndef QT_NETPROTOCOL_H
#define QT_NETPROTOCOL_H

#include <QtCore>
#include <QtNetwork>
#include <QtSql>
#include <QtScript>
#include <Essentials>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#    if defined(QT_BUILD_NETPROTOCOL_LIB)
#      define Q_NETPROTOCOL_EXPORT Q_DECL_EXPORT
#    else
#      define Q_NETPROTOCOL_EXPORT Q_DECL_IMPORT
#    endif
#else
#      define Q_NETPROTOCOL_EXPORT
#endif

namespace N
{

class Q_NETPROTOCOL_EXPORT NetProtocol     ;
class Q_NETPROTOCOL_EXPORT NetTalk         ;
class Q_NETPROTOCOL_EXPORT NetConnector    ;
class Q_NETPROTOCOL_EXPORT FindConnector   ;
class Q_NETPROTOCOL_EXPORT NetServer       ;
class Q_NETPROTOCOL_EXPORT TcpConnector    ;
class Q_NETPROTOCOL_EXPORT TcpServer       ;
class Q_NETPROTOCOL_EXPORT TcpService      ;
class Q_NETPROTOCOL_EXPORT UdpConnector    ;
class Q_NETPROTOCOL_EXPORT UdpServer       ;
class Q_NETPROTOCOL_EXPORT UdpService      ;
class Q_NETPROTOCOL_EXPORT UdpBroadcast    ;
class Q_NETPROTOCOL_EXPORT UdpDiscovery    ;
class Q_NETPROTOCOL_EXPORT CiosNeighbors   ;
class Q_NETPROTOCOL_EXPORT CiosDiscovery   ;
class Q_NETPROTOCOL_EXPORT DnsProtocol     ;
class Q_NETPROTOCOL_EXPORT HttpParser      ;
class Q_NETPROTOCOL_EXPORT ScriptableHttp  ;
class Q_NETPROTOCOL_EXPORT XmlCursor       ;
class Q_NETPROTOCOL_EXPORT XmlVisitor      ;
class Q_NETPROTOCOL_EXPORT XmlBase         ;
class Q_NETPROTOCOL_EXPORT XmlParsing      ;
class Q_NETPROTOCOL_EXPORT XmlAttribute    ;
class Q_NETPROTOCOL_EXPORT XmlAttributeSet ;
class Q_NETPROTOCOL_EXPORT XmlNode         ;
class Q_NETPROTOCOL_EXPORT XmlElement      ;
class Q_NETPROTOCOL_EXPORT XmlComment      ;
class Q_NETPROTOCOL_EXPORT XmlUnknown      ;
class Q_NETPROTOCOL_EXPORT XmlText         ;
class Q_NETPROTOCOL_EXPORT XmlDeclaration  ;
class Q_NETPROTOCOL_EXPORT XmlDocument     ;
class Q_NETPROTOCOL_EXPORT XmlHandle       ;
class Q_NETPROTOCOL_EXPORT XmlParser       ;
class Q_NETPROTOCOL_EXPORT ScriptableXml   ;

namespace XML
{

Q_NETPROTOCOL_EXPORT extern const char * UserAgents[] ;
Q_NETPROTOCOL_EXPORT QStringList GetUserAgents  (void) ;
Q_NETPROTOCOL_EXPORT QString     RandUserAgents (void) ;

}

/*****************************************************************************\
 *                                                                           *
 *                            Generic Socket Interface                       *
 *                                                                           *
\*****************************************************************************/

class Q_NETPROTOCOL_EXPORT NetProtocol
{ // Normally, this is an interpreter
  public:

    enum               {
      CanRead  = 10001 ,
      CanWrite = 10002
    }                  ;

    typedef enum    {
      Pending = 0   ,
      Input   = 1   ,
      Output  = 2   }
      DataDirection ;

    QString              Carry      ;
    DecisionTree         Decisions  ;
    WMAPs                Parameters ;
    QDateTime            Stamp      ;
    QDateTime            Coda       ;
    QMap<int,QByteArray> Buffers    ;

    explicit             NetProtocol  (void) ;
    virtual             ~NetProtocol  (void) ;

    virtual int          type         (void) const = 0 ; // Normally, the lastest RFC number

    virtual bool         In           (int size,char * data) = 0 ;
    virtual bool         In           (QByteArray & data) = 0 ;
    virtual bool         Out          (QByteArray & data) = 0 ;

    virtual int          setBuffer    (int index,int size) ;
    virtual void         setParameter (QString name,QVariant value) ;
    virtual void         setCondition (SUID id,bool cond) ;
    virtual QByteArray & Data         (int index) ;

    virtual bool         contains     (QString key) ;
    virtual QVariant   & value        (QString key) ;

    virtual void         Place        (QString command) ;

    virtual int          Response     (void) ;
    virtual bool         Take         (QByteArray & line) ;
    virtual bool         Tail         (QByteArray & line) ;
    virtual bool         Interpret    (int code,QByteArray & line) ;

    virtual bool         Actions      (void) ;
    virtual bool         Decide       (void) ;
    virtual bool         Process      (UUIDs & reactions) ;

  protected:

  private:

};

class Q_NETPROTOCOL_EXPORT NetTalk
{
  public:

    enum            {
      Chunk     = 0 ,
      LineFeed  = 1 ,
      Structure = 2
    }               ;

    enum               {
      NetIdle      = 0 ,
      NetError     = 1 ,
      NetConnected = 2 ,
      NetLogin     = 3 ,
      NetLogout    = 4 ,
      NetReading   = 5 ,
      NetWriting   = 6
    }                  ;

    IpAddress     Address     ;
    IpAddresses   Alternates  ;
    Socket        Connector   ;
    DecisionTable Decision    ;
    WMAPs         Parameters  ;
    QDateTime     Stamp       ;
    QDateTime     Coda        ;
    NetProtocol * Interpreter ;

    explicit     NetTalk       (void) ;
    virtual     ~NetTalk       (void) ;

    virtual bool Lookup        (QString hostname) ;
    virtual void setParameter  (QString name,QVariant value) ;
    virtual void setCondition  (SUID id,bool cond) ;

    virtual bool Reset         (void) ;
    virtual bool Connect       (void) ;
    virtual bool isConnected   (TUID usec = 0) ;
    virtual bool Close         (void) ;

    virtual bool Decide        (void) ;
    virtual bool Process       (UUIDs & reactions) ;

    virtual bool ReadChunk     (void) ;
    virtual bool ReadLine      (void) ;
    virtual bool ReadStructure (void) ;

    virtual bool WriteCommand  (void) ;

    bool         Until         (void) ;
    void         setUntil      (int msec) ;
    void         SyncTime      (void) ;

    virtual void AccessTime    (void) ;
    virtual void Intermediate  (bool & sleeping,bool & duty) ;
    virtual void AutoDeletion  (void) ;
    virtual void Prepend       (void) ;

  protected:

    void         setSocketType (QString socketType) ;

  private:

};

class Q_NETPROTOCOL_EXPORT NetConnector : public NetTalk
{
  public:

    explicit NetConnector     (void) ;
    virtual ~NetConnector     (void) ;

    // Normally, the lastest RFC number
    virtual int  type         (void) const = 0 ;

    virtual bool BothChannels (void) ;
    virtual bool ReadChannel  (void) ;
    virtual bool WriteChannel (void) ;
    virtual bool AtChannel    (int channels) ;
    virtual bool AssignHost   (void) ;

  protected:

    virtual void Prepared     (int flags) ;
    virtual void Started      (int flags) ;
    virtual void Finished     (int flags) ;

  private:

};

class Q_NETPROTOCOL_EXPORT FindConnector
{
  public:

    explicit FindConnector (void) ;
    virtual ~FindConnector (void) ;

    virtual NetConnector * Allocator (void) = 0 ;

  protected:

  private:

};

class Q_NETPROTOCOL_EXPORT NetServer : public NetTalk
{
  public:

    explicit NetServer                     (void) ;
    virtual ~NetServer                     (void) ;

    // Normally, the lastest RFC number
    virtual int type                       (void) const = 0 ;

    virtual void Listening                 (void) ;

    virtual void connectorParameter        (QString name,QVariant value) ;
    virtual void connectorCondition        (SUID id,bool cond) ;

    void         setFinder                 (FindConnector * finder) ;

  protected:

    QMap<int,NetConnector *> Connectors ;
    QMap<NetConnector *,int> Reversals  ;
    FindConnector          * Finder     ;

    virtual void Prepared                  (void) ;
    virtual void Started                   (void) ;
    virtual void Accepting                 (void) ;
    virtual void Finished                  (void) ;

    virtual NetConnector * GetConnector    (void) ;
    int                    addConnector    (NetConnector * connector) ;
    int                    deleteConnector (int id) ;
    virtual bool           startConnector  (NetConnector * connector) ;

  private:

};

class Q_NETPROTOCOL_EXPORT TcpConnector : public NetConnector
{
  public:

    explicit TcpConnector (void) ;
    virtual ~TcpConnector (void) ;

    virtual int type      (void) const ; // Normally, the lastest RFC number

  protected:

  private:

};

class Q_NETPROTOCOL_EXPORT TcpServer : public NetServer
{
  public:

    explicit TcpServer (void) ;
    virtual ~TcpServer (void) ;

    virtual int type   (void) const ; // Normally, the lastest RFC number

  protected:

  private:

};

class Q_NETPROTOCOL_EXPORT TcpService : public TcpServer
                                      , public Thread
{
  public:

    explicit TcpService         (void) ;
    virtual ~TcpService         (void) ;

    virtual int  type           (void) const ;

  protected:

    virtual void run            (int Type,ThreadData * Data) ;
    virtual void run            (void) ;
    virtual bool startConnector (NetConnector * connector) ;

  private:

};

class Q_NETPROTOCOL_EXPORT UdpConnector : public NetConnector
{
  public:

    explicit UdpConnector (void) ;
    virtual ~UdpConnector (void) ;

    virtual int type      (void) const ; // Normally, the lastest RFC number

  protected:

  private:

};

class Q_NETPROTOCOL_EXPORT UdpServer : public NetServer
{
  public:

    explicit UdpServer (void) ;
    virtual ~UdpServer (void) ;

    virtual int type   (void) const ; // Normally, the lastest RFC number

  protected:

  private:

};

class Q_NETPROTOCOL_EXPORT UdpService : public UdpServer
                                      , public Thread
{
  public:

    explicit UdpService         (void) ;
    virtual ~UdpService         (void) ;

    virtual int  type           (void) const ;

  protected:

    virtual void run            (int Type,ThreadData * Data) ;
    virtual void run            (void) ;
    virtual bool startConnector (NetConnector * connector) ;

  private:

};

class Q_NETPROTOCOL_EXPORT UdpBroadcast : public Thread
{
  public:

    NetProtocol * Interpreter ;

    explicit     UdpBroadcast     (void) ;
    virtual     ~UdpBroadcast     (void) ;

    virtual int  BroadcastSocket  (void) ;
    virtual void Probe            (void) ;
    virtual void Bind             (void) ;
    virtual void Broadcast        (void) ;

  protected:

    virtual void run              (int Type,ThreadData * Data) ;
    virtual void setNonblock      (int socket) ;
    virtual bool uselect          (int socket,long long usec) ;

    void         AnyAddress       (struct sockaddr_in & a,int port) ;
    void         BroadcastAddress (struct sockaddr_in & a,int port) ;

  private:

    void         Deletion         (void) ;
    void         Intermediate     (void) ;
    void         CloseUDP         (int socket) ;

};

class Q_NETPROTOCOL_EXPORT UdpDiscovery : public NetProtocol
{
  public:

    explicit     UdpDiscovery (void) ;
    virtual     ~UdpDiscovery (void) ;

    virtual int  type         (void) const ;

    virtual bool In           (int size,char * data) ;
    virtual bool In           (QByteArray & data) ;
    virtual bool Out          (QByteArray & data) ;

    virtual bool Actions      (void) ;
    virtual bool Interpret    (int code,QByteArray & line) ;

  protected:

    virtual bool Analysis     (void) ;

  private:

};

class Q_NETPROTOCOL_EXPORT CiosNeighbors : public QObject
{
  Q_OBJECT
  public:

    QStringList             Hostnames      ;
    KMAPs                   Applications   ;
    KMAPs                   Addresses      ;
    UMAPs                   Status         ;
    QMap<QString,QDateTime> Lastest        ;
    QMap<QString,QStrings > OtherAddresses ;
    int                     TimeOut        ;
    int                     Used           ;

    explicit            CiosNeighbors  (QObject * parent = NULL) ;
    virtual            ~CiosNeighbors  (void) ;

    virtual void        Clear          (void) ;

    virtual QStringList Alive          (void) ;
    virtual QStringList Neighbors      (void) ;

    virtual void        Update         (QString address,QString host,QString app) ;
    virtual void        Others         (QString host,QStringList & addresses) ;
    virtual void        Verify         (void) ;
    virtual void        Take           (QString host) ;

  protected:

    QMutex mutex ;

    virtual void        Changes        (void) ;

  private:

  public slots:

  protected slots:

    virtual void        PrivateChanges (void) ;

  private slots:

  signals:

    void                PrivateChanged (void) ;
    void                Changed        (void) ;

};

class Q_NETPROTOCOL_EXPORT CiosDiscovery : public UdpDiscovery
{
  public:

    CiosNeighbors * neighbors ;

    explicit     CiosDiscovery (void) ;
    virtual     ~CiosDiscovery (void) ;

    virtual bool Interpret     (int code,QByteArray & line) ;

  protected:

    QString      LocalAddress  (void) ;

    virtual void PlaceAddress  (int code) ;
    virtual void AcceptAddress (QString address) ;

  private:

};

/*****************************************************************************\
 *                                                                           *
 *                         Domain Name System / DNS                          *
 *                                                                           *
\*****************************************************************************/

class Q_NETPROTOCOL_EXPORT DnsProtocol : public NetProtocol
{ // DNS protocol interpreter
  public:

    explicit DnsProtocol (void) ;
    virtual ~DnsProtocol (void) ;

    virtual int  type    (void) const ; // RFC 1035

    virtual bool In      (int size,char * data) ;
    virtual bool In      (QByteArray & data) ;
    virtual bool Out     (QByteArray & data) ;

  protected:

  private:

};

/*****************************************************************************\
 *                                                                           *
 *                                   HTTP                                    *
 *                                                                           *
\*****************************************************************************/

class Q_NETPROTOCOL_EXPORT HttpParser
{
  public:

    WMAPs      Tags   ;
    QByteArray Header ;

    explicit        HttpParser       (void) ;
                    HttpParser       (const HttpParser & parser) ;
                    HttpParser       (QByteArray & data) ;
    virtual        ~HttpParser       (void) ;

    HttpParser    & operator =       (const HttpParser & parser) ;
    QStringList     keys             (void) const ;

    void            clear            (void) ;
    int             setHeader        (QByteArray & data) ;
    bool            hasHttpHeader    (void) ;
    int             ConfirmHttp      (void) ;
    int             ParseItem        (int index) ;
    bool            EndOfHeader      (int & index) ;
    int             NextNotBlank     (int index) ;
    int             LineEnd          (int index) ;

    int             statusCode       (void) const ;
    bool            hasContentType   (void) const ;
    QString         contentType      (void) const ;
    bool            hasMimeType      (void)       ;
    QString         mimeType         (void)       ;
    bool            hasEncoding      (void)       ;
    QString         Encoding         (void)       ;
    TUID            contentLength    (void) const ;
    bool            hasContentLength (void) const ;
    bool            isValid          (void) const ;
    virtual QString toString         (void) const ;
    bool            hasKey           (const QString & key) const ;
    QString         value            (const QString & key) const ;
    QDateTime       toDateTime       (const QString & key) ;

  protected:

  private:

} ;

class Q_NETPROTOCOL_EXPORT ScriptableHttp : public QObject
                                          , public QScriptable
                                          , public HttpParser
{
  Q_OBJECT
  public:

    explicit          ScriptableHttp   (QObject * parent = NULL) ;
    virtual          ~ScriptableHttp   (void) ;

  protected:

  private:

  public slots:

    virtual void      Clear            (void) ;
    virtual int       SetHeader        (ScriptableByteArray & data) ;
    virtual bool      HasHttpHeader    (void) ;
    virtual int       IsConfirmHttp    (void) ;
    virtual int       DoParseItem      (int index) ;
    virtual bool      IsEndOfHeader    (int & index) ;
    virtual int       GetNextNotBlank  (int index) ;
    virtual int       GetLineEnd       (int index) ;
    virtual int       StatusCode       (void) const ;
    virtual bool      HasContentType   (void) const ;
    virtual QString   ContentType      (void) const ;
    virtual bool      HasMimeType      (void)       ;
    virtual QString   MimeType         (void)       ;
    virtual bool      HasEncoding      (void)       ;
    virtual QString   GetEncoding      (void)       ;
    virtual TUID      ContentLength    (void) const ;
    virtual bool      HasContentLength (void) const ;
    virtual bool      IsValid          (void) const ;
    virtual QString   ToString         (void) const ;
    virtual int       KeysCount        (void) ;
    virtual void      setValue         (QString key,QString v) ;
    virtual bool      HasKey           (const QString key) const ;
    virtual QString   Value            (const QString key) const ;
    virtual QDateTime ToDateTime       (const QString key) ;

  protected slots:

  private slots:

  signals:

} ;

/*****************************************************************************\
 *                                                                           *
 *                         Fault tolerant XML Parser                         *
 *                                                                           *
\*****************************************************************************/

class Q_NETPROTOCOL_EXPORT XmlCursor
{
  public:

    int row ; // 0 based.
    int col ; // 0 based.

    explicit XmlCursor (void) ;
    virtual ~XmlCursor (void) ;

    void clear         (void) ;

  protected:

  private:

};

class Q_NETPROTOCOL_EXPORT XmlVisitor
{
  public:

    explicit XmlVisitor     (void) ;
    virtual ~XmlVisitor     (void) ;

    virtual bool VisitEnter (const XmlDocument    & doc        ) ;
    virtual bool VisitExit  (const XmlDocument    & doc        ) ;

    virtual bool VisitEnter (const XmlElement     & element,const XmlAttribute * firstAttribute) ;
    virtual bool VisitExit  (const XmlElement     & element    ) ;

    virtual bool Visit      (const XmlDeclaration & declaration) ;
    virtual bool Visit      (const XmlText        & text       ) ;
    virtual bool Visit      (const XmlComment     & comment    ) ;
    virtual bool Visit      (const XmlUnknown     & unknown    ) ;

  protected:

  private:

};

class Q_NETPROTOCOL_EXPORT XmlParsing
{
  friend class XmlDocument ;

  public:

    explicit XmlParsing      (void) ;
    virtual ~XmlParsing      (void) ;

    const XmlCursor & Cursor (void) const ;
    void  Stamp              (const char * now,XML::Encoding encoding) ;

  protected:

  private:

    XmlCursor    cursor  ;
    const char * stamp   ;
    int          tabsize ;

    XmlParsing (const char * start,int _tabsize,int row,int col) ;

};

class Q_NETPROTOCOL_EXPORT XmlBase
{
  friend class XmlNode     ;
  friend class XmlElement  ;
  friend class XmlDocument ;

  public:

    explicit XmlBase (void) ;
    virtual ~XmlBase (void) ;

    static void  SetCondenseWhiteSpace (bool condense) ;
    static bool  IsWhiteSpaceCondensed (void) ;

    int          Row                   (void) const  ;
    int          Column                (void) const  ;

    void         SetUserData           (void * user) ;
    void       * GetUserData           (void)        ;
    const void * GetUserData           (void) const  ;

    static const int utf8ByteTable[256];

    virtual const char * Parse(const char  * p     ,
                               XmlParsing  * data  ,
                               XML::Encoding encoding /* = Unknown */ ) = 0;

    static void EncodeString(const std::string & str,std::string * out) ;

    enum ErrorCode                                {
      NoError                              =  0   ,
      XmlError                             =  1   ,
      XmlError_Opening_File                =  2   ,
      XmlError_Parsing_Element             =  3   ,
      XmlError_Failed_To_Read_Element_Name =  4   ,
      XmlError_Reading_Element_Value       =  5   ,
      XmlError_Reading_Attributes          =  6   ,
      XmlError_Parsing_Empty               =  7   ,
      XmlError_Reading_End_Tag             =  8   ,
      XmlError_Parsing_Unknown             =  9   ,
      XmlError_Parsing_Comment             = 10   ,
      XmlError_Parsing_Declaration         = 11   ,
      XmlError_Document_Empty              = 12   ,
      XmlError_Embedded_NULL               = 13   ,
      XmlError_Parsing_CDATA               = 14   ,
      XmlError_Document_Top_Only           = 15   ,
      XmlError_String_Count                = 16 } ;

  protected:

    static const char * errorString [ XmlError_String_Count ] ;
    XmlCursor           location ;
    void              * userData ;

    static bool         empty          (const char * p) ;
    static bool         exists         (const char * p) ;

    static const char * SkipWhiteSpace (const char * p,XML::Encoding encoding);
    static bool         IsWhiteSpace   (char c) ;
    static bool         IsWhiteSpace   (int  c);

    static const char * ReadName       (const char * p,std::string * name,XML::Encoding encoding) ;

    static const char * ReadText           (
            const char  * in               , // where to start
            std::string * text             , // the string read
            bool          ignoreWhiteSpace , // whether to keep the white space
            const char  * endTag           , // what ends this text
            bool          ignoreCase       , // whether to ignore case in the end tag
            XML::Encoding encoding       ) ;// the current encoding

    static const char * GetEntity             (
            const char            * in        ,
            char                  * value     ,
            int                   * length    ,
            XML::Encoding encoding) ;

    static const char * GetChar               (
            const char            * p         ,
            char                  * _value    ,
            int                   * length    ,
            XML::Encoding encoding) ;

    static bool StringEqual                    (
            const char            * p          ,
            const char            * endTag     ,
            bool                    ignoreCase ,
            XML::Encoding encoding ) ;

    static int  IsAlpha            (unsigned char anyByte,XML::Encoding encoding);
    static int  IsAlphaNum         (unsigned char anyByte,XML::Encoding encoding);
    static int  ToLower            (int v,XML::Encoding encoding);
    static void ConvertUTF32ToUTF8 (unsigned long input,char* output,int* length);

  private:

    struct Entity
    {
      const char * str       ;
      unsigned int strLength ;
      char         chr       ;
    };

    enum {
        NUM_ENTITY        = 5 ,
        MAX_ENTITY_LENGTH = 6
    };

    static Entity entity[ NUM_ENTITY ];
    static bool condenseWhiteSpace;

};

class Q_NETPROTOCOL_EXPORT XmlAttribute : public XmlBase
{
  friend class XmlAttributeSet ;

  public:

    explicit XmlAttribute                (void) ;
    explicit XmlAttribute                (const char * _name,const char * _value) ;
    virtual ~XmlAttribute                (void) ;

    const char *        Name             (void) const ;
    const char *        Value            (void) const ;
    int                 IntValue         (void) const ;
    double              DoubleValue      (void) const ;

    const std::string & NameTStr         (void) const ;

    int                 QueryIntValue    (int    * _value) const ;
    int                 QueryDoubleValue (double * _value) const ;

    void                SetName          (const char * _name ) ;
    void                SetValue         (const char * _value) ;

    void                SetIntValue      (int    _value) ;
    void                SetDoubleValue   (double _value) ;

    const XmlAttribute * Next            (void) const ;
          XmlAttribute * Next            (void)       ;

    const XmlAttribute * Previous        (void) const ;
          XmlAttribute * Previous        (void)       ;

    bool operator == (const XmlAttribute & rhs ) const ;
    bool operator  < (const XmlAttribute & rhs ) const ;
    bool operator  > (const XmlAttribute & rhs ) const ;

    virtual const char * Parse (const char * p,XmlParsing * data,XML::Encoding encoding) ;

    void SetDocument (XmlDocument * doc);

  protected:

  private:

    std::string    name     ;
    std::string    value    ;
    XmlDocument  * document ;
    XmlAttribute * prev     ;
    XmlAttribute * next     ;

};

class Q_NETPROTOCOL_EXPORT XmlAttributeSet
{
  public:

    explicit XmlAttributeSet    (void) ;
    virtual ~XmlAttributeSet    (void) ;

    void                 Add    (XmlAttribute * attribute) ;
    void                 Remove (XmlAttribute * attribute) ;

    const XmlAttribute * First  (void) const ;
          XmlAttribute * First  (void)       ;
    const XmlAttribute * Last   (void) const ;
          XmlAttribute * Last   (void)       ;

    XmlAttribute * Find         (const char * _name) const ;
    XmlAttribute * FindOrCreate (const char * _name)       ;

  protected:

  private:

    XmlAttribute sentinel;

};

class Q_NETPROTOCOL_EXPORT XmlNode : public XmlBase
{
  friend class XmlDocument ;
  friend class XmlElement  ;

  public:

    enum NodeType          {
      xmlDocument    = 0   ,
      xmlElement     = 1   ,
      xmlComment     = 2   ,
      xmlUnknown     = 3   ,
      xmlText        = 4   ,
      xmlDeclaration = 5   ,
      xmlTypeCount   = 6 } ;

    explicit XmlNode (void);
    virtual ~XmlNode (void);

    bool                isTag     (QString tag) ;
    QString             Tag       (void) const ;
    const char        * Value     (void) const ;
    const std::string & ValueTStr (void) const ;
    void                SetValue  (const char * _value) ;
    void                Clear     (void);

          XmlNode * Parent     (void)       ;
    const XmlNode * Parent     (void) const ;
    const XmlNode * FirstChild (void) const ;

          XmlNode * FirstChild (void)       ;
    const XmlNode * FirstChild (const char *  value) const ;
          XmlNode * FirstChild (const char * _value)       ;

    const XmlNode * LastChild  (void) const ;
          XmlNode * LastChild  (void)       ;

    const XmlNode * LastChild  (const char *  value) const ;
          XmlNode * LastChild  (const char * _value)       ;

    const XmlNode * IterateChildren (const XmlNode * previous) const ;
          XmlNode * IterateChildren (const XmlNode * previous)       ;

    const XmlNode * IterateChildren (const char *  value,const XmlNode * previous) const ;
          XmlNode * IterateChildren (const char * _value,const XmlNode * previous)       ;

    XmlNode       * InsertEndChild    ( const XmlNode & addThis                          ) ;
    XmlNode       * LinkEndChild      ( XmlNode * addThis                                ) ;
    XmlNode       * InsertBeforeChild ( XmlNode * beforeThis  , const XmlNode & addThis  ) ;
    XmlNode       * InsertAfterChild  ( XmlNode * afterThis   , const XmlNode & addThis  ) ;
    XmlNode       * ReplaceChild      ( XmlNode * replaceThis , const XmlNode & withThis ) ;

    bool            RemoveChild       ( XmlNode * removeThis ) ;

    const XmlNode * PreviousSibling   (void) const ;
          XmlNode * PreviousSibling   (void)       ;

    const XmlNode * PreviousSibling   (const char *     ) const ;
          XmlNode * PreviousSibling   (const char *_prev)       ;

    const XmlNode * NextSibling       (void) const ;
          XmlNode * NextSibling       (void)       ;

    const XmlNode * NextSibling       (const char *      ) const ;
          XmlNode * NextSibling       (const char * _next)       ;

    const XmlElement * NextSiblingElement (void) const ;
          XmlElement * NextSiblingElement (void);

    const XmlElement * NextSiblingElement (const char *     ) const ;
          XmlElement * NextSiblingElement (const char *_next)       ;

    const XmlElement * FirstChildElement (void) const ;
          XmlElement * FirstChildElement (void)       ;

    const XmlElement * FirstChildElement (const char * _value) const ;
          XmlElement * FirstChildElement (const char * _value)       ;

    int Type(void) const;

    const XmlDocument * GetDocument (void) const ;
          XmlDocument * GetDocument (void)       ;

    bool NoChildren (void) const ;

    virtual const XmlDocument    * ToDocument    (void) const ;
    virtual const XmlElement     * ToElement     (void) const ;
    virtual const XmlComment     * ToComment     (void) const ;
    virtual const XmlUnknown     * ToUnknown     (void) const ;
    virtual const XmlText        * ToText        (void) const ;
    virtual const XmlDeclaration * ToDeclaration (void) const ;

    virtual XmlDocument    * ToDocument    (void) ;
    virtual XmlElement     * ToElement     (void) ;
    virtual XmlComment     * ToComment     (void) ;
    virtual XmlUnknown     * ToUnknown     (void) ;
    virtual XmlText        * ToText        (void) ;
    virtual XmlDeclaration * ToDeclaration (void) ;

    virtual XmlNode * Clone (void) const = 0;

    virtual bool Accept (XmlVisitor * visitor) const = 0;

  protected:

    explicit XmlNode (NodeType _type);

    void       CopyTo   (XmlNode * target) const ;

    XmlNode * Identify (const char * start,XML::Encoding encoding) ;

    NodeType    type       ;
    std::string value      ;
    XmlNode   * parent     ;
    XmlNode   * firstChild ;
    XmlNode   * lastChild  ;
    XmlNode   * prev       ;
    XmlNode   * next       ;

  private:

};

class Q_NETPROTOCOL_EXPORT XmlElement : public XmlNode
{
  public:

    explicit XmlElement        (const char       * in_value) ;
    explicit XmlElement        (const XmlElement &         ) ;
    virtual ~XmlElement        (void                       ) ;

    virtual bool NoEndTag      (std::string tag,XML::Encoding encoding) ;
    virtual bool IgnoreBalance (std::string tag,XML::Encoding encoding) ;

    XmlElement & operator =    (const XmlElement & base    ) ;

    const char * Attribute     (const char * name                  ) const ;
    const char * Attribute     (const char * name,int      * i     ) const ;
    const char * Attribute     (const char * name,double   * d     ) const ;

    int QueryIntAttribute      (const char * name,int      * _value) const ;
    int QueryUnsignedAttribute (const char * name,unsigned * _value) const ;
    int QueryBoolAttribute     (const char * name,bool     * _value) const ;
    int QueryDoubleAttribute   (const char * name,double   * _value) const ;
    int QueryFloatAttribute    (const char * name,float    * _value) const ;

    void SetAttribute          (const char * name,const char * _value) ;
    void SetAttribute          (const char * name,int           value) ;

    void SetDoubleAttribute    (const char * name, double       value) ;
    void RemoveAttribute       (const char * name                    ) ;

    const XmlAttribute * FirstAttribute (void) const ;
          XmlAttribute * FirstAttribute (void)       ;
    const XmlAttribute * LastAttribute  (void) const ;
          XmlAttribute * LastAttribute  (void)       ;

    const char * GetText (void) const ;

    virtual XmlNode * Clone (void) const ;

    virtual const char * Parse(const char * p,XmlParsing * data,XML::Encoding encoding) ;

    virtual const XmlElement * ToElement (void) const ;
    virtual       XmlElement * ToElement (void)       ;

    virtual bool Accept ( XmlVisitor * visitor ) const ;

  protected:

    void CopyTo    ( XmlElement * target ) const ;
    void ClearThis ( void                )       ;

    const char * ReadValue (const char * in,XmlParsing * prevData,XML::Encoding encoding) ;

  private:

    XmlAttributeSet attributeSet ;

};

class Q_NETPROTOCOL_EXPORT XmlComment : public XmlNode
{
  public:

    explicit XmlComment     ( void                      ) ;
    explicit XmlComment     ( const char       * _value ) ;
    explicit XmlComment     ( const XmlComment &        ) ;
    virtual ~XmlComment     (                           ) ;

    XmlComment & operator = ( const XmlComment & base   ) ;

    virtual XmlNode * Clone ( void ) const ;

    virtual const char * Parse ( const char * p,XmlParsing * data,XML::Encoding encoding ) ;

    virtual const XmlComment *  ToComment(void) const ;
    virtual       XmlComment *  ToComment(void)       ;

    virtual bool Accept ( XmlVisitor * visitor ) const ;

  protected:

    void CopyTo ( XmlComment * target ) const ;

  private:

};

class Q_NETPROTOCOL_EXPORT XmlText : public XmlNode
{
  friend class XmlElement ;

  public:

    explicit XmlText     ( const char * initValue ) ;
    explicit XmlText     ( const XmlText & copy  ) ;
    virtual ~XmlText     ( void                   ) ;

    XmlText & operator = ( const XmlText & base  ) ;

    bool CDATA    ( void        ) const ;
    void SetCDATA ( bool _cdata )       ;

    virtual const char * Parse (const char * p,XmlParsing * data,XML::Encoding encoding) ;

    virtual const XmlText * ToText (void) const ;
    virtual       XmlText * ToText (void)       ;

    virtual bool Accept ( XmlVisitor * content ) const ;

  protected :

    virtual XmlNode * Clone (void) const ;
    void    CopyTo( XmlText * target ) const ;

    bool Blank (void) const ;

  private:

    bool cdata       ;
    bool script_bool ;

};

class Q_NETPROTOCOL_EXPORT XmlDeclaration : public XmlNode
{
  public:

    explicit XmlDeclaration (void)       ;
    explicit XmlDeclaration              (
               const char * _version      ,
               const char * _encoding     ,
               const char * _standalone ) ;
    explicit XmlDeclaration (const XmlDeclaration & copy) ;
    virtual ~XmlDeclaration (void                       ) ;

    XmlDeclaration & operator = ( const XmlDeclaration & copy ) ;

    const char * Version     (void) const ;
    const char * Encoding    (void) const ;
    const char * Standalone  (void) const ;

    virtual XmlNode * Clone (void) const ;

    virtual const char * Parse ( const char * p,XmlParsing * data,XML::Encoding encoding ) ;

    virtual const XmlDeclaration * ToDeclaration ( void ) const ;
    virtual       XmlDeclaration * ToDeclaration ( void ) ;

    virtual bool Accept ( XmlVisitor* visitor ) const;

  protected:

    void CopyTo ( XmlDeclaration * target ) const;

  private:

    std::string version    ;
    std::string encoding   ;
    std::string standalone ;

};

class Q_NETPROTOCOL_EXPORT XmlUnknown : public XmlNode
{
  public:

    explicit XmlUnknown     ( void                    ) ;
    explicit XmlUnknown     ( const XmlUnknown & copy ) ;
    virtual ~XmlUnknown     ( void                    ) ;

    XmlUnknown & operator = ( const XmlUnknown & copy ) ;

    virtual XmlNode * Clone (void) const ;

    virtual const char * Parse ( const char * p,XmlParsing * data,XML::Encoding encoding ) ;

    virtual const XmlUnknown * ToUnknown (void) const ;
    virtual       XmlUnknown * ToUnknown (void)       ;

    virtual bool Accept ( XmlVisitor * content ) const ;

  protected:

    void CopyTo ( XmlUnknown * target ) const ;

  private:

};

class Q_NETPROTOCOL_EXPORT XmlDocument : public XmlNode
{
  public:

    explicit XmlDocument       ( void                             ) ;
    explicit XmlDocument       ( const char        * documentName ) ;
    explicit XmlDocument       ( const XmlDocument & copy         ) ;
    virtual ~XmlDocument       ( void                             ) ;

    XmlDocument & operator =   ( const XmlDocument & copy ) ;

    virtual const char * Parse ( QByteArray & Body ) ;
    virtual const char * Parse ( const char * p,XmlParsing * data = NULL,XML::Encoding encoding = XML::DefaultEncoding) ;

    virtual const XmlElement  * RootElement (void) const ;
    virtual       XmlElement  * RootElement (void)       ;
    virtual const XmlDocument * ToDocument  (void) const ;
    virtual       XmlDocument * ToDocument  (void)       ;

    void         SetTabSize (int _tabsize) ;
    int          TabSize    (void) const ;

    bool         Error      (void) const ;
    const char * ErrorDesc  (void) const ;
    int          ErrorId    (void) const ;
    int          ErrorRow   (void) const ;
    int          ErrorCol   (void) const ;
    void         ClearError (void)       ;
    void         SetError   (int err,const char * errorLocation,XmlParsing * prevData,XML::Encoding encoding) ;

    virtual bool Accept (XmlVisitor * content) const ;

  protected :

    virtual XmlNode * Clone(void) const ;

  private:

    void CopyTo ( XmlDocument * target ) const ;

    bool        error           ;
    int         errorId         ;
    std::string errorDesc       ;
    int         tabsize         ;
    XmlCursor   errorLocation   ;
    bool        useMicrosoftBOM ;

};

class Q_NETPROTOCOL_EXPORT XmlHandle
{
  public:

             XmlHandle          ( XmlNode * _node = NULL  ) ;
             XmlHandle          ( const XmlHandle &  ref  ) ;
    virtual ~XmlHandle          ( void                    ) ;

    XmlHandle operator =        ( const XmlHandle &  ref  ) ;

    XmlHandle FirstChild        ( void               ) const ;
    XmlHandle FirstChild        ( const char * value ) const ;
    XmlHandle FirstChildElement ( void               ) const ;
    XmlHandle FirstChildElement ( const char * value ) const ;

    XmlHandle Child             ( const char * value,int index ) const ;
    XmlHandle Child             (                    int index ) const ;
    XmlHandle ChildElement      ( const char * value,int index ) const ;
    XmlHandle ChildElement      (                    int index ) const ;

    XmlNode    * ToNode         (void) const ;
    XmlElement * ToElement      (void) const ;
    XmlText    * ToText         (void) const ;
    XmlUnknown * ToUnknown      (void) const ;

    XmlNode    * Node           (void) const ;
    XmlElement * Element        (void) const ;
    XmlText    * Text           (void) const ;
    XmlUnknown * Unknown        (void) const ;

  private:

    XmlNode * node ;

};

class Q_NETPROTOCOL_EXPORT XmlParser
{
  public:

    int     ErrorCode   ;
    QString ErrorString ;
    int     ErrorRow    ;
    int     ErrorColumn ;

    explicit XmlParser         (void) ;
    virtual ~XmlParser         (void) ;

    bool         hasError      (void) const ;
    virtual bool ParseLinks    (QUrl & Url,QByteArray & Body) ;

  protected:

    QTextCodec * Codec ;

    virtual void setEncoding   (QString encoding                 ) ;
    virtual void AppendTitle   (QString title,QByteArray origin  ) ;
    virtual void AppendLink    (QString name,QUrl origin,QUrl url) ;
    virtual void AppendImage   (QString name,QUrl origin,QUrl url) ;
    virtual void ReportError   (QString text                     ) ;
    virtual void AppendString  (QString text                     ) ;

    virtual bool ParseElement  (QUrl & Url,XmlNode * element) ;
    virtual void HandleElement (QUrl & Url,XmlNode * element) ;

  private:

} ;

class Q_NETPROTOCOL_EXPORT ScriptableXml : public QObject
                                         , public QScriptable
                                         , public XmlParser
{
  Q_OBJECT
  public:

    explicit         ScriptableXml (QObject * parent = NULL) ;
    virtual         ~ScriptableXml (void) ;

  protected:

    WMAPs Variables ;

    virtual void     AppendTitle   (QString title,QByteArray origin  ) ;
    virtual void     AppendLink    (QString name,QUrl origin,QUrl url) ;
    virtual void     AppendImage   (QString name,QUrl origin,QUrl url) ;
    virtual void     ReportError   (QString text                     ) ;
    virtual void     AppendString  (QString text                     ) ;

  private:

  public slots:

    virtual void     setValue      (QString key,QVariant v) ;
    virtual QVariant Value         (QString key) ;
    virtual void     SetEncoding   (QString encoding) ;

    virtual bool     HasError      (void) ;
    virtual bool     Parse         (QString Url,ScriptableByteArray & Body) ;

  protected slots:

  private slots:

  signals:

} ;

Q_NETPROTOCOL_EXPORT QScriptValue AttacheHttp (QScriptContext * context,QScriptEngine * engine) ;
Q_NETPROTOCOL_EXPORT QScriptValue AttacheXml  (QScriptContext * context,QScriptEngine * engine) ;

}

Q_DECLARE_METATYPE(N::TcpConnector)
Q_DECLARE_METATYPE(N::UdpConnector)
Q_DECLARE_METATYPE(N::TcpServer)
Q_DECLARE_METATYPE(N::UdpServer)
Q_DECLARE_METATYPE(N::TcpService)
Q_DECLARE_METATYPE(N::UdpService)
Q_DECLARE_METATYPE(N::UdpBroadcast)
Q_DECLARE_METATYPE(N::UdpDiscovery)
Q_DECLARE_METATYPE(N::CiosDiscovery)
Q_DECLARE_METATYPE(N::DnsProtocol)
Q_DECLARE_METATYPE(N::HttpParser)
Q_DECLARE_METATYPE(N::XmlCursor)
Q_DECLARE_METATYPE(N::XmlVisitor)
Q_DECLARE_METATYPE(N::XmlParsing)
Q_DECLARE_METATYPE(N::XmlAttribute)
Q_DECLARE_METATYPE(N::XmlAttributeSet)
Q_DECLARE_METATYPE(N::XmlComment)
Q_DECLARE_METATYPE(N::XmlUnknown)
Q_DECLARE_METATYPE(N::XmlDeclaration)
Q_DECLARE_METATYPE(N::XmlDocument)
Q_DECLARE_METATYPE(N::XmlHandle)
Q_DECLARE_METATYPE(N::XmlParser)

Q_DECLARE_INTERFACE( N::NetProtocol   , "com.neutrino.network.protocol"  )
Q_DECLARE_INTERFACE( N::NetTalk       , "com.neutrino.network.talk"      )
Q_DECLARE_INTERFACE( N::NetConnector  , "com.neutrino.network.connector" )
Q_DECLARE_INTERFACE( N::NetServer     , "com.neutrino.network.server"    )
Q_DECLARE_INTERFACE( N::FindConnector , "com.neutrino.network.finder"    )

QT_END_NAMESPACE

#endif
