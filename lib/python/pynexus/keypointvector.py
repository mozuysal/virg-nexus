import ctypes as _C

import __init__ as _NX

import keypoint
import memblock

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

class Struct(_C.Structure):
    _fields_ = [('size', _c_size_t),
                ('capacity', _c_size_t),
                ('data', keypoint.Ptr),
                ('mem', memblock.Ptr)]

    def __str__(self):
        return "<NXKeypointVector: {} / {}>".format(self.size, self.capacity);

Ptr = _POINTER(Struct)

alloc = _nexus.nx_keypoint_vector_alloc
alloc.restype = Ptr
alloc.argtypes = []

new = _nexus.nx_keypoint_vector_new
new.restype = Ptr
new.argtypes = [_c_int]

free = _nexus.nx_keypoint_vector_free
free.restype = None
free.argtypes = [Ptr]

release = _nexus.nx_keypoint_vector_release
release.restype = None
release.argtypes = [Ptr]

reserve = _nexus.nx_keypoint_vector_reserve
reserve.restype = None
reserve.argtypes = [Ptr, _c_size_t]

resize = _nexus.nx_keypoint_vector_resize
resize.restype = None
resize.argtypes = [Ptr, _c_size_t]

clear = _nexus.nx_keypoint_vector_clear
clear.restype = None
clear.argtypes = [Ptr]

wrap = _nexus.nx_keypoint_vector_wrap
wrap.restype = None
wrap.argtypes = [Ptr, keypoint.Ptr, _c_size_t, _c_size_t, _NX.Bool]

copy0 = _nexus.nx_keypoint_vector_copy0
copy0.restype = Ptr
copy0.argtypes = [Ptr]

copy = _nexus.nx_keypoint_vector_copy
copy.restype = None
copy.argtypes = [Ptr, Ptr]

push_back = _nexus.nx_keypoint_vector_push_back
push_back.restype = keypoint.Ptr
push_back.argtypes = [Ptr]


