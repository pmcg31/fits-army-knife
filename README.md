<h1 align="center">
  FITS Army Knife
</h1>

Contains a FITS file viewer implemented using the cftisio library and Qt. Qt should enable cross-platform support, but I have only built and run on linux so far.

It is unlikely I would have achieved success without [this code example](https://github.com/richmit/ex-CFITSIO/blob/master/fits2tga.c). The
[cfitsio documention](https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/cfitsio.html) does not really make things obvious. So, thank you Mitch Richling!

There are times when I would just like to quickly take a look at a FITS image, and I have not yet found a simple tool for this. This project
will become such a tool.

Current features:

- Zoom with scroll wheel
  - Image position under mouse pointer is maintained (i.e., zoom what is pointed at)
  - Buttons for 1:1 and zoom-to-fit
- Drag image position
- Histogram
- Statistics (min/mean/median/max)
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

You will need Qt5 and the cfitsio library. On debian based linux systems:

`sudo apt install qt5-default` for Qt

`sudo apt install libcfitsio-dev` for cfitsio

You will also need some FITS files to test with.

## Building

Create a directory called "build" (or whatever you like, really, but build is already git ignored) in the root directory of the project.

Change to the build directory and execute `qmake ..`

You will now have a Makefile. Run `make`

You should now have an executable, unless you are missing a dependency somewhere. To run:

`./fits-army-knife <path-to-fits-file>`
