#include "LogoDetect.h"

#include "opencv/highgui.h"
#include "opencv/cv.h"

LogoDetect::LogoDetect( const std::string& targetImage,
                        float ratio /*= 0.5f*/ )
  : m_kdTree(0)
  , m_ratio(ratio)
{
  // load target image
  
  IplImage* target = cvLoadImage( targetImage.c_str() );

  if ( ! target )
    throw Exception();
  
  // convert to YCrCb colour space
  
  IplImage* img =
    cvCreateImage( cvSize( target->width, target->height ), 8, 3 );

  cvCvtColor( target, img, CV_RGB2YCrCb );

  cvReleaseImage( &target );
  
  // use libsiftfast to extract features
  
  Image siftImg = CreateImage( img->height, img->width );
    
  for ( int y(0); y<img->height; ++y )
    for ( int x(0); x<img->width; ++x )
      siftImg->pixels[y*siftImg->stride+x] =
        float(img->imageData[(y*img->width+x)*3+0]); // Y - channel

  cvReleaseImage( &img );
    
  Keypoint keypts = GetKeypoints( siftImg );

  int nFeatures(0);
  for ( Keypoint tmp( keypts ); tmp; tmp = tmp->next )
    ++nFeatures;
    
#ifndef NDEBUG    
  fprintf( stderr, "nFeatures in target = %d\n", nFeatures );
#endif
  
  // build a kd tree from features
  
  ANNpointArray pts = annAllocPts( nFeatures, 128 );

  int i(0);
  for ( Keypoint tmp( keypts ); tmp; tmp = tmp->next )
    ::memcpy( pts[i++], tmp->descrip, sizeof(float)*128 );

  m_kdTree = new ANNkd_tree( pts, nFeatures, 128, 16 );

  FreeKeypoints( keypts );

  DestroyAllImages();
}

LogoDetect::~LogoDetect()
{
  delete m_kdTree;
}

void LogoDetect::detect( const uint8_t* frame, int stride,
                         int width, int height )
{
  // use libsiftfast to extract features from video frame (Y-channel)
  
  Image img = CreateImage( height, width );

  for ( int y(0); y<height; ++y )
    for ( int x(0); x<width; ++x )
      img->pixels[y*img->stride+x] = float(frame[y*stride+x]);

  Keypoint keypts = GetKeypoints( img );

  // match extracted features with target
  
  matchWithTarget( keypts );

  FreeKeypoints( keypts );

  DestroyAllImages();
}

void LogoDetect::matchWithTarget( Keypoint keypts )
{
  m_matches.clear();

  int nFeatures(0);
  for ( Keypoint tmp( keypts ); tmp; tmp = tmp->next )
  {
    ANNidx nnIdx[2];
    ANNdist dist[2];
        
    m_kdTree->annkPriSearch( tmp->descrip, 2, nnIdx, dist, 0.0 );

    if ( dist[0] < m_ratio * dist[1] )
      m_matches.push_back( Match( tmp->row, tmp->col ) );

    ++nFeatures;
  }

#ifndef NDEBUG
  fprintf( stderr, "nFeatures in source  = %d\n", nFeatures );
  fprintf( stderr, "nMatches with target = %d\n", m_matches.size() );
#endif

  // TODO: remove false matches
}
