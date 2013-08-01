import ctypes as _C

import __init__ as _NX

import memblock
import image
import image_warp

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

class Struct(_C.Structure):
    _fields_ = [("width", _c_int),
                ("height", _c_int),
                ("n_channels", _c_int),
                ("mem", memblock.Ptr),
                ("data", _POINTER(_c_float)),
                ("row_stride", _c_int)]

    def __str__(self):
        return "<pynexus.float_image.Struct: {}x{}x{}>".format(self.width, self.height, self.n_channels);

Ptr = _POINTER(Struct)

alloc = _nexus.nx_float_image_alloc
alloc.restype = Ptr
alloc.argtypes = []

new = _nexus.nx_float_image_new
new.restype = Ptr
new.argtypes = [_c_int, _c_int, _c_int]

new_gray = _nexus.nx_float_image_new_gray
new_gray.restype = Ptr
new_gray.argtypes = [_c_int, _c_int]

new_rgba = _nexus.nx_float_image_new_rgba
new_rgba.restype = Ptr
new_rgba.argtypes = [_c_int, _c_int]

free = _nexus.nx_float_image_free
free.restype = None
free.argtypes = [Ptr]

resize = _nexus.nx_float_image_resize
resize.restype = None
resize.argtypes = [Ptr, _c_int, _c_int, _c_int, _c_int]

release = _nexus.nx_float_image_release
release.restype = None
release.argtypes = [Ptr]

copy0 = _nexus.nx_float_image_copy0
copy0.restype = Ptr
copy0.argtypes = [Ptr]

copy = _nexus.nx_float_image_copy
copy.restype = None
copy.argtypes = [Ptr, Ptr]

set_zero = _nexus.nx_float_image_set_zero
set_zero.restype = None
set_zero.argtypes = [Ptr]

wrap = _nexus.nx_float_image_wrap
wrap.restype = None
wrap.argtypes = [Ptr, _POINTER(_c_float), _c_int, _c_int, _c_int, _c_int, _NX.Bool]

swap = _nexus.nx_float_image_swap
swap.restype = None
swap.argtypes = [Ptr, Ptr]

filter_buffer_alloc = _nexus.nx_float_image_filter_buffer_alloc
filter_buffer_alloc.restype = _POINTER(_c_float)
filter_buffer_alloc.argtypes = [_c_int, _c_int, _c_float, _c_float]

smooth_s = _nexus.nx_float_image_smooth_s
smooth_s.restype = None
smooth_s.argtypes = [Ptr, Ptr, _c_float, _c_float, _POINTER(_c_float)]

transform_affine = _nexus.nx_float_image_transform_affine
transform_affine.restype = None
transform_affine.argtypes = [image.Ptr, Ptr, _POINTER(_c_float), image_warp.BackgroundMode]

transform_affine_prm = _nexus.nx_float_image_transform_affine_prm
transform_affine_prm.restype = None
transform_affine_prm.argtypes = [image.Ptr, Ptr, _c_float, _c_float, _c_float, _c_float, image_warp.BackgroundMode]

xsave_raw = _nexus.nx_float_image_xsave_raw
xsave_raw.restype = None
xsave_raw.argtypes = [Ptr, _c_char_p]

xload_raw = _nexus.nx_float_image_xload_raw
xload_raw.restype = None
xload_raw.argtypes = [Ptr, _c_char_p]

save_raw = _nexus.nx_float_image_save_raw
save_raw.restype = _NX.Result
save_raw.argtypes = [Ptr, _c_char_p]

load_raw = _nexus.nx_float_image_load_raw
load_raw.restype = _NX.Result
load_raw.argtypes = [Ptr, _c_char_p]

from_uchar0 = _nexus.nx_float_image_from_uchar0
from_uchar0.restype = Ptr
from_uchar0.argtypes = [image.Ptr]

from_uchar = _nexus.nx_float_image_from_uchar
from_uchar.restype = None
from_uchar.argtypes = [Ptr, image.Ptr]

to_uchar = _nexus.nx_float_image_to_uchar
to_uchar.restype = None
to_uchar.argtypes = [image.Ptr, Ptr]

spline_coeff_of = _nexus.nx_float_image_spline_coeff_of
spline_coeff_of.restype = None
spline_coeff_of.argtypes = [Ptr, image.Ptr]
