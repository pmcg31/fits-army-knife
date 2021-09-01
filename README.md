<h1 align="center">
  FITS Army Knife
</h1>

Contains a FITS/XISF file viewer implemented using the cftisio library, PixInsight Class Library (PCL) and Qt. Qt should enable cross-platform support, but I have only built and run on linux so far.

It is unlikely I would have achieved success without [this code example](https://github.com/richmit/ex-CFITSIO/blob/master/fits2tga.c). The
[cfitsio documention](https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/cfitsio.html) does not really make things obvious. So, thank you Mitch Richling!

There are times when I would just like to quickly take a look at a FITS or XISF image, and I have not yet found a simple tool for this. This project will become such a tool.

Current features:

- FITS support
- XISF support
- Zoom with scroll wheel
  - Image position under mouse pointer is maintained (i.e., zoom what is pointed at)
  - Buttons for 1:1 and zoom-to-fit
- Drag image position
- Histogram
- Pixel Statistics (min/mean/median/max)
- Integrated screen transfer function
  - From [PixInsight Reference Documentation](https://pixinsight.com/doc/docs/XISF-1.0-spec/XISF-1.0-spec.html#__XISF_Data_Objects_:_XISF_Image_:_Display_Function__)
- Multi-file support
  - First invocation shows user interface, subsequent invocations exit after passing arguments to running instance (adding files to its list)
  - At least one file or folder must be given on first invocation
  - Subsequent invocations can add files and/or folders using same syntax as first invocation
  - Implemented using QSockets for platform independence

Feature ideas:

- Quick option to see center and corners at 1:1
- Option to highlight the brightest and/or darkest pixels
- Histogram refinements?

Those are the key ones, and some nice to haves:

- Ability to scroll through many images
  - Keep zoom/position the same
- Easy and quick Pick/Reject function
- Ability to watch a directory and automatically keep loading files as they show up
- Support for DSLR raw files? Not sure how hard (i.e., proprietary) that is yet
- Possible live stack? Depending on if I can figure out the math...

## Dependencies

You will need Qt5, the cfitsio library, and the PixInsight Class Library (PCL). On debian based linux systems:

`sudo apt install qt5-default` for Qt

`sudo apt install libcfitsio-dev` for cfitsio

PCL must be downloaded from [GitLab](https://gitlab.com/pixinsight/PCL/) and built from source.

You will also need some FITS/XISF files to test with.

## Building

**Please note that this project only builds under Linux for now**

Define environment variables needed by PCL. These are:

- `PCLDIR` - Set to the directory where you extracted PCL
- `PCLINCDIR` - `${PCLDIR}/include`
- `PCLSRCDIR` - `${PCLDIR}/src`
- `PCLBINDIR` - `${PCLDIR}/bin`
- `PCLBINDIR64` - `${PCLBINDIR}`
- `PCLLIBDIR` - `${PCLDIR}/lib/linux/x64`
- `PCLLIBDIR64` - `${PCLLIBDIR}`

Create a directory called "build" (or whatever you like, really, but build is already git ignored) in the root directory of the project.

Change to the build directory and execute `qmake ..`

You will now have a Makefile. Run `make`

You should now have an executable, unless you are missing a dependency somewhere. To run:

`./fits-army-knife <path-to-fits-or-xisf-file>`

or

`./fits-army-knife <path-to-dir-containing-fits-or-xisf-files>`

## Building PCL

Define PCL environment variables as above.

Navigate to `${PCLDIR}/src/pcl/linux/g++`.

`make`

(The version of PCL I downloaded [2021-Aug-18] did not build for me due to references to `cuda.h`. I believe that is for accelerated NVIDIA. I removed the one file that referenced this [`CUDADevice.cpp`] from the makefile and it then built without any ill effects that I have discovered yet.)

Verify that the libraries `libPCL-pxi.a` and `libRFC6234-pxi.a` exist in `${PCLLIBDIR}` and build this project as outlined in the previous section.
