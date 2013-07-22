import ctypes as _C

from __init__ import _nexus

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

class Struct(_C.Structure):
    _fields_ = [("ptr", _POINTER(_c_ubyte)),
                ("size", _c_size_t),
                ("capacity", _c_size_t),
                ("own_memory", _c_int)]

    def __str__(self):
        return "<NXMemBlock: {} / {}>".format(self.size, self.capacity);

Ptr = _POINTER(Struct)

alloc = _nexus.nx_mem_block_alloc
alloc.argtypes = []
alloc.restype = Ptr

new = _nexus.nx_mem_block_new
new.argtypes = [_c_int, _c_int]
new.restype = Ptr

free = _nexus.nx_mem_block_free
free.argtypes = [Ptr]
free.restype = None

resize = _nexus.nx_mem_block_resize
resize.argtypes = [Ptr, _c_size_t]
resize.restype = None

release = _nexus.nx_mem_block_release
release.argtypes = [Ptr]
release.restype = None

wrap = _nexus.nx_mem_block_wrap
wrap.argtypes = [Ptr, _POINTER(_c_ubyte), _c_size_t, _c_int]
wrap.restype = None

swap = _nexus.nx_mem_block_swap
swap.argtypes = [Ptr, Ptr]
swap.restype = None

copy0 = _nexus.nx_mem_block_copy0
copy0.argtypes = [Ptr]
copy0.restype = Ptr

copy = _nexus.nx_mem_block_copy
copy.argtypes = [Ptr, Ptr]
copy.restype = None

set_zero = _nexus.nx_mem_block_set_zero
set_zero.argtypes = [Ptr]
set_zero.restype = None

