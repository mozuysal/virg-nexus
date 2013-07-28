import ctypes as _C

import __init__ as _NX

import memblock
import image
import image_pyr
import keypoint

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

class Struct(_C.Structure):
    _fields_ = [('max_n_keys', _c_int),
                ('threshold', _c_int),
                ('n_keys', _c_int),
                ('keys', keypoint.Ptr),
                ('n_work', _c_int),
                ('keys_work', keypoint.Ptr),
                ('mem', memblock.Ptr)]

    def __str__(self):
        return "<pynexus.fast_detector.Struct: {} of {} maximum>".format(self.n_keys, self.max_n_keys);

Ptr = _POINTER(Struct)

alloc = _nexus.nx_fast_detector_alloc
alloc.restype = Ptr
alloc.argtypes = []

new = _nexus.nx_fast_detector_new
new.restype = Ptr
new.argtypes = [_c_int, _c_int]

free = _nexus.nx_fast_detector_free
free.restype = None
free.argtypes = [Ptr]

resize = _nexus.nx_fast_detector_resize
resize.restype = None
resize.argtypes = [Ptr, _c_int, _c_int]

detect = _nexus.nx_fast_detector_detect
detect.restype = None
detect.argtypes = [Ptr, image.Ptr]

detect_pyr = _nexus.nx_fast_detector_detect_pyr
detect_pyr.restype = None
detect_pyr.argtypes = [Ptr, image_pyr.Ptr]

adapt_threshold = _nexus.nx_fast_detector_adapt_threshold
adapt_threshold.restype = None
adapt_threshold.argtypes = [Ptr]
