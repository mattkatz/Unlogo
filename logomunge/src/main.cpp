
#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include <iostream>               // for std::cout


using namespace std;
using namespace boost::filesystem;
using namespace boost::program_options;


int main (int argc, char * const argv[])
{
	options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("version,v", "print version string")
		("verbose", value<int>()->implicit_value(1), "enable verbosity (optionally specify level)")
		("path,p", value< vector<string> >(), "the path to the directory that contains the logos")
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
	
	if (vm.count("verbose"))
	{
		cout << "Verbosity enabled. Level is " << vm["verbose"].as<int>() << endl;
	}
	
	if (vm.count("version"))
	{
		cout << "logomunge, version 1.0\n";
		return 0;
	}
	
	
	if (!vm.count("path"))
	{
		cout << "No paths provided.  Exiting.";
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
				cout << "\t\t" << dir_itr->path().filename() << "\n";
				
				cout << "\t\tLooking for " << path( dir_path / dir_itr->path().stem() ) << endl;
			}
		}
	}
}
