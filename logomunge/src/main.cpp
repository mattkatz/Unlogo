#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "TrainingSet.h"


using namespace std;
using namespace cv;
namespace fs = boost::filesystem;
namespace po = boost::program_options;


// ARGS (set in parse_args)
bool recursive=false;
int max_levels;
int verbose;
vector<string> directories;


// OTHER ARGS
SurfFeatureDetector* detector;
SurfDescriptorExtractor* extractor;


// METHODS
void parse_args(int argc, char * const argv[]);
void scan_directory(fs::path dir, int level);


// ----------------------------------------------------
int main(int argc, char * const argv[])
{
	double hessianThreshold = 400;
	int octaves = 3;
	int octaveLayers = 4;
	detector = new SurfFeatureDetector(hessianThreshold,octaves,octaveLayers);
	extractor = new SurfDescriptorExtractor();
	
	parse_args(argc, argv);
	for(int i=0; i<directories.size(); i++)
	{
		scan_directory(fs::path(directories[i]), 0);
	}
}



// ----------------------------------------------------
void scan_directory(fs::path dir, int level) 
{
	if(level > max_levels) return;
	
	// We will make a TrainingSet out of the images in this directory.
	TrainingSet set;
	set.setFeatureDetector( detector );
	set.setDescriptorExtractor( extractor );
	
	set.loadFromDirectory(dir.string());
	set.save();
	
	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_itr(dir); dir_itr != end_iter;  ++dir_itr )
	{
		if(fs::is_directory(dir_itr->path()))
		{
			if(recursive) scan_directory(dir_itr->path(), level+1);
		}
	}
}

// ----------------------------------------------------
void parse_args(int argc, char* const argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
	("help,h", "produce help message")
	("version,v", "print version string")
	("recursive,r", "recurse into directories")
	("verbose", po::value<int>()->default_value(1), "enable verbosity (optionally specify level)")
	("maxlevels,m", po::value<int>()->default_value(10), "The maximum number of levels to recurse (only used if -r is set)")
	("directory,d", po::value< vector<string> >(), "a path to the directory that contains logos")
	;
	
	po::variables_map vm;
	
	try {
		po::store(parse_command_line(argc, argv, desc), vm);
	} catch(boost::program_options::invalid_option_value e){
		cerr << "invalid_option_value exception thrown parsing config file:" << e.what() << endl;
		exit(1);
	} catch(std::exception e){
		cerr << "Other exception thrown parsing config file:" << e.what() << endl;
		exit(1);
	}		
	
	notify(vm);
	if (vm.count("help"))
	{
		cout << "Usage: logomunge [options]\n";
		cout << desc;
		exit(0);
	}
	
	if (vm.count("version"))
	{
		cout << "logomunge, version 1.0" << endl;
		exit( 0 );
	}
	
	if (vm.count("maxlevels"))
	{
		max_levels = vm["maxlevels"].as<int>();
	}
	
	if (vm.count("verbose"))
	{
		verbose = vm["verbose"].as<int>();
		cout << "Verbosity enabled. Level is " << vm["verbose"].as<int>() << endl;
	}
	
	if (vm.count("recursive"))
	{
		recursive = true;
		cout << "Recursing through paths up to " << max_levels << " levels" << endl;
	}
	
	if (!vm.count("directory"))
	{
		cout << "No directories provided.  Exiting." << endl;
		exit(0);
	}
	
	directories = vm["directory"].as< vector<string> >();	
}