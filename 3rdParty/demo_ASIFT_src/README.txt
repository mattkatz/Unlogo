Demo code for Affine-SIFT (ASIFT) image matching
-------------------------------------------------------------------------
-------------------------------------------------------------------------
Jean-Michel Morel (Jean-Michel Morel <morel@cmla.ens-cachan.fr>)
Guoshen Yu (yu@cmap.polytechnique.fr)

Version 2.1.2, Jan. 9, 2011

This directory contains the C++ code of ASIFT, a fully affine invariant image 
matching algorithm. 

************************** Unix/Linux/Mac Users **************************
For Unix/Linux and Mac users, the code is ready to be compiled. The 
executable adapted to your computer system is generated after compiling.
**************************************************************************

***************************** Windows Users ******************************
For Windows users, the executable as well as the code is provided. 
(For the executable, you need to download separately the installation file from 
http://www.ipol.im/pub/algo/my_affine_sift/ .)
**************************************************************************

**************************** Matlab Interface ****************************
Although the ASIFT program is standalone and can be executed without Matlab,
a Matlab interface is provided (for Unix/Linux/Mac/Windows users). 
**************************************************************************

If you have any problem using the this program, please contact Guoshen Yu 
yu@cmap.polytechnique.fr
				   
For more information about ASIFT, please see the web page at 
http://www.ipol.im/pub/algo/my_affine_sift/.
You can also try ASIFT using the online demo. The online demo allows testing
ASIFT with your own images without installing the program. 

If you use the ASIFT code or software, please cite the following paper: 
J.M. Morel and G.Yu, ASIFT: A New Framework for Fully Affine Invariant Image 
Comparison, SIAM Journal on Imaging Sciences, vol. 2, issue 2, pp. 438-469, 2009. 

-------------------------------------------------------------------------
-------------------------------------------------------------------------

UNIX/LINUX/MAC USER GUIDE

The code is compilable on Unix/Linux and Mac OS. 

- Compilation. 
Automated compilation requires the make program.

- Library. 
This code requires the libpng library. You can automatically download, 
compile and include this library to the compiled program by adding the 
LOCAL_LIBS=1 option to the make commands.

- Image format. 
Only the PNG format is supported. 
 
-------------------------------------------------------------------------
Usage:
1. Download the ASIFT code package and extract it. Go to that directory. 

2. Compile the source code (on Unix/Linux/Mac OS). 
There are two ways to compile the code. 
(1) RECOMMENDED, with Open Multi-Processing multithread parallelization 
(http://openmp.org/). Roughly speaking, it accelerates the program using the 
multiple processors in the computer. Run
make OMP=1

OR
(2) If the complier does not support OpenMp, run 
make

ATTENTION:
If libpng (the official PNG reference library) is not installed in your computer, 
an option LOCAL_LIBS=1 should be added after make. Example
make OMP=1 LOCAL_LIBS=1
The compilation will automatically download and compile libpng and zlib and include 
the library to the program.

3. Run ASIFT. 
./demo_ASIFT imgIn1.png, imgIn2.png imgOutVert.png imgOutHori.png matchings.txt keys1.txt keys2.txt

-- imgIn1.png, imgIn2.png: Input images (in png format).
-- imgOutVert.png, imgOutHori.png: Output images (vertical/horizontal concatenated). 
The detected matches are connected by write lines.
-- matchings.txt: The file format starts with 1 integer giving the total number 
of matches. Then each line specifies the coordinates (col1, row1, col2, row2) of a pair
 of matched points. (col: horizontal axis, from left to right. row: vertical axis,
 from top to bottom.)
-- keys1.txt keys2.txt: ASIFT keypoints in the two images, in the same format 
as the SIFT keypoints of David Lowe. The file starts with 2 integers giving 
the total number of keypoints and the length of the descriptor vector for each 
keypoint (128). Then the location of each keypoint in the image is specified 
by 4 floating point numbers giving subpixel column and row location, scale, 
and orientation (in radians from -PI to PI). Finally, the invariant descriptor
 vector for the keypoint is given as a list of 128 integers in range [0,255]. 
-- [optional 0/1]. 1: input images resize to an area equal to 800x600 for ASIFT,
 in keeping the aspect ratio (by default). 0: no resize. The resize is to limit
 the ASIFT computation time. The results (output images, keypoint coordinates
 and scales) are normalized to the original image size, so the resize is 
"transparent" to the user. 

Example, run
./demo_ASIFT adam1.png adam2.png imgOutVert.png imgOutHori.png matchings.txt keys1.txt keys2.txt 

You get on the screen 
"WARNING: The input images are resized to 800x600 for ASIFT. 
         But the results will be normalized to the original image size.

Computing keypoints on the two images...
12928 ASIFT keypoints are detected. 
8972 ASIFT keypoints are detected. 
Keypoints computation accomplished in 24 seconds.
Matching the keypoints...
The two images match! 914 matchings are identified. log(nfa)=-1496.88.
Keypoints matching accomplished in 4 seconds."

-------------------------------------------------------------------------
-------------------------------------------------------------------------
WINDOWS USER GUIDE
(For the executable, you need to download separately the installation file from 
http://www.ipol.im/pub/algo/my_affine_sift/.)

- The provided Windows executable demo_ASIFT.exe has been compiled by the Intel C++ 
compiler on 32-bit Windows. It is executable on both 32-bit and 64-bit Windows, 
although it is not optimized for the latter. 

- The executable has not been extensively tested. If you have any problem using it,
please contact Guoshen Yu yu@cmap.polytechnique.fr. 

- The source code is compilable with the Intel C++ compiler and Visual
C++ compiler. To use ASIFT, you do not need to recompile the code, but
you want to do so, Intel C++ is recommended since it supports both OpenMP
and vectorization. 

In order to have the code compilable with Intel C++ and Visual C++ compilers, 
the following minor changes of the code may be required. 
* In ./io_png/io_png.c
#include <png.h> ----> #include "./libs/png/png.h"

* In demo_lib_sift.cpp
unsigned short distsq = 0; ----> int distsq = 0;

-------------------------------------------------------------------------
Usage:
1. Download the installation file demo_ASIFTsetp.exe from 
http://www.ipol.im/pub/algo/my_affine_sift/

2. Install the program.
Double click the file demo_ASIFTsetp.exe. A small library distributed by Microsoft 
(Microsoft Visual C++ 2010 Redistributable Package) will be installed to your PC. 
The ASIFT software will be installed to C:\Program Files\demo_ASIFT

3. Run ASIFT. 
Run a Dos command prompt (you find it in Start > All Programs > Accessories > Command Prompt)
- Go to the ASIFT directory by typing
cd C:\Program Files\demo_ASIFT.
- Run the ASIFT program by typing
demo_ASIFT adam1.png adam2.png imgOutVert.png imgOutHori.png matchings.txt keys1.txt keys2.txt
(It follows the same syntax of that for Unix/Linux/Mac described above.)

You can of course move the ASIFT directory C:\Program Files\demo_ASIFT to wherever that
is more convenient. 

-------------------------------------------------------------------------
Troubleshooting 
1. If you are able to run the program but the results are not written to 
the output files, check and make sure that you have the write file permission 
in the demo_ASIFT directory.

2. Microsoft Visual Studio is NOT required to run the program. However, 
in case you cannot run the program (for example some library or dll is missing), 
you may try installing a Microsoft Visual Studio and then running again 
the program. 

-------------------------------------------------------------------------
-------------------------------------------------------------------------
MATLAB INTERFACE (OPTIONAL)
Run ASIFT via Matlab: Open test_demo_ASIFT.m in Matlab and execute the script. 
The Matlab interface reads most standard image formats.

-------------------------------------------------------------------------
-------------------------------------------------------------------------
CREDITS
- The epipolar geometry filtering algorithm ORSA of Moisan and Stival is 
used at the end of the ASIFT algorithm 
to eliminate false matches. 
http://www.math-info.univ-paris5.fr/~moisan/epipolar/

- We would like to thank Nicolas Limare for his help on developing the program, 
and Pierre Moulon for his insights on producing the executable for Windows. 

-------------------------------------------------------------------------
-------------------------------------------------------------------------
Licensing conditions

This software is being made available for research purposes only.  It
is necessary to obtain a license from Ecole Polytechnique, ENS Cachan, 
Jean-Michel Morel, and Guoshen Yu for commercial applications. See the
 file LICENSE in this directory for conditions of use.
