/*
 *  Compress.h
 *  logomunge
 *
 *  Created by Jeffrey Crouse on 4/6/11.
 *  Using minigzip file found in the zlib distribution
 *
 */

#include "zlib.h"
#include <stdio.h>

#ifdef STDC
#  include <string.h>
#  include <stdlib.h>
#endif

#ifdef USE_MMAP
#  include <sys/types.h>
#  include <sys/mman.h>
#  include <sys/stat.h>
#endif

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  ifdef UNDER_CE
#    include <stdlib.h>
#  endif
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#ifdef VMS
#  define unlink delete
#  define GZ_SUFFIX "-gz"
#endif
#ifdef RISCOS
#  define unlink remove
#  define GZ_SUFFIX "-gz"
#  define fileno(file) file->__file
#endif
#if defined(__MWERKS__) && __dest_os != __be_os && __dest_os != __win32_os
#  include <unix.h> /* for fileno */
#endif

#if !defined(Z_HAVE_UNISTD_H) && !defined(_LARGEFILE64_SOURCE)
#ifndef WIN32 /* unlink already in stdio.h for WIN32 */
extern int unlink OF((const char *));
#endif
#endif

#if defined(UNDER_CE)
#  include <windows.h>
#  define perror(s) pwinerror(s)

/* Map the Windows error number in ERROR to a locale-dependent error
 message string and return a pointer to it.  Typically, the values
 for ERROR come from GetLastError.
 
 The string pointed to shall not be modified by the application,
 but may be overwritten by a subsequent call to strwinerror
 
 The strwinerror function does not change the current setting
 of GetLastError.  */

static char *strwinerror (DWORD error)
{
    static char buf[1024];
	
    wchar_t *msgbuf;
    DWORD lasterr = GetLastError();
    DWORD chars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
								| FORMAT_MESSAGE_ALLOCATE_BUFFER,
								NULL,
								error,
								0, /* Default language */
								(LPVOID)&msgbuf,
								0,
								NULL);
    if (chars != 0) {
        /* If there is an \r\n appended, zap it.  */
        if (chars >= 2
            && msgbuf[chars - 2] == '\r' && msgbuf[chars - 1] == '\n') {
            chars -= 2;
            msgbuf[chars] = 0;
        }
		
        if (chars > sizeof (buf) - 1) {
            chars = sizeof (buf) - 1;
            msgbuf[chars] = 0;
        }
		
        wcstombs(buf, msgbuf, chars + 1);
        LocalFree(msgbuf);
    }
    else {
        sprintf(buf, "unknown win32 error (%ld)", error);
    }
	
    SetLastError(lasterr);
    return buf;
}

static void pwinerror(const char *s)
{
    if (s && *s)
        fprintf(stderr, "%s: %s\n", s, strwinerror(GetLastError ()));
	else
		fprintf(stderr, "%s\n", strwinerror(GetLastError ()));
}

#endif /* UNDER_CE */

#ifndef GZ_SUFFIX
#  define GZ_SUFFIX ".gz"
#endif
#define SUFFIX_LEN (sizeof(GZ_SUFFIX)-1)

#define BUFLEN      16384
#define MAX_NAME_LEN 1024

#ifdef MAXSEG_64K
#  define local static
/* Needed for systems with limitation on stack size. */
#else
#  define local
#endif



class Compress {
public:
	
	static void compress(const char* file, int level=Z_DEFAULT_COMPRESSION, int mode=Z_FILTERED) {
		char outmode[20];
		strcpy(outmode, "wb6 ");
		outmode[2]=level;
		if(mode==Z_FILTERED)		outmode[3] = 'f';
		if(mode==Z_HUFFMAN_ONLY)	outmode[3] = 'h';
		if(mode==Z_RLE)				outmode[3] = 'R';
		file_compress((char*)file, outmode);
	}
	
	static void uncompress(const char* file) {
		 file_uncompress((char*)file);
	}


protected:	

	static void error            OF((const char *msg));
	static void gz_compress      OF((FILE   *in, gzFile out));
#ifdef USE_MMAP
	static int  gz_compress_mmap OF((FILE   *in, gzFile out));
#endif
	static void gz_uncompress    OF((gzFile in, FILE   *out));
	static void file_compress    OF((char  *file, char *mode));
	static void file_uncompress  OF((char  *file));
	
};
