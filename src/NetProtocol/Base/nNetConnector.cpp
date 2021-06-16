#include <netprotocol.h>

N::NetConnector:: NetConnector (void)
                : NetTalk      (    )
{
  setParameter ( "Running"      , true  ) ;
  setParameter ( "Stopped"      , true  ) ;
  setParameter ( "ReadStopped"  , true  ) ;
  setParameter ( "WriteStopped" , true  ) ;
  setParameter ( "Block"        , false ) ;
}

N::NetConnector::~NetConnector (void)
{
}

void N::NetConnector::Prepared (int flags)
{
}

void N::NetConnector::Started (int flags)
{
}

void N::NetConnector::Finished (int flags)
{
}

bool N::NetConnector::AssignHost(void)
{
  return false ;
}

bool N::NetConnector::BothChannels (void)
{
  bool      listening = false                                                ;
  bool      duty      = false                                                ;
  bool      sleeping  = true                                                 ;
  int       state     = 0                                                    ;
  ////////////////////////////////////////////////////////////////////////////
  duty = Parameters [ "Running"   ] . toBool ( )                             ;
  setParameter ( "Stopped"      , false )                                    ;
  setParameter ( "ReadStopped"  , false )                                    ;
  setParameter ( "WriteStopped" , false )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Parameters.contains("State"   ) )                                     {
    state = Parameters [ "State" ] .toInt ( )                                ;
  } else setParameter ( "State" , state ) ;                                  ;
  ////////////////////////////////////////////////////////////////////////////
  AccessTime ( )                                                             ;
  while ( duty )                                                             {
    state = Parameters [ "State" ] .toInt ( )                                ;
    switch ( state )                                                         {
      case   0                                                               :
        state    = 101                                                       ;
        sleeping = false                                                     ;
        setParameter ( "State" , state )                                     ;
        Prepend      (                 )                                     ;
        Prepared     ( 3               )                                     ;
        AccessTime   (                 )                                     ;
      break                                                                  ;
      case 101                                                               :
        state    = 102                                                       ;
        sleeping = true                                                      ;
        AssignHost   (                 )                                     ;
        setParameter ( "State" , state )                                     ;
        AccessTime   (                 )                                     ;
      break                                                                  ;
      case 102                                                               :
        sleeping = true                                                      ;
        if ( Connect ( ) )                                                   {
          state = 103                                                        ;
          AccessTime ( )                                                     ;
        } else                                                               {
          state = 901                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 103                                                               :
        sleeping = true                                                      ;
        if ( isConnected ( 100 ) )                                           {
          state     = 401                                                    ;
          listening = true                                                   ;
          Started    ( 3 )                                                   ;
          AccessTime (   )                                                   ;
        } else                                                               {
          state = 901                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 201                                                               :
        sleeping  = true                                                     ;
        listening = true                                                     ;
        if ( ReadChunk ( ) )                                                 {
          AccessTime ( )                                                     ;
        } else                                                               {
        }                                                                    ;
        state = 401                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 202                                                               :
        sleeping  = true                                                     ;
        listening = true                                                     ;
        if ( ReadLine ( ) )                                                  {
          AccessTime ( )                                                     ;
        } else                                                               {
        }                                                                    ;
        state = 401                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 203                                                               :
        sleeping  = true                                                     ;
        listening = true                                                     ;
        if ( ReadStructure ( ) )                                             {
          AccessTime ( )                                                     ;
        } else                                                               {
        }                                                                    ;
        state = 401                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 301                                                               :
        sleeping  = true                                                     ;
        listening = true                                                     ;
        if ( WriteCommand ( ) )                                              {
          state = 302                                                        ;
        } else                                                               {
          state = 303                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 302                                                               :
        sleeping = true                                                      ;
        Interpreter -> Actions ( )                                           ;
        AccessTime ( )                                                       ;
        state    = 304                                                       ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 303                                                               :
        sleeping = true                                                      ;
        Interpreter -> Actions ( )                                           ;
        state    = 304                                                       ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 304                                                               :
        sleeping = true                                                      ;
        state    = 401                                                       ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 401                                                               :
        sleeping = true                                                      ;
        if ( Interpreter->Data(NetProtocol::Output).size() > 0 )             {
          state = 301                                                        ;
        } else
        if ( Interpreter -> contains ( "State" ) )                           {
          Interpreter -> Actions ( )                                         ;
          state = Interpreter -> value ( "State" ) . toInt ( )               ;
          switch ( state )                                                   {
            case 0                                                           :
              state = 901                                                    ;
            break                                                            ;
            case 1                                                           :
              state = 201                                                    ;
            break                                                            ;
            case 2                                                           :
              state = 202                                                    ;
            break                                                            ;
            case 3                                                           :
              state = 203                                                    ;
            break                                                            ;
          }                                                                  ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 901                                                               :
        sleeping  = true                                                     ;
        listening = false                                                    ;
        Close ( )                                                            ;
        state = 999                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 911                                                               :
        Time :: msleep ( 10 )                                                ;
      break                                                                  ;
      case 999                                                               :
        if ( Parameters . contains ( "Running" )                            &&
             Parameters [ "Running" ] . toBool ( ) )                         {
          bool retry   = false                                               ;
          bool Forever = false                                               ;
          ////////////////////////////////////////////////////////////////////
          if ( Parameters . contains ( "Forever" ) )                         {
            Forever = Parameters [ "Forever" ] . toBool ( )                  ;
          }                                                                  ;
          if ( Forever )                                                     {
            retry = true                                                     ;
          } else                                                             {
            if ( Parameters.contains("Retry") )                              {
              if ( Parameters["Trying"].toInt()<Parameters["Retry"].toInt()) {
                retry = true                                                 ;
              }                                                              ;
            }                                                                ;
          }                                                                  ;
          ////////////////////////////////////////////////////////////////////
          if ( retry )                                                       {
            state = 101                                                      ;
            setParameter ( "Trying" , Parameters["Trying"].toInt() + 1 )     ;
            Time :: msleep ( 500 )                                           ;
          } else                                                             {
            duty  = false                                                    ;
          }                                                                  ;
        } else                                                               {
          duty = false                                                       ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      default                                                                :
      break                                                                  ;
    }                                                                        ;
    SyncTime     (                 )                                         ;
    Intermediate ( sleeping , duty )                                         ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( listening ) Close ( )                                                 ;
  Finished     ( 3                     )                                     ;
  setParameter ( "Stopped"      , true )                                     ;
  setParameter ( "ReadStopped"  , true )                                     ;
  setParameter ( "WriteStopped" , true )                                     ;
  AutoDeletion (                       )                                     ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

bool N::NetConnector::ReadChannel  (void)
{
  bool      listening = false                                                ;
  bool      duty      = false                                                ;
  bool      sleeping  = true                                                 ;
  int       state     = 0                                                    ;
  ////////////////////////////////////////////////////////////////////////////
  duty = Parameters [ "Running"   ] . toBool ( )                             ;
  setParameter ( "Stopped"      , false )                                    ;
  setParameter ( "ReadStopped"  , false )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Parameters.contains("State"   ) )                                     {
    state = Parameters [ "State" ] .toInt ( )                                ;
  } else setParameter ( "State" , state ) ;                                  ;
  ////////////////////////////////////////////////////////////////////////////
  AccessTime ( )                                                             ;
  while ( duty )                                                             {
    state = Parameters [ "State" ] .toInt ( )                                ;
    switch ( state )                                                         {
      case   0                                                               :
        state    = 101                                                       ;
        sleeping = false                                                     ;
        setParameter ( "State" , state )                                     ;
        Prepend      (                 )                                     ;
        Prepared     ( 1               )                                     ;
        AccessTime   (                 )                                     ;
      break                                                                  ;
      case 101                                                               :
        state    = 102                                                       ;
        sleeping = true                                                      ;
        AssignHost   (                 )                                     ;
        AccessTime   (                 )                                     ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 102                                                               :
        sleeping = true                                                      ;
        if ( Connect ( ) )                                                   {
          state = 103                                                        ;
          AccessTime ( )                                                     ;
        } else                                                               {
          state = 901                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 103                                                               :
        sleeping = true                                                      ;
        if ( isConnected ( 100 ) )                                           {
          state     = 401                                                    ;
          listening = true                                                   ;
          Started    ( 1 )                                                   ;
          AccessTime (   )                                                   ;
        } else                                                               {
          state = 901                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 201                                                               :
        sleeping = true                                                      ;
        if ( ReadChunk ( ) )                                                 {
          AccessTime ( )                                                     ;
        } else                                                               {
        }                                                                    ;
        state = 401                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 202                                                               :
        sleeping = true                                                      ;
        if ( ReadLine ( ) )                                                  {
          AccessTime ( )                                                     ;
        } else                                                               {
        }                                                                    ;
        state = 401                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 203                                                               :
        sleeping = true                                                      ;
        if ( ReadStructure ( ) )                                             {
          AccessTime ( )                                                     ;
        } else                                                               {
        }                                                                    ;
        state = 401                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 401                                                               :
        sleeping = true                                                      ;
        if ( Interpreter -> contains ( "State" ) )                           {
          Interpreter -> Actions ( )                                         ;
          state = Interpreter -> value ( "State" ) . toInt ( )               ;
          switch ( state )                                                   {
            case 0                                                           :
              state = 901                                                    ;
            break                                                            ;
            case 1                                                           :
              state = 201                                                    ;
            break                                                            ;
            case 2                                                           :
              state = 202                                                    ;
            break                                                            ;
            case 3                                                           :
              state = 203                                                    ;
            break                                                            ;
          }                                                                  ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 901                                                               :
        sleeping  = true                                                     ;
        listening = false                                                    ;
        Close ( )                                                            ;
        state = 999                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 911                                                               :
        Time :: msleep ( 10 )                                                ;
      break                                                                  ;
      case 999                                                               :
        if ( Parameters . contains ( "Running" )                            &&
             Parameters [ "Running" ] . toBool ( ) )                         {
          bool retry   = false                                               ;
          bool Forever = false                                               ;
          ////////////////////////////////////////////////////////////////////
          if ( Parameters . contains ( "Forever" ) )                         {
            Forever = Parameters [ "Forever" ] . toBool ( )                  ;
          }                                                                  ;
          if ( Forever )                                                     {
            retry = true                                                     ;
          } else                                                             {
            if ( Parameters.contains("Retry") )                              {
              if ( Parameters["Trying"].toInt()<Parameters["Retry"].toInt()) {
                retry = true                                                 ;
              }                                                              ;
            }                                                                ;
          }                                                                  ;
          ////////////////////////////////////////////////////////////////////
          if ( retry )                                                       {
            state = 101                                                      ;
            setParameter ( "Trying" , Parameters["Trying"].toInt() + 1 )     ;
            Time :: msleep ( 500 )                                           ;
          } else                                                             {
            duty  = false                                                    ;
          }                                                                  ;
        } else                                                               {
          duty = false                                                       ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      default                                                                :
      break                                                                  ;
    }                                                                        ;
    SyncTime     (                 )                                         ;
    Intermediate ( sleeping , duty )                                         ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( listening ) Close ( )                                                 ;
  Finished     ( 1                     )                                     ;
  setParameter ( "ReadStopped"  , true )                                     ;
  if ( Parameters["WriteStopped"].toBool() ) setParameter("Stopped",true)    ;
  AutoDeletion (                       )                                     ;
  return true                                                                ;
}

bool N::NetConnector::WriteChannel (void)
{
  bool      listening = false                                                ;
  bool      duty      = false                                                ;
  bool      sleeping  = true                                                 ;
  int       state     = 0                                                    ;
  ////////////////////////////////////////////////////////////////////////////
  duty = Parameters [ "Running"   ] . toBool ( )                             ;
  setParameter ( "Stopped"      , false )                                    ;
  setParameter ( "WriteStopped" , false )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Parameters.contains("State"   ) )                                     {
    state = Parameters [ "State" ] .toInt ( )                                ;
  } else setParameter ( "State" , state ) ;                                  ;
  ////////////////////////////////////////////////////////////////////////////
  AccessTime ( )                                                             ;
  while ( duty )                                                             {
    state = Parameters [ "State" ] .toInt ( )                                ;
    switch ( state )                                                         {
      case   0                                                               :
        state    = 101                                                       ;
        sleeping = false                                                     ;
        setParameter ( "State" , state )                                     ;
        Prepend      (                 )                                     ;
        Prepared     ( 2               )                                     ;
        AccessTime   (                 )                                     ;
      break                                                                  ;
      case 101                                                               :
        state    = 102                                                       ;
        sleeping = true                                                      ;
        AssignHost   (                 )                                     ;
        AccessTime   (                 )                                     ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 102                                                               :
        sleeping = true                                                      ;
        if ( Connect ( ) )                                                   {
          state = 103                                                        ;
          AccessTime ( )                                                     ;
        } else                                                               {
          state = 901                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 103                                                               :
        sleeping = true                                                      ;
        if ( isConnected ( 100 ) )                                           {
          state     = 401                                                    ;
          listening = true                                                   ;
          Started    ( 2 )                                                   ;
          AccessTime (   )                                                   ;
        } else                                                               {
          state = 901                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 301                                                               :
        sleeping = true                                                      ;
        if ( WriteCommand ( ) )                                              {
          state = 302                                                        ;
        } else                                                               {
          state = 303                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 302                                                               :
        sleeping = true                                                      ;
        Interpreter -> Actions ( )                                           ;
        AccessTime ( )                                                       ;
        state    = 304                                                       ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 303                                                               :
        sleeping = true                                                      ;
        Interpreter -> Actions ( )                                           ;
        state    = 304                                                       ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 304                                                               :
        sleeping = true                                                      ;
        state    = 401                                                       ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 401                                                               :
        sleeping = true                                                      ;
        if ( Interpreter->Data(NetProtocol::Output).size() > 0 )             {
          state = 301                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 901                                                               :
        sleeping  = true                                                     ;
        listening = false                                                    ;
        Close ( )                                                            ;
        state = 999                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 911                                                               :
        Time :: msleep ( 10 )                                                ;
      break                                                                  ;
      case 999                                                               :
        if ( Parameters . contains ( "Running" )                            &&
             Parameters [ "Running" ] . toBool ( ) )                         {
          bool retry   = false                                               ;
          bool Forever = false                                               ;
          ////////////////////////////////////////////////////////////////////
          if ( Parameters . contains ( "Forever" ) )                         {
            Forever = Parameters [ "Forever" ] . toBool ( )                  ;
          }                                                                  ;
          if ( Forever )                                                     {
            retry = true                                                     ;
          } else                                                             {
            if ( Parameters.contains("Retry") )                              {
              if ( Parameters["Trying"].toInt()<Parameters["Retry"].toInt()) {
                retry = true                                                 ;
              }                                                              ;
            }                                                                ;
          }                                                                  ;
          ////////////////////////////////////////////////////////////////////
          if ( retry )                                                       {
            state = 101                                                      ;
            setParameter ( "Trying" , Parameters["Trying"].toInt() + 1 )     ;
            Time :: msleep ( 500 )                                           ;
          } else                                                             {
            duty  = false                                                    ;
          }                                                                  ;
        } else                                                               {
          duty = false                                                       ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      default                                                                :
      break                                                                  ;
    }                                                                        ;
    SyncTime     (                 )                                         ;
    Intermediate ( sleeping , duty )                                         ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( listening ) Close ( )                                                 ;
  Finished     ( 2                     )                                     ;
  setParameter ( "WriteStopped" , true )                                     ;
  if ( Parameters["ReadStopped"].toBool() ) setParameter("Stopped",true)     ;
  AutoDeletion (                       )                                     ;
  return true                                                                ;
}

bool N::NetConnector::AtChannel(int channels)
{
  switch ( channels )               {
    case 1: return ReadChannel  ( ) ;
    case 2: return WriteChannel ( ) ;
    case 3: return BothChannels ( ) ;
  }                                 ;
  return false                      ;
}
