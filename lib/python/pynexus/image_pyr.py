import ctypes as _C

import __init__ as _NX
import image

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_float = _C.c_float

Type = _C.c_int

(FAST, FINE, SCALED) = (0, 1, 2)

class Level(_C.Structure):
    _fields_ = [('img', image.Ptr),
                ('sigma', _c_float),
                ('scale', _c_float)]

class FastInfo(_C.Structure):
    _fields_ = [('width0', _c_int),
                ('height0', _c_int),
                ('sigma0', _c_float)]

class FineInfo(_C.Structure):
    _fields_ = [('width0', _c_int),
                ('height0', _c_int),
                ('n_octaves', _c_int),
                ('n_octave_steps', _c_int),
                ('sigma0', _c_float)]

class ScaledInfo(_C.Structure):
    _fields_ = [('width0', _c_int),
                ('height0', _c_int),
                ('scale_factor', _c_float),
                ('sigma0', _c_float)]

class Info(_C.Union):
    _fields_ = [('fast', FastInfo),
                ('fine', FineInfo),
                ('scaled', ScaledInfo)]


class Struct(_C.Structure):
    _fields_ = [('n_levels', _c_int),
                ('levels', _POINTER(Level)),
                ('type', Type),
                ('info', Info),
                ('work_img', image.Ptr)]

    def __str__(self):
        return "<pynexus.image_pyr.Struct: {} Levels>".format(self.n_levels);

Ptr = _POINTER(Struct)

alloc = _nexus.nx_image_pyr_alloc
alloc.restype = Ptr
alloc.argtypes = []

new_fast = _nexus.nx_image_pyr_new_fast
new_fast.restype = Ptr
new_fast.argtypes = [_c_int, _c_int, _c_int, _c_float]

new_fine = _nexus.nx_image_pyr_new_fine
new_fine.restype = Ptr
new_fine.argtypes = [_c_int, _c_int, _c_int, _c_int, _c_float]

new_scaled = _nexus.nx_image_pyr_new_scaled
new_scaled.restype = Ptr
new_scaled.argtypes = [_c_int, _c_int, _c_int, _c_float, _c_float]

free = _nexus.nx_image_pyr_free
free.restype = None
free.argtypes = [Ptr]

resize_fast = _nexus.nx_image_pyr_resize_fast
resize_fast.restype = None
resize_fast.argtypes = [Ptr, _c_int, _c_int, _c_int, _c_float]

resize_fine = _nexus.nx_image_pyr_resize_fine
resize_fine.restype = None
resize_fine.argtypes = [Ptr, _c_int, _c_int, _c_int, _c_int, _c_float]

resize_scaled = _nexus.nx_image_pyr_resize_scaled
resize_scaled.restype = None
resize_scaled.argtypes = [Ptr, _c_int, _c_int, _c_int, _c_float, _c_float]

release = _nexus.nx_image_pyr_release
release.restype = None
release.argtypes = [Ptr]

compute = _nexus.nx_image_pyr_compute
compute.restype = None
compute.argtypes = [Ptr, image.Ptr]

update = _nexus.nx_image_pyr_update
update.restype = None
update.argtypes = [Ptr]

copy0 = _nexus.nx_image_pyr_copy0
copy0.restype = Ptr
copy0.argtypes = [Ptr]

copy = _nexus.nx_image_pyr_copy
copy.restype = None
copy.argtypes = [Ptr, Ptr]

