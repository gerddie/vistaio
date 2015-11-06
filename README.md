# Resume

This library includes the IO and data type part of the 
vista library originaly implemented by 

  Arthur Pope, University of British Columbia 
  Laboratory or Computational Intelligence

Further development was done by Gert Wollny, Marc Tittgemeyer, 
and Heike Jähnike at the Max-Planck-Institute of Human 
Cognitive and Brain Sciences, Leipzig, Germany, and later by 
Gert Wollny at the ETSI Telecomunicacion, UPM, Madrid, Spain.  


# Installation

## Pre-packaged

* The library is available as libvistaio-dev/libvistaio14 from
  the official Debian archives.
* Ebuild for Gentoo Linux are available from the overlay gentoo-imaging
  (currently not availabe from _layman_)

     https://github.com/gerddie/gentoo-imaging

## Compilation from source: 

Requirements:

 * cmake - http://www.cmake.org
 * a  POSIX compatible C compiler (GNU gcc will do just fine). 

Then after unpacking the software to

    vistaio-X.X.X one does: 
    cd vistaio-X.X.X
    mkdir build 
    cd build 
    cmake .. -DCMAKE_INSTALL_PATH=<prefix> 
    make 
    make test 
    make install 

The "make install" command you may have to run as root. 
The tests cover only the 2d/3dfield loading and storing, and
the attribute IO. 

# Function naming remark 

In order to avoid naming conflicts with other libraries that build 
onm the same source code, all functions, types and macro names have 
been  changed from the name V.* to VistaIO.*. A python script to 
autoatically apply such name schange to a source code tree is included
in the source code distribtuion. 

just run it as 

  python vistaio_change_names.py <source tree root>

Note however, that it will replace all "V\w*", i.e. also those not 
related to VistaIO. 

The original vista library (converted to the automake build
system) is still available from 
  http://sourceforge.net/projects/mia/files/vista/2.1.7/

This implementation should be able to read all files that were 
stored using the original library. If data is saved to one of the 
original formats the original library should also be able to read 
the files properly.


# About endianess: 

The vista file format was initially developed on a big-endian machine, 
as a result Arthur Pope decided it is best to store the data in big-endian 
format on the disk. Therefore, if you work on a low-endian machine (e.g. 
all Intel & compatibles) all the data stored in the original formats is 
converted during loading and storing. If, on the other hand you are loading 
or storing to from or to one of the new formats, the data is written in 
low-endian format, but on a big endian machine it will also be automatically 
converted.
