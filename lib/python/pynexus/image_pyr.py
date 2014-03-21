import ctypes as _C

import __init__ as _NX
import image

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_float = _C.c_float

class Level(_C.Structure):
    _fields_ = [('img', image.Ptr),
                ('sigma', _c_float),
                ('scale', _c_float)]



class Struct(_C.Structure):
    _fields_ = [('n_levels', _c_int),
                ('levels', _POINTER(Level))]

    def __str__(self):
        return "<pynexus.image_pyr.Struct: {} Levels>".format(self.n_levels);

Ptr = _POINTER(Struct)

alloc = _nexus.nx_image_pyr_alloc
alloc.restype = Ptr
alloc.argtypes = []

free = _nexus.nx_image_pyr_free
free.restype = None
free.argtypes = [Ptr]

alloc_levels = _nexus.nx_image_pyr_alloc_levels
alloc_levels.restype = None
alloc_levels.argtypes = [Ptr, _c_int]

free_levels = _nexus.nx_image_pyr_free_levels
free_levels.restype = None
free_levels.argtypes = [Ptr]

create_level = _nexus.nx_image_pyr_create_level
create_level.restype = None
create_level.argtypes = [Ptr, _c_int, _c_int, _c_int, _c_float, _c_float]

copy_to_level0 = _nexus.nx_image_pyr_copy_to_level0
copy_to_level0.restype = None
copy_to_level0.argtypes = [Ptr, image.Ptr, _c_float]
