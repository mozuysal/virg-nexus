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
               ('scale', _c_float),
               ('sigma', _c_float),
               ('score', _c_float),
               ('ori', _c_float),
               ('id', _c_long)]

    def __str__(self):
        return "<pynexus.keypoint.Struct: ({}, {}) scale: {}, sigma: {}, score: {}>".format(self.x, self.y, self.scale, self.sigma, self.score);

Ptr = _POINTER(Struct)
