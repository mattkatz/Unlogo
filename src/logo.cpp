#include <stdint.h>
#include <string.h>

#include "LogoDetect.h"

LogoDetect* g_detect = 0;

extern "C" int init( const char* args )
{
  try {
    g_detect = new LogoDetect( args, 0.5f );
  }
  catch ( ... ) {
    return -1;
  }
    
  return 0;
}

extern "C" int uninit()
{
  delete g_detect;
  g_detect = 0;
    
  return 0;
}

extern "C" int process( uint8_t* dst[4], int dst_stride[4],
                        uint8_t* src[4], int src_stride[4],
                        int width, int height )
{
  // copy whole image from source to destination
  
  for ( int i(0); i<3; ++i )
  {
    uint8_t* pdst = dst[i];
    uint8_t* psrc = src[i];
    int w = width;
    int h = height;
    if ( i > 0 )
      w >>= 1, h >>= 1;
        
    if ( dst_stride[i] == src_stride[i] )
      memcpy( pdst, psrc, src_stride[i] * h );
    else {
      for ( int y(0); y<h; ++y )
      {
        memcpy( pdst, psrc, w );
        pdst += dst_stride[i], psrc += src_stride[i];
      }
    }
  }        
  
  // run the detection on the Y channel
  
  g_detect->detect( src[0], src_stride[0], width, height );
  
  // draw matches onto destination image
  
  const std::list<LogoDetect::Match>& matches =
    g_detect->getMatches();

  std::list<LogoDetect::Match>::const_iterator it =
    matches.begin();

  while ( it != matches.end() )
  {
    dst[0][int(it->m_row+0.5)*dst_stride[0]+int(it->m_col+0.5)] = 255;
    ++it;
  }
    
  return 0;
}
