# pmutils: Image manipulation tools for old DOS PrintMaster software

This is a set of tools to list, display and insert new image in a
PrintMaster image library, but under Linux.

I made this because I wanted to add new images to the library
and refused to use the extremely rudimentary built-in editor. Now the new images
can be prepared in (for instance) Gimp, saved in .PNG format and appeneded to the library
using these tools.

## The library concept

There is the standard image library (SLIB.SDR/SLIB.SHP) containing the original
122 images. User-defined images are saved in ULIB.SDR/ULIB.SHP.

The .SDR files contain records of 16 bytes representing the name/description for
each image in the library.

The .SHP files contains the corresponding images. Each image has a header containing
the image size (Normally 88x52) and the uncompressed image data.

## The tools

pmutils provides the following tools.

### pmdir

The pmdir tool simply lists the image names from a .SDR file.

### pmshow

The pmshow tool displays a specified images from a .SHP file on a terminal.

### pmappend

The pmappend tool converts a PNG image and adds appends it to an existing
library (.SHP and .SDR file).

Notes:
- PNG must be 1 bit per pixel
- PNG _should_ probably be 88x52 as this seems to be the standard used.

## License

Public Domain
