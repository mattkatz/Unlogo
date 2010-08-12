#ifndef __LOGO_DETECT_H
#define __LOGO_DETECT_H

#include <cstdio>
#include "siftfast.h"
#include "ANN/ANN.h"

#include <string>
#include <list>

class LogoDetect
{
 public:
  struct Match
  {
    float m_row,m_col;
    Match( float row, float col )
      : m_row(row), m_col(col) {}
  };

  struct Exception {};
    
 private:
  void matchWithTarget( Keypoint keypts );
    
 public:
  LogoDetect( const std::string& targetImage,
              float ratio = 0.5f );
  ~LogoDetect();

  void detect( const uint8_t* frame, int stride,
               int width, int height );

  const std::list<Match>& getMatches() const { return m_matches; }
    
 private:
  ANNkd_tree*      m_kdTree;
  std::list<Match> m_matches;
  float            m_ratio;
};

#endif // __LOGO_DETECT_H
