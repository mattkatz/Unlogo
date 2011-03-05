/*
 *  ASIFTMatcher.cpp
 *  unlogo
 *
 *  Created by Jeffrey Crouse on 3/4/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "ASIFTMatcher.h"


//--------------------------------------------------------------
bool ASIFTMatcher::init(float InitSigma, int BorderDist, int Scales, float PeakThresh, 
					  float EdgeThresh, float EdgeThresh1, int OriBins, float OriSigma,
					  float OriHistThresh, float MaxIndexVal, int MagFactor, float IndexSigma,
					  int IgnoreGradSign, float MatchRatio, float MatchXradius, float MatchYradius) 
{	
	/* InitSigma gives the amount of smoothing applied to the image at the
	 first level of each octave.  In effect, this determines the sampling
	 needed in the image domain relative to amount of smoothing.  Good
	 values determined experimentally are in the range 1.2 to 1.8.
	 */
	params.InitSigma = InitSigma;
	
	
	/* Peaks in the DOG function must be at least BorderDist samples away
	 from the image border, at whatever sampling is used for that scale.
	 Keypoints close to the border (BorderDist < about 15) will have part
	 of the descriptor landing outside the image, which is approximated by
	 having the closest image pixel replicated.  However, to perform as much
	 matching as possible close to the edge, use BorderDist of 4.
	 */
	params.BorderDist = BorderDist;
	
	
	/* Scales gives the number of discrete smoothing levels within each octave.
	 For example, Scales = 2 implies dividing octave into 2 intervals, so
	 smoothing for each scale sample is sqrt(2) more than previous level.
	 Value of 2 works well, but higher values find somewhat more keypoints.
	 */
	params.Scales = Scales;
	
	
	/// Decreasing PeakThresh allows more non contrasted keypoints
	/* Magnitude of difference-of-Gaussian value at a keypoint must be above
	 this threshold.  This avoids considering points with very low contrast
	 that are dominated by noise.  It is divided by Scales because more
	 closely spaced scale samples produce smaller DOG values.  A value of
	 0.08 considers only the most stable keypoints, but applications may
	 wish to use lower values such as 0.02 to find keypoints from low-contast
	 regions.
	 */
	params.PeakThresh = PeakThresh;
	
	/// Decreasing EdgeThresh allows more edge points
	/* This threshold eliminates responses at edges.  A value of 0.08 means
	 that the ratio of the largest to smallest eigenvalues (principle
	 curvatures) is below 10.  A value of 0.14 means ratio is less than 5.
	 A value of 0.0 does not eliminate any responses.
	 Threshold at first octave is different.
	 */
	params.EdgeThresh = EdgeThresh; 
	params.EdgeThresh1 = EdgeThresh1; 
	
	
	/* OriBins gives the number of bins in the histogram (36 gives 10
	 degree spacing of bins).
	 */
	params.OriBins = OriBins;
	
	
	/* Size of Gaussian used to select orientations as multiple of scale
	 of smaller Gaussian in DOG function used to find keypoint.
	 Best values: 1.0 for UseHistogramOri = FALSE; 1.5 for TRUE.
	 */
	params.OriSigma = OriSigma;
	
	
	/// Look for local (3-neighborhood) maximum with valuer larger or equal than OriHistThresh * maxval
	///  Setting one returns a single peak
	/* All local peaks in the orientation histogram are used to generate
	 keypoints, as long as the local peak is within OriHistThresh of
	 the maximum peak.  A value of 1.0 only selects a single orientation
	 at each location.
	 */
	params.OriHistThresh = OriHistThresh;
	
	
	/// Feature vector is normalized to has euclidean norm 1.
	/// This threshold avoid the excessive concentration of information on single peaks
	/* Index values are thresholded at this value so that regions with
	 high gradients do not need to match precisely in magnitude.
	 Best value should be determined experimentally.  Value of 1.0
	 has no effect.  Value of 0.2 is significantly better.
	 */
	params.MaxIndexVal = MaxIndexVal;
	
	
	/* This constant specifies how large a region is covered by each index
	 vector bin.  It gives the spacing of index samples in terms of
	 pixels at this scale (which is then multiplied by the scale of a
	 keypoint).  It should be set experimentally to as small a value as
	 possible to keep features local (good values are in range 3 to 5).
	 */
	params.MagFactor = MagFactor;
	
	
	/* Width of Gaussian weighting window for index vector values.  It is
	 given relative to half-width of index, so value of 1.0 means that
	 weight has fallen to about half near corners of index patch.  A
	 value of 1.0 works slightly better than large values (which are
	 equivalent to not using weighting).  Value of 0.5 is considerably
	 worse.
	 */
	params.IndexSigma = IndexSigma;
	
	
	/* If this is TRUE, then treat gradients with opposite signs as being
	 the same.  In theory, this could create more illumination invariance,
	 but generally harms performance in practice.
	 */
	params.IgnoreGradSign = IgnoreGradSign; 
	
	
	// Guoshen Yu. Since l1 distance is used for matching instead of l2, a larger threshold is needed.
	params.MatchRatio = MatchRatio;
	
	
	/*
	 In order to constrain the research zone for matches.
	 Useful for example when looking only at epipolar lines
	 */
	params.MatchXradius = MatchXradius;
	params.MatchYradius = MatchYradius;
	
	
	// I don't know what these do so I am ignoring them for now. (JC)
	params.OctaveMax=100000;
	params.DoubleImSize = 0;
	params.order = 3;
	params.noncorrectlylocalized = 0;
	
	
	bInited=true;
	return true;
}


//--------------------------------------------------------------
bool ASIFTMatcher::setTrainImage(Image& _train, string datapath) 
{
	assert(_train.channels()==1);
	
	train = _train;
	
	string data_filename = datapath+".json";
	struct stat stFileInfo; 
	if(stat(data_filename.c_str(), &stFileInfo) == 0)
	{ 
		return loadTrainingData(data_filename);
	}
	else
	{ 
		cout << "File " << data_filename <<" does not exist.  Generating keypoints." << endl;
		
		vector<float> fpix;
		for(int i=0; i<train.cvImg.total(); i++) {
			fpix.push_back( train.cvImg.data[i] );
		}
		
		numKeypoints = compute_asift_keypoints(fpix, train.width(), train.height(), tilts, true, train_keys, params);
		bKeysGenerated=true;
		
		return saveTrainingData(data_filename);
	} 
}

//--------------------------------------------------------------
void ASIFTMatcher::drawTrainKeypointsIntoImage(Image& img)
{	
	float x, y, x2, y2, scale, angle;
	for(int i=0; i<train_keys.size(); i++)
	{
		for(int j=0; j<train_keys[i].size(); j++) 
		{
			for(int k=0; k<train_keys[i][j].size(); k++)
			{
				x = train_keys[i][j][k].x;
				y = train_keys[i][j][k].y;
				scale = train_keys[i][j][k].scale/2.;
				angle = train_keys[i][j][k].angle;
				x2 = (cos(angle)*scale)+x;
				y2 = (sin(angle)*scale)+y;
				
				circle(img.cvImg, Point(x,y), scale, Scalar(0,150,150), 1);
				line(img.cvImg, Point(x,y), Point(x2,y2), Scalar(210,100,0), 1);
			}
		}
	}
}


//--------------------------------------------------------------
void ASIFTMatcher::doQuery(Image& query, bool showCorrespondence)
{
	if(!bInited)	{
		cerr << "WARNING: Not inited. Using default values. Please call init before doQuery" << endl;
		init();
	}
	
	if(!bKeysGenerated) {
		cerr << "WARNING:Keys not generated for train image." << endl;
		return;
	}
	
	assert(query.channels()==1);
	
	vector<float> fpix;
	for(int i=0; i<query.cvImg.total(); i++) {
		fpix.push_back( query.cvImg.data[i] );
	}
	
	vector< vector< keypointslist > > query_keys;
	compute_asift_keypoints(fpix, query.width(), query.height(), tilts, true, query_keys, params);
	
	
	num_matchings = compute_asift_matches(tilts, tilts, 
											  train.width(), train.height(), 
											  query.width(), query.height(), 
											  true, train_keys, query_keys, matchings, params);
	if(num_matchings>0)
	{
		// Convert the matches into CvMats
		CvMat _pt1, _pt2;
		vector<CvPoint2D32f> pt1, pt2;
		pt1.resize(num_matchings);
		pt2.resize(num_matchings);
		for(int i = 0; i < num_matchings; i++ ) {
			pt1[i] = cvPoint2D32f(matchings[i].first.x, matchings[i].first.y);
			pt2[i] = cvPoint2D32f(matchings[i].second.x, matchings[i].second.y);
		}
		_pt1 = cvMat(1, num_matchings, CV_32FC2, &pt1[0] );
		_pt2 = cvMat(1, num_matchings, CV_32FC2, &pt2[0] );
		
		// Find the homography using the matches points.
		cvFindHomography( &_pt1, &_pt2, &_h, CV_RANSAC, 10 );
	}
	
	
	if(showCorrespondence)
	{
		Size s;
		s.width = train.width() + query.width();
		s.height = max(train.height(), query.height());
		Image correspondence( s );
		
		train.copyInto(correspondence, 0, 0);
		query.copyInto(correspondence, train.width(), 0);
		
		matchingslist::iterator ptr = matchings.begin();
		for(int i=0; i < (int) matchings.size(); i++, ptr++)
		{	
			Point pt1(ptr->first.x, ptr->first.y);
			Point pt2(ptr->second.x+train.width(), ptr->second.y);
			line(correspondence.cvImg, pt1, pt2, Scalar(255,0,0), 1);
		}
	}
}


//--------------------------------------------------------------
bool ASIFTMatcher::saveTrainingData(string filename)
{	
	if(!bKeysGenerated) {
		cout << "You must calculate keypoints before attempting to save them!" << endl;
		return false;
	}
	
	JSONElement root;
	root["params"]["OctaveMax"] = params.OctaveMax;
	root["params"]["DoubleImSize"] = params.DoubleImSize;
	root["params"]["order"] = params.order;
	root["params"]["InitSigma"] = params.InitSigma;
	root["params"]["BorderDist"] = params.BorderDist; 
	root["params"]["Scales"] = params.Scales;
	root["params"]["PeakThresh"] = params.PeakThresh;
	root["params"]["EdgeThresh"] = params.EdgeThresh; 
	root["params"]["EdgeThresh1"] = params.EdgeThresh1; 
	root["params"]["OriBins"]  = params.OriBins;
	root["params"]["OriSigma"] = params.OriSigma;
	root["params"]["OriHistThresh"] = params.OriHistThresh;
	root["params"]["MaxIndexVal"] = params.MaxIndexVal;
	root["params"]["MagFactor"]  = params.MagFactor;
	root["params"]["IndexSigma"]  = params.IndexSigma;
	root["params"]["IgnoreGradSign"] = params.IgnoreGradSign;
	root["params"]["MatchRatio"] = params.MatchRatio;
	root["params"]["MatchXradius"] = params.MatchXradius;
	root["params"]["MatchYradius"] = params.MatchYradius;
	root["params"]["noncorrectlylocalized"] = params.noncorrectlylocalized;
	
	for (int i = 0; i<(int)train_keys.size(); i++)
	{
		Json::Value elem1;
		for (int j = 0; j < (int)train_keys[i].size(); j++)
		{
			Json::Value elem2;
			for(int k=0; k<(int)train_keys[i][j].size(); k++)	
			{
				Json::Value keypoint;
				keypoint["x"] = train_keys[i][j][k].x;
				keypoint["y"] = train_keys[i][j][k].y;
				keypoint["scale"] = train_keys[i][j][k].scale;
				keypoint["angle"] = train_keys[i][j][k].angle;
				for (int ii = 0; ii <(int)VecLength; ii++) {
					keypoint["data"].append((int)train_keys[i][j][k].vec[ii]);
				}
				elem2.append(keypoint);
			}
			elem1.append(elem2);
		}
		root["keypoints"].append(elem1);
	}
	
	cout << "Saving keypoints to " << filename << endl;
	return root.save(filename);
}


//--------------------------------------------------------------
bool ASIFTMatcher::loadTrainingData(string filename)
{
	numKeypoints=0;
	
	JSONElement root;
	if(!root.open(filename)) {
		cout << "Couldn't open " << filename << endl;;
		return false;
	}
	
	Json::Value kpts = root["keypoints"];
	for(int i=0; i<kpts.size(); i++)
	{
		vector<keypointslist> vec;
		for(int j=0; j<kpts[i].size(); j++)
		{
			keypointslist list;
			for(int k=0; k<kpts[i][j].size(); k++) 
			{
				keypoint kpt;
				kpt.x = kpts[i][j][k]["x"].asDouble();
				kpt.y = kpts[i][j][k]["y"].asDouble();
				kpt.scale = kpts[i][j][k]["scale"].asDouble();
				kpt.angle = kpts[i][j][k]["angle"].asDouble();
				//cout << "keypoint: " << kpt.x << ", " << kpt.y << endl;
				for(int ii=0; ii<VecLength; ii++)
				{
					kpt.vec[ii] = kpts[i][j][k]["data"][ii].asInt();
				}
				list.push_back( kpt );
				numKeypoints++;
			}
			vec.push_back( list );
		}
		train_keys.push_back( vec );
	}
	bKeysGenerated=true;
	return true;
}
