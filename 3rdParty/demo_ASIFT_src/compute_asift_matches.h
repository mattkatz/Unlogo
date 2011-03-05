#include "library.h"
#include "demo_lib_sift.h"
#include "frot.h"
#include "fproj.h"
#include <vector>
using namespace std;


int compute_asift_matches(int num_of_tilts1, int num_of_tilts2, int w1, int h1, int w2, int h2, int verb, vector< vector< keypointslist > >& keys1, vector< vector< keypointslist > >& keys2, matchingslist &matchings, siftPar &siftparameters);

