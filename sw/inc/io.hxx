/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: io.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: ihi $ $Date: 2007-07-12 10:40:51 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
// eigene Klasse fuer IO, die die systemunabhaengige Darstellung
// uebernimmt (bytes dreht, Character konvertiert)
// das Schreiben erfolgt aus Effizienzgruenden binaer
#ifndef _IO_HXX
#define _IO_HXX

#ifdef UNX
#include <unistd.h>
#else
#include <io.h>
#endif

#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>


#ifndef _KEYCOD_HXX //autogen
#include <vcl/keycod.hxx>
#endif

#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif

/*$
class BinaryFile {
    int fd;
public:
    enum IO_OpenMode {
        BF_READ = O_RDONLY,
        BF_WRITE = O_RDWR,
        BF_CREATE = O_CREAT,
        BF_TRUNC = O_TRUNC
    };
        // ctor oeffnet File im BinearMode, dtor schliesst es
    BinaryFile(const String &, int eOpenMode);
    ~BinaryFile();
    BOOL Ok() const {
        return -1 != fd;
    }
    operator int() const { return fd; }
};
*/

class SwIOin {
private:
        SvFileStream aStr; //$ ifstream
public:
    // Stream wird im entsprechenden Mode erzeugt.
    SwIOin(const String &rFilename, StreamMode nMode =
                                    STREAM_READ | STREAM_NOCREATE );

    SwIOin& operator>>(char& val);
    SwIOin& operator>>(unsigned char& val);
    SwIOin& operator>>(char* val);
    SwIOin& operator>>(unsigned char* val);
    SwIOin& operator>>(short& val);
    SwIOin& operator>>(unsigned short& val);
    SwIOin& operator>>(long& val);
    SwIOin& operator>>(unsigned long& val);
    String ReadString();
    KeyCode ReadKeyCode();
    // kann erweitert werden fuer weitere Arrays von
    // Basistypen; nLen ist die Anzahl der Elemente
    SwIOin& Read(char *buf, unsigned nLen);

    int operator!() { return aStr.GetError() != SVSTREAM_OK; }
        SvFileStream &operator()() {
        return aStr;
    }
};

class SwIOout {
private:
    void _write(const char *buf, unsigned size);
    SvFileStream aStr; //$ ofstream
public:
    // Stream wird im entsprechenden Mode erzeugt.
    SwIOout( const String &rFilename, StreamMode nMode =
                                      STREAM_WRITE | STREAM_NOCREATE );
    SwIOout& operator<<(char val);
    SwIOout& operator<<(unsigned char val);
    SwIOout& operator<<(char* val);
    SwIOout& operator<<(unsigned char* val);
    SwIOout& operator<<(short val);
    SwIOout& operator<<(unsigned short val);
    SwIOout& operator<<(long val);
    SwIOout& operator<<(unsigned long val);
    SwIOout& operator<<(const String &);
    SwIOout& operator<<(const KeyCode &);
    // kann erweitert werden fuer weitere Arrays von
    // Basistypen; nLen ist die Anzahl der Elemente
    SwIOout& Write(const char *buf, unsigned nLen);

    int operator!() { return aStr.GetError() != SVSTREAM_OK; }
    SvFileStream &operator()() {
        return aStr;
    }
};


class SwIOinout {
private:
    SvFileStream aStr; //$ fstream

public:
    // Stream wird im entsprechenden Mode erzeugt.
    SwIOinout(const String &rFilename, StreamMode nMode =
                                       STREAM_READWRITE  | STREAM_NOCREATE );

    SwIOinout& operator>>(char& val);
    SwIOinout& operator>>(unsigned char& val);
    SwIOinout& operator>>(char* val);
    SwIOinout& operator>>(unsigned char* val);
    SwIOinout& operator>>(short& val);
    SwIOinout& operator>>(unsigned short& val);
    SwIOinout& operator>>(long& val);
    SwIOinout& operator>>(unsigned long& val);
    String ReadString();
    KeyCode ReadKeyCode();
    // kann erweitert werden fuer weitere Arrays von
    // Basistypen; nLen ist die Anzahl der Elemente
    SwIOinout& Read(char *buf, unsigned nLen);
    SwIOinout& Read(unsigned short *buf, unsigned nLen );

    SwIOinout& operator<<(char val);
    SwIOinout& operator<<(unsigned char val);
    SwIOinout& operator<<(char* val);
    SwIOinout& operator<<(unsigned char* val);
    SwIOinout& operator<<(short val);
    SwIOinout& operator<<(unsigned short val);
    SwIOinout& operator<<(long val);
    SwIOinout& operator<<(unsigned long val);
    SwIOinout& operator<<(const String &);
    SwIOinout& operator<<(const KeyCode &);
    // kann erweitert werden fuer weitere Arrays von
    // Basistypen; nLen ist die Anzahl der Elemente
    SwIOinout& Write(const char *buf, unsigned nLen);

    int operator!() { return aStr.GetError() != SVSTREAM_OK; }
        SvFileStream &operator()() {
        return aStr;
    }

    BOOL Ok();
};



#endif

