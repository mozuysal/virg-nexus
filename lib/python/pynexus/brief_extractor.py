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

nx_brief_extractor_alloc.restype = Ptr
nx_brief_extractor_alloc.argtypes = []

nx_brief_extractor_new.restype = Ptr
nx_brief_extractor_new.argtypes = [_c_int, _c_int]

nx_brief_extractor_free.restype = None
nx_brief_extractor_free.argtypes = [Ptr]

nx_brief_extractor_resize.restype = None
nx_brief_extractor_resize.argtypes = [Ptr, _c_int, _c_int]

nx_brief_extractor_randomize.restype = None
nx_brief_extractor_randomize.argtypes = [Ptr]

nx_brief_extractor_update_limits.restype = None
nx_brief_extractor_update_limits.argtypes = [Ptr]

nx_brief_extractor_check_point_pyr.restype = NXBool
nx_brief_extractor_check_point_pyr.argtypes = [Ptr, image_pyr.Ptr, _c_int, _c_int, _c_int]

nx_brief_extractor_compute_pyr.restype = None
nx_brief_extractor_compute_pyr.argtypes = [Ptr, image_pyr.Ptr, _c_int, _c_int, _c_int, _POINTER(_NX.uchar)]

nx_brief_extractor_descriptor_distance.restype = _c_int
nx_brief_extractor_descriptor_distance.argtypes = [_c_int, _POINTER(_NX.uchar), _POINTER(_NX.uchar)]
