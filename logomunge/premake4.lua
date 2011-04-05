--by Jeff Crouse (March 30, 2011)
--Run 'premake4 gmake' to generate buildfiles


solution "LogoMunge" 
	configurations { "Debug", "Release" }
	
project "logomunge"
	kind "ConsoleApp"
	language "C++"
	files { "src/*.h", "src/*.cpp" }
	links { 
		"opencv_core",  "opencv_highgui", 
		"opencv_features2d", "opencv_imgproc", 
		"opencv_objdetect", "boost_program_options", 
		"boost_system", "boost_filesystem"}
	libdirs { 
		"/opt/local/lib", 
		"/usr/local/lib", 
		os.findlib("opencv_core"),
		os.findlib("boost_system")
	}
	includedirs { 
		"/opt/local/include",
		"/usr/local/include",
		"/usr/include"
	}
	
configuration "Debug"
	defines { "DEBUG" }
	flags { "Symbols" }
	
configuration "Release"
	defines { "NDEBUG" }
	flags { "Optimize" }  