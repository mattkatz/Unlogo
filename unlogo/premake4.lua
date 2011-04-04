--by Jeff Crouse (March 30, 2011)
--Run 'premake4 gmake' to generate buildfiles


solution "Unlogo" 
	configurations { "Debug", "Release" }

project "unlogo"
	kind "StaticLib"
	language "C++"
	files { 
		"src/*.h", "src/*.cpp", 
		"../src/*/*.h", "../src/*/*.cpp",
		"../3rdParty/demo_ASIFT_src/*.h", 
		"../3rdParty/demo_ASIFT_src/*.cpp",
		"../3rdParty/jsoncpp-src-0.5.0/src/lib_json/*.h", 
		"../3rdParty/jsoncpp-src-0.5.0/src/lib_json/*.cpp"
		}
	excludes {
		"../3rdParty/demo_ASIFT_src/demo_ASIFT.cpp"
	}
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
		"../3rdParty/demo_ASIFT_src",
		"../3rdParty/jsoncpp-src-0.5.0/include",
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