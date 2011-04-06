--by Jeff Crouse (March 30, 2011)
--Run 'premake4 gmake' to generate buildfiles


solution "Unlogo" 
	configurations { "Debug", "Release" }

project "unlogo"
	kind "StaticLib"
	language "C++"
	files {  "src/*.h", "src/*.cpp", }
	links { 
		"opencv_core",  "opencv_highgui", 
		"opencv_imgproc", "opencv_objdetect"}
	libdirs { 
		"/opt/local/lib", 
		"/usr/local/lib", 
		os.findlib("opencv_core"),
	}
	includedirs { 
		"../src/*",
		"../src/Tools/ASIFT",
		"../src/Utils/JSONElement",
		"../src/Utils/JSONElement/jsoncpp/include",
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