import ctypes as _C

import __init__ as _NX

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_int = _C.c_int
_c_float = _C.c_float
_c_long = _C.c_long

class Struct(_C.Structure):
    _fields_ = [('x', _c_int),
                ('y', _c_int),
                ('xs', _c_float),
                ('ys', _c_float),
                ('level', _c_int),
                ('scale', _c_float),
                ('sigma', _c_float),
                ('score', _c_float),
                ('ori', _c_float),
                ('id', _c_long)]

    def __str__(self):
        s = "<pynexus.keypoint.Struct: ({}, {}) scale: {}, sigma: {}, ori: {}, score: {}>"
        return s.format(self.x, self.y, self.scale, self.sigma, self.ori, self.score)

    @staticmethod
    def clone_keys(n_keys, keys):
        if n_keys <= 0:
            return None

        key_array = Struct * n_keys
        cpy = key_array()
        _C.memmove(cpy, keys, n_keys * _C.sizeof(Struct))

        return cpy


Ptr = _POINTER(Struct)
