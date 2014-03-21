import ctypes as _C

import __init__ as _NX
import image
import image_pyr

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_float = _C.c_float

Type = _C.c_int

INITIAL_SIGMA = 0.5
BUILDER_NONE, BUILDER_FAST, BUILDER_FINE, BUILDER_SCALED = (-1, 0, 1, 2)
TYPE_STRINGS = {
    BUILDER_NONE : 'NONE',
    BUILDER_FAST : 'FAST',
    BUILDER_FINE : 'FINE',
    BUILDER_SCALED : 'SCALED'}

class PyrInfo(_C.Structure):
    _fields_ = [('n_levels', _c_int),
                ('n_octaves', _c_int),
                ('n_octave_steps', _c_int),
                ('scale_factor', _c_float),
                ('sigma0', _c_float)]

class Struct(_C.Structure):
    _fields_ = [('type', Type),
                ('pyr_info', PyrInfo),
                ('work_img', image.Ptr)]

    def __str__(self):
        return "<pynexus.image_pyr_builder.Struct: Type {} >".format(TYPE_STRING[self.type]);

Ptr = _POINTER(Struct)

alloc = _nexus.nx_image_pyr_builder_alloc
alloc.restype = Ptr
alloc.argtypes = []

new_fast = _nexus.nx_image_pyr_builder_new_fast
new_fast.restype = Ptr
new_fast.argtypes = [_c_int, _c_float]

new_fine = _nexus.nx_image_pyr_builder_new_fine
new_fine.restype = Ptr
new_fine.argtypes = [_c_int, _c_int, _c_float]

new_scaled = _nexus.nx_image_pyr_builder_new_scaled
new_scaled.restype = Ptr
new_scaled.argtypes = [_c_int, _c_float, _c_float]

free = _nexus.nx_image_pyr_builder_free
free.restype = None
free.argtypes = [Ptr]

set_fast = _nexus.nx_image_pyr_builder_set_fast
set_fast.restype = None
set_fast.argtypes = [Ptr, _c_int, _c_float]

set_fine = _nexus.nx_image_pyr_builder_set_fine
set_fine.restype = None
set_fine.argtypes = [Ptr, _c_int, _c_int, _c_float]

set_scaled = _nexus.nx_image_pyr_builder_set_scaled
set_scaled.restype = None
set_scaled.argtypes = [Ptr, _c_int, _c_float, _c_float]

build0 = _nexus.nx_image_pyr_builder_build0
build0.restype = image_pyr.Ptr
build0.argtypes = [Ptr, image.Ptr]

build = _nexus.nx_image_pyr_builder_build
build.restype = None
build.argtypes = [Ptr, image_pyr.Ptr, image.Ptr]

update = _nexus.nx_image_pyr_builder_update
update.restype = None
update.argtypes = [Ptr, image_pyr.Ptr]

init_levels = _nexus.nx_image_pyr_builder_init_levels
init_levels.restype = None
init_levels.argtypes = [Ptr, image_pyr.Ptr, _c_int, _c_int]
