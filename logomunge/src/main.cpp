
#include <iostream>
#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"   

using namespace std;
using namespace boost::filesystem;
using namespace boost::program_options;

int main (int argc, char * const argv[])
{
	options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("version,v", "print version string")
		("verbose", value<int>()->default_value(1), "enable verbosity (optionally specify level)")
		("path,p", value< vector<string> >(), "a path to the directory that contains logos")
		;

	variables_map vm;        
	store(parse_command_line(argc, argv, desc), vm);
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
		cout << "Verbosity enabled. Level is " << vm["verbose"].as<int>() << endl;
	}
	
	if (!vm.count("path"))
	{
		cout << "No paths provided.  Exiting." << endl;
		return 0;
	}
	
	vector<string> paths = vm["path"].as< vector<string> >();
	for(int i=0; i<paths.size(); i++)
	{
		path dir_path( paths[i] );
		if( !exists( dir_path ) ) 
		{
			cout << "\t" << dir_path << " doesn't exist.  Skipping." << endl;
			continue;
		}

		cout << "\tOpening " << dir_path << endl;
		directory_iterator end_iter;
		for (directory_iterator dir_itr( dir_path ); dir_itr != end_iter;  ++dir_itr )
		{
			if ( is_regular_file( dir_itr->status() ) )
			{
				path model_image_path = dir_itr->path();
				cout << "\t\t" << "Found a model image: " << model_image_path << "\n";
				
				string stem = dir_itr->path().stem().string();
				path instances_dir = path( dir_path / stem );
				if( exists( instances_dir ) ) 
				{
					cout << "\t\t\tLooking for instances in " << instances_dir << endl;
					
					
				}
				else 
				{
					cout << "\t\t" << instances_dir << " doesn't exist.  Skipping." << endl;
				}
		

			}
		}
	}
}
