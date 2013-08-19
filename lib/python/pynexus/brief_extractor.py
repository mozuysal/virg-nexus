import ctypes as _C

import __init__ as _NX

import image_pyr

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_float = _C.c_float

class Struct(_C.Structure):
    _fields_ = [
        ('radius', _c_int),
        ('n_octets', _c_int),
        ('offsets', _POINTER(_c_int)),
        ('offset_limits', _c_int * 4),
        ('pyr_level_offset', _c_int),
        ]

    def __str__(self):
        return "<pynexus.brief_extractor.Struct: {} bits at radius {}>".format(self.n_octets * 8, self.radius);

Ptr = _POINTER(Struct)


alloc = _nexus.nx_brief_extractor_alloc
alloc.restype = Ptr
alloc.argtypes = []

new = _nexus.nx_brief_extractor_new
new.restype = Ptr
new.argtypes = [_c_int, _c_int]

free = _nexus.nx_brief_extractor_free
free.restype = None
free.argtypes = [Ptr]

resize = _nexus.nx_brief_extractor_resize
resize.restype = None
resize.argtypes = [Ptr, _c_int, _c_int]

randomize = _nexus.nx_brief_extractor_randomize
randomize.restype = None
randomize.argtypes = [Ptr]

update_limits = _nexus.nx_brief_extractor_update_limits
update_limits.restype = None
update_limits.argtypes = [Ptr]

check_point_pyr = _nexus.nx_brief_extractor_check_point_pyr
check_point_pyr.restype = _NX.Bool
check_point_pyr.argtypes = [Ptr, image_pyr.Ptr, _c_int, _c_int, _c_int]

compute_pyr = _nexus.nx_brief_extractor_compute_pyr
compute_pyr.restype = None
compute_pyr.argtypes = [Ptr, image_pyr.Ptr, _c_int, _c_int, _c_int, _POINTER(_NX.uchar)]

descriptor_distance = _nexus.nx_brief_extractor_descriptor_distance
descriptor_distance.restype = _c_int
descriptor_distance.argtypes = [_c_int, _POINTER(_NX.uchar), _POINTER(_NX.uchar)]
