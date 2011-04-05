#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace std;
using namespace cv;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

bool recursive=false;
int verbose;
Mat image_w_margin;
string window_name;
vector<Point> corners;
int margin=50;


SurfFeatureDetector detector(400);
SurfDescriptorExtractor extractor;


void scan_directory(fs::path dir, int level);
bool is_image(fs::path p);
void generate_datafile(Mat image, fs::path data_path);
void on_mouse( int event, int x, int y, int flags, void* param );

// ----------------------------------------------------
int main (int argc, char * const argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("version,v", "print version string")
		("recursive,r", "recurse into directories")
		("verbose", po::value<int>()->default_value(1), "enable verbosity (optionally specify level)")
		("directory,d", po::value< vector<string> >(), "a path to the directory that contains logos")
		;

	po::variables_map vm;
	
	try {
		po::store(parse_command_line(argc, argv, desc), vm);
	} catch(boost::program_options::invalid_option_value e){
		cerr << "invalid_option_value exception thrown parsing config file:" << e.what() << endl;
		return -2;
	} catch(std::exception e){
		cerr << "Other exception thrown parsing config file:" << e.what() << endl;
		return -2;
	}		
		
	notify(vm);
	if (vm.count("help"))
	{
		cout << "Usage: logomunge [options]\n";
		cout << desc;
		return 0;
	}

	if (vm.count("version"))
	{
		cout << "logomunge, version 1.0" << endl;
		return 0;
	}
	
	if (vm.count("verbose"))
	{
		verbose = vm["verbose"].as<int>();
		cout << "Verbosity enabled. Level is " << vm["verbose"].as<int>() << endl;
	}
	
	if (vm.count("recursive"))
	{
		recursive = true;
		cout << "Recursing through paths" << endl;
	}
	
	if (!vm.count("directory"))
	{
		cout << "No directory provided.  Exiting." << endl;
		return 0;
	}
	
	
	vector<string> directories = vm["directory"].as< vector<string> >();
	for(int i=0; i<directories.size(); i++)
	{
		fs::path dir_path( directories[i] );
		scan_directory(dir_path, 0);
	}
}




// ----------------------------------------------------
void scan_directory(fs::path dir, int level)
{
	if( !fs::exists( dir ) ) 
	{
		cout << dir << " doesn't exist.  Skipping." << endl;
		return;
	}
	
	if( !fs::is_directory(dir))
	{
		cout << dir << " is not a directory.  Skipping." << endl;
		return;
	}
	
	cout << "Opening " << dir << endl;
	
	// This will hold descriptors for every image in the directory.
	vector<Mat> all_descriptors;
	
	// Loop through the contents of the directory and assemble SURF points of any images found.
	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_itr( dir ); dir_itr != end_iter;  ++dir_itr )
	{
		// If there is a query string after the extension, take it off!
		fs::path file = dir_itr->path();
		string old_name = file.string();
		size_t found = old_name.find("&");
		if (found!=string::npos)
		{
			string new_name = old_name.substr(0, found);
			fs::rename(dir_itr->path(), new_name);
			file = fs::path(new_name);
		}

		
		// Choose what to do with the the item in the iterator
		if(fs::is_directory(file))
		{
			if(recursive) scan_directory(file, level+1);
		}
		else if(is_image(file))
		{
			// read in the image as a 3-channel Mat
			Mat image = imread( file.string(), 1);
			
			fs::path cnrfile = file;
			cnrfile.replace_extension(".cnr");
			
			cout << "found an image " << file << endl;
			
			if(!fs::exists(cnrfile))
			{
				cout << "No corners file found.  Lets create one." << endl;
				generate_datafile(image, cnrfile);
			}
			
			if(fs::exists(cnrfile))
			{
				// TO DO:  Read in and use the corners from the file!
				
				
				// examine the image.
				vector<KeyPoint> keypoints;
				Mat descriptors;
				
				cout << "Identifying keypoints" << endl;
				detector.detect(image, keypoints);
				
				cout << "Extracting descriptors " << endl;
				extractor.compute(image, keypoints, descriptors);
				
				all_descriptors.push_back( descriptors );
			}
		}
		else
		{
			cout << "not recognized: " << file << endl;
		}
	} // end for(all files in directory)
	
	
	if(all_descriptors.size() > 0)
	{
		fs::path datafile(dir.parent_path() / dir.filename());
		datafile.replace_extension(".xml");
		
		cout << "saving data to " << datafile << endl;
		
		FileStorage descriptors_fs;
		descriptors_fs.open(datafile.string(), FileStorage::WRITE);
		if(descriptors_fs.isOpened())
		{
			char nodename[255];
			for(int i=0; i<all_descriptors.size(); i++)
			{
				sprintf(nodename, "descriptors_%d", i);
				descriptors_fs << string(nodename) << all_descriptors[i];
				
			}
			descriptors_fs.release();
		}
	}
}




// ----------------------------------------------------
bool is_image(fs::path p)
{
	if(!is_regular_file(p))
	{
		return false;
	}
	string ext = extension(p);
	boost::to_lower(ext);
	const char *image_exts[] = {".jpg", ".jpeg", ".png", ".tiff", ".tif"};
	for(int i=0; i<5; i++)
	{
		if(boost::starts_with(ext, image_exts[i]))	
		{
			return true;
		}
	}
	return false;
}




// ----------------------------------------------------
void generate_datafile(Mat image, fs::path data_path)
{
	corners.clear();
		
	// We need to construct an image with a margin around it
	// This is because the bounds of the "plane" on which the logo exists might
	// extend outside of the image.
	image_w_margin = Mat(Size(image.size().width+(margin*2), image.size().height+(margin*2)), image.type());
	image_w_margin = Scalar(255,255,255);
	
	// Copy the image from the file into the one with the margin
	Mat roi = image_w_margin(Rect(50, 50, image.size().width, image.size().height));
	image.copyTo( roi );
	
	// Show the image
	imshow("image", image_w_margin);
	setMouseCallback("image", on_mouse);
	
	bool done=false;
	do {
		int key = waitKey(100);
		switch (key) {
			case 27:
				done=true;
				break;
			case 'c':
				generate_datafile(image, data_path);
				break;
			default:
				break;
		}
		imshow("image", image_w_margin);
		if(corners.size() > 3)
		{
			done=true;
		}
	} while(!done);
	
	if(corners.size()==4)
	{
		ofstream myfile;
		myfile.open(data_path.string().c_str(), ios::trunc);
		if(myfile.is_open())
		{
			for(int i=0; i<corners.size(); i++)
			{
				myfile << corners[i].x << " " << corners[i].y << endl;
			}
			myfile.close();
		}
	}
	corners.clear();
	//destroyWindow(window_name);
	waitKey(100);
	//destroyAllWindows();
}

// ----------------------------------------------------
void on_mouse( int event, int x, int y, int flags, void* param )
{
	if(event==CV_EVENT_LBUTTONUP)
	{
		circle(image_w_margin, Point(x,y), 2, Scalar(0,0,255), 2);
		corners.push_back(Point(x-margin, y-margin));
	}
}
