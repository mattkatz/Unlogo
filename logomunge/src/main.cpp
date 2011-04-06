#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "Compress.h"


using namespace std;
using namespace cv;
namespace fs = boost::filesystem;
namespace po = boost::program_options;


struct SURFImage {
	fs::path imgfile;			// path to the image file
	fs::path cnrfile;			// path to the corners file
	vector<KeyPoint> keypoints;	// SURF keypoints
	Mat descriptors;			// SURF descriptors
	Mat cvImg;					// The image 
};


// ARGS (set in parse_args)
bool recursive=false;
int verbose;
vector<string> directories;

// OTHER VARS
double hessianThreshold = 400;
int octaves = 3;
int octaveLayers = 4;
SurfFeatureDetector detector(hessianThreshold,octaves,octaveLayers);
SurfDescriptorExtractor extractor;
Mat image_w_margin;
vector<Point> corners;
int margin=50;

// METHODS
void parse_args(int argc, char * const argv[]);
void scan_directory(fs::path dir, int level);
bool is_image(fs::path p);
void generate_cnrfile(SURFImage& img);
void on_mouse(int event, int x, int y, int flags, void* param);
void remove_query_string(SURFImage& img);



// ----------------------------------------------------
int main(int argc, char * const argv[])
{
	parse_args(argc, argv);
	
	for(int i=0; i<directories.size(); i++)
	{
		scan_directory(fs::path( directories[i] ), 0);
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
	
	// This will store all of the information about the images in this directory
	vector<SURFImage> images;
	
	
	// Loop through the contents of the directory and assemble SURF points of any images found.
	cout << "Opening " << dir << endl;
	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_itr( dir ); dir_itr != end_iter;  ++dir_itr )
	{
		fs::path file = dir_itr->path();	// The current item in the directory
		
		
		// What kind of item is it?
		if(fs::is_directory(file))
		{
			if(recursive) scan_directory(file, level+1);
		}
		else if(is_image(file))
		{
			cout << "found an image " << file << endl;
			
			SURFImage img;
			img.imgfile = file;
			remove_query_string(img);
			img.cvImg = imread(file.string(), 1);	// read in the image as a 3-channel Mat
			generate_cnrfile(img);
			
			if(fs::exists(img.cnrfile))
			{
				// TO DO: Read in and use the corners from the file!
				// TO DO: pre-process the image! (Soebel, grayscale, etc)
				
				// examine the image.
				cout << "Identifying keypoints" << endl;
				detector.detect(img.cvImg, img.keypoints);
				
				cout << "Extracting descriptors " << endl;
				extractor.compute(img.cvImg, img.keypoints, img.descriptors);
				
				images.push_back( img );
			}
		}
		else
		{
			cout << "not recognized: " << file << endl;
		}
	} // end for(all files in directory)
	
	
	// If we found any images, make a datafile.
	if(images.size() > 0)
	{
		// Construct the path to where the data will be stored
		fs::path datafile(dir.parent_path() / dir.filename());
		datafile.replace_extension(".xml");
		cout << "saving data to " << datafile << endl;
		
		// Open up the file so we can write to it.
		FileStorage fs;
		fs.open(datafile.string(), FileStorage::WRITE);
		
		if(fs.isOpened())
		{
			fs << "total" << (int)images.size();
			fs << "logos" << "{";
			
			char nodename[255];
			for(int i=0; i<images.size(); i++)
			{
				fs << "logo" << "{";
					sprintf(nodename, "image_%d", i);
					fs << "filename" << nodename;
					write(fs, "keypoints", images[i].keypoints);
					fs << "descriptors" << images[i].descriptors;
				fs << "}";
			}
			
			fs << "}";
			fs.release();
			
			const char* f = datafile.string().c_str();
			Compress::compress(f);
		}
	}
}


// ----------------------------------------------------
// If an image has a name like myimage.jpg?fooblah=boo, rename it myimage.jpg 
void remove_query_string(SURFImage& img)
{
	// If there is a query string after the extension, take it off!
	string old_name = img.imgfile.string();
	size_t found = old_name.find("&");
	if (found!=string::npos)
	{
		string new_name = old_name.substr(0, found);
		fs::rename(old_name, new_name);
		img.imgfile = fs::path(new_name);
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
void generate_cnrfile(SURFImage& img)
{
	img.cnrfile = img.imgfile;
	img.cnrfile.replace_extension(".cnr");

	if(fs::exists(img.cnrfile))
	{
		return;
	}
	
	cout << "No corners file found.  Lets create one." << endl;
	corners.clear();
		
	// We need to construct an image with a margin around it
	// This is because the bounds of the "plane" on which the logo exists might
	// extend outside of the image.
	image_w_margin = Mat(Size(img.cvImg.size().width+(margin*2), img.cvImg.size().height+(margin*2)), img.cvImg.type());
	image_w_margin = Scalar(255,255,255);
	
	// Copy the image from the file into the one with the margin
	Mat roi = image_w_margin(Rect(50, 50, img.cvImg.size().width, img.cvImg.size().height));
	img.cvImg.copyTo( roi );
	
	// Show the image
	imshow("image", image_w_margin);
	setMouseCallback("image", on_mouse);
	
	bool done=false;
	do {
		int key = waitKey(100);
		if(key==27 || corners.size() > 3)
		{
			done=true;
		}
		if(key=='c')
		{
			generate_cnrfile(img);
		}
		imshow("image", image_w_margin);
	} while(!done);
	
	if(corners.size()==4)
	{
		ofstream myfile;
		myfile.open(img.cnrfile.string().c_str(), ios::trunc);
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
// OpenCV mouse listener
void on_mouse( int event, int x, int y, int flags, void* param )
{
	if(event==CV_EVENT_LBUTTONUP)
	{
		circle(image_w_margin, Point(x,y), 2, Scalar(0,0,255), 2);
		corners.push_back(Point(x-margin, y-margin));
	}
}


void parse_args(int argc, char* const argv[])
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
		cout << "No directories provided.  Exiting." << endl;
		exit(0);
	}
	
	directories = vm["directory"].as< vector<string> >();	
}


/*
// report a zlib or i/o error 
void zerr(int ret)
{
    fputs("zpipe: ", stderr);
    switch (ret) {
		case Z_ERRNO:
			if (ferror(stdin))
				fputs("error reading stdin\n", stderr);
			if (ferror(stdout))
				fputs("error writing stdout\n", stderr);
			break;
		case Z_STREAM_ERROR:
			fputs("invalid compression level\n", stderr);
			break;
		case Z_DATA_ERROR:
			fputs("invalid or incomplete deflate data\n", stderr);
			break;
		case Z_MEM_ERROR:
			fputs("out of memory\n", stderr);
			break;
		case Z_VERSION_ERROR:
			fputs("zlib version mismatch!\n", stderr);
    }
}


void compress_data(fs::path ipath)
{
	fs::path opath = ipath;
	opath.replace_extension(".data");
	
	cout << "compressing " << ipath << " to " << opath << endl;
	
	FILE* in  = fopen(ipath.string().c_str(), "r");
	FILE* out = fopen(opath.string().c_str(), "w+");
	
	int ret = def(in, out, Z_DEFAULT_COMPRESSION);
	if (ret == Z_OK)
	{
		remove(ipath);
	}
	else
	{
		zerr(ret);
	}
}


//Compress from file source to file dest until EOF on source.
// def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
// allocated for processing, Z_STREAM_ERROR if an invalid compression
// level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
// version of the library linked do not match, or Z_ERRNO if there is
// an error reading or writing the files. 
int def(FILE *source, FILE *dest, int level)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
	
    // allocate deflate state 
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;
	
    // compress until end of file 
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
		
        //run deflate() on input until output buffer not full, finish
		// compression if all of source has been read in 
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);	// no bad return value 
            assert(ret != Z_STREAM_ERROR);  // state not clobbered 
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);    // all input will be used 
		
        // done when last data in file processed 
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        // stream will be complete 
	
    // clean up and return 
    (void)deflateEnd(&strm);
    return Z_OK;
}
*/