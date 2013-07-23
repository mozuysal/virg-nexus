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

Type = _C.c_int
LoadMode = _C.c_int

(GRAYSCALE, RGBA) = (0, 1)
(LOAD_AS_IS, LOAD_GRAYSCALE, LOAD_RGBA) = (-1, 0, 1)

class Struct(_C.Structure):
    _fields_ = [("width", _c_int),
                ("height", _c_int),
                ("type", Type),
                ("n_channels", _c_int),
                ("mem", memblock.Ptr),
                ("data", _POINTER(_NX.uchar)),
                ("row_stride", _c_int)]

    def __str__(self):
        return "<pynexus.image.Struct: {}x{}x{}>".format(self.width, self.height, self.n_channels);

Ptr = _POINTER(Struct)

alloc = _nexus.nx_image_alloc
alloc.argtypes = []
alloc.restype = Ptr

new = _nexus.nx_image_new
new.argtypes = [_c_int, _c_int, Type]
new.restype = Ptr

new_gray = _nexus.nx_image_new_gray
new_gray.argtypes = [_c_int, _c_int]
new_gray.restype = Ptr

new_rgba = _nexus.nx_image_new_rgba
new_rgba.argtypes = [_c_int, _c_int]
new_rgba.restype = Ptr

free = _nexus.nx_image_free
free.argtypes = [Ptr]
free.restype = None

resize = _nexus.nx_image_resize
resize.argtypes = [Ptr, _c_int, _c_int, _c_int, Type]
resize.restype = None

release = _nexus.nx_image_release
release.argtypes = [Ptr]
release.restype = None

copy0 = _nexus.nx_image_copy0
copy0.argtypes = [Ptr]
copy0.restype = Ptr

copy = _nexus.nx_image_copy
copy.argtypes = [Ptr, Ptr]
copy.restype = None

set_zero = _nexus.nx_image_set_zero
set_zero.argtypes = [Ptr]
set_zero.restype = None

wrap = _nexus.nx_image_wrap
wrap.argtypes = [Ptr, _POINTER(_NX.uchar), _c_int, _c_int, _c_int, Type, _NX.Bool]
wrap.restype = None

swap = _nexus.nx_image_swap
swap.argtypes = [Ptr, Ptr]
swap.restype = None

convert = _nexus.nx_image_convert
convert.argtypes = [Ptr, Ptr]
convert.restype = None

scale = _nexus.nx_image_scale
scale.argtypes = [Ptr, Ptr, _c_float]
scale.restype = None

downsample = _nexus.nx_image_downsample
downsample.argtypes = [Ptr, Ptr]
downsample.restype = None

filter_buffer_alloc = _nexus.nx_image_filter_buffer_alloc
filter_buffer_alloc.argtypes = [_c_int, _c_int, _c_float, _c_float]
filter_buffer_alloc.restype = _POINTER(_NX.uchar)

smooth_si = _nexus.nx_image_smooth_si
smooth_si.argtypes = [Ptr, Ptr, _c_float, _c_float, _POINTER(_NX.uchar)]
smooth_si.restype = None

smooth_s = _nexus.nx_image_smooth_s
smooth_s.argtypes = [Ptr, Ptr, _c_float, _c_float, _POINTER(_NX.uchar)]
smooth_s.restype = None

xsave_pnm = _nexus.nx_image_xsave_pnm
xsave_pnm.argtypes = [Ptr, _c_char_p]
xsave_pnm.restype = None

xload_pnm = _nexus.nx_image_xload_pnm
xload_pnm.argtypes = [Ptr, _c_char_p, LoadMode]
xload_pnm.restype = None

save_pnm = _nexus.nx_image_save_pnm
xsave_pnm.argtypes = [Ptr, _c_char_p]
xsave_pnm.restype = _NX.Result

load_pnm = _nexus.nx_image_load_pnm
xload_pnm.argtypes = [Ptr, _c_char_p, LoadMode]
xload_pnm.restype = _NX.Result
