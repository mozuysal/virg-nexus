import ctypes as _C

import __init__ as _NX

import memblock

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

BackgroundMode = _c_int

BLACK = 0
REPEAT = 3
MIRROR = 4
NOISE = 2
WHITE = 1

affine_bilinear = _nexus.nx_image_warp_affine_bilinear
affine_bilinear.restype = None
affine_bilinear.argtypes = [_c_int, _c_int, _POINTER(_NX.uchar), _c_int, _c_int, _c_int, _POINTER(_NX.uchar), _c_int, _POINTER(_c_float), BackgroundMode]

affine_spline = _nexus.nx_image_warp_affine_spline
affine_spline.restype = None
affine_spline.argtypes = [_c_int, _c_int, _POINTER(_NX.uchar), _c_int, _c_int, _c_int, _POINTER(_c_float), _c_int, _POINTER(_c_float), BackgroundMode]

