#include "library.h"
#include "demo_lib_sift.h"
#include "frot.h"
#include "fproj.h"
#include <vector>
using namespace std;


int compute_asift_keypoints(vector<float>& image, int width, int height, int num_of_tilts, int verb, vector< vector< keypointslist > >& keys_all, siftPar &siftparameters);

void GaussianBlur1D(vector<float>& image, int width, int height, float sigma, int flag_dir);
