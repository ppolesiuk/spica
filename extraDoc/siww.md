Spica Image With Weights (SIWW format)
======================================

SIWW (Spica Image With Weights) is a simple image file format used for
representing and storing images in similar way, as images are represented
by Spica internally (see [SImage](@ref SImage.h)).
The file consists of header (at least 20 bytes long) followed by image data.
The following table shows the structure of the header. All values are
little-endian.

| Offset | Length | Contents                 |
| ------ | ------ | ------------------------ |
| 0      | 8      | Magic string: "SPICAIWW" |
| 8      | 4      | Version (currently 1)    |
| 12     | 2      | Header size (⩾ 20)       |
| 14     | 2      | Format                   |
| 16     | 2      | Width                    |
| 18     | 2      | Height                   |

#### Header size

Header size field describes length of the header in bytes. It should be at
least 20. Longer headers are possible and the meaning of extra bytes is
reserved for future versions of the format.

#### Format

Format field describes the format of the image. Basically, it contains value
of type @ref SImageFormat_t.

| Value | Name                  | Data size           |
| ----- | --------------------- | ------------------- |
| 0     | @ref SFmt_Invalid     | 0                   |
| 1     | @ref SFmt_Gray        | 8  × width × height |
| 2     | @ref SFmt_RGB         | 16 × width × height |
| 3     | @ref SFmt_SeparateRGB | 24 × width × height |

#### Image data

Exact image data organization depends on the format, but generally images
are row-major, left-to-right, top-to-bottom matrices of pixels. Pixels are
stored in little-endian byte order.

##### SFmt_Invalid

Images of this type contains no data.

##### SFmt_Gray

Gray-scale images. Each pixels is represented as a pair of single precision
floating point numbers. The second component of the pair is a weight. The real
value of a pixel is a quotient of first component and the weight. Normally
it is in range from 0 (black) to 1 (white), but out-of-range values are
accepted.

##### SFmt_RGB

Color (RGB) images. Each pixels is represented as a quadruple of single
precision floating point numbers. The last (fourth) component is a weight,
common to each of RGB channels, stored in the first (red), the second (green),
and the third (blue) component of the quadruple. As in SFmt_Gray images, to
obtain real values of pixels, values from the quadruples must be divided by
the weight.

##### SFmt_SeparateRGB

Color (RGB) images, with separate weights for each of channels. Data is
organized as a sequence of three gray-scale images (of the format described
in SFmt_Gray), representing reg, green, and blue channels respectively.
