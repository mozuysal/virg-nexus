import ctypes as _C

import __init__ as _NX

import memblock
import image

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_short = _C.c_short
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

LoadMode = _C.c_int
(LOAD_AS_IS, LOAD_GRAYSCALE, LOAD_RGBA) = (-1, 0, 1)

xsave_pnm = _nexus.nx_image_xsave_pnm
xsave_pnm.argtypes = [image.Ptr, _c_char_p]
xsave_pnm.restype = None

xload_pnm = _nexus.nx_image_xload_pnm
xload_pnm.argtypes = [image.Ptr, _c_char_p, LoadMode]
xload_pnm.restype = None

save_pnm = _nexus.nx_image_save_pnm
xsave_pnm.argtypes = [image.Ptr, _c_char_p]
xsave_pnm.restype = _NX.Result

load_pnm = _nexus.nx_image_load_pnm
xload_pnm.argtypes = [image.Ptr, _c_char_p, LoadMode]
xload_pnm.restype = _NX.Result
