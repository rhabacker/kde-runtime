#ifndef __file_h__
#define __file_h__

#include <kio_interface.h>
#include <kio_base.h>

#include <string>
#include <list>

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

class FileProtocol : public IOProtocol
{
public:
  FileProtocol( Connection *_conn );
  
  virtual void slotGet( const char *_url );
  virtual void slotGetSize( const char *_url );

  virtual void slotPut( const char *_url, int _mode,
			bool _overwrite, bool _resume, int _size );

  virtual void slotMkdir( const char *_url, int _mode );

  virtual void slotCopy( const char*, const char *_dest );
  virtual void slotCopy( list<string>&, const char *_dest );

  virtual void slotMove( const char *_source, const char *_dest );
  virtual void slotMove( list<string>& _source, const char *_dest );

  virtual void slotDel( const char *_url );
  virtual void slotDel( list<string>& _source );

  virtual void slotListDir( const char *_url );
  virtual void slotTestDir( const char *_url );

  virtual void slotUnmount( const char *_point );
  virtual void slotMount( bool _ro, const char *_fstype, const char* _dev, const char *_point );
  
  virtual void slotData( void *_p, int _len );
  virtual void slotDataEnd();
  
  Connection* connection() { return ConnectionSignals::m_pConnection; }

  void jobError( int _errid, const char *_txt );
  
protected:
  struct Copy
  {
    string m_strAbsSource;
    string m_strRelDest;
    mode_t m_mode;
    off_t m_size;
  };
  
  struct CopyDir
  {
    string m_strAbsSource;
    string m_strRelDest;
    mode_t m_mode;
    ino_t m_ino;
  };
  
  void doCopy( list<string>&, const char *_dest, bool _rename, bool _move = false );

  long listRecursive( const char *_path, list<Copy>& _files, list<CopyDir>& _dirs, bool _rename );
  long listRecursive2( const char *_abs_path, const char *_rel_path, list<Copy>& _files, list<CopyDir>& _dirs );

  int m_cmd;
  bool m_bIgnoreJobErrors;
  
  FILE* m_fPut;
};

class FileIOJob : public IOJob
{
public:
  FileIOJob( Connection *_conn, FileProtocol *_File );
  
  virtual void slotError( int _errid, const char *_txt );

protected:
  FileProtocol* m_pFile;
};

#endif
