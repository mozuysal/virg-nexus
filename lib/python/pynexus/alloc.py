import ctypes as _C

import __init__ as _NX

_nexus = _NX._nexus

_c_size_t = _C.c_size_t
_c_void_p = _C.c_void_p

malloc = _nexus.nx_malloc
malloc.restype = _c_void_p
malloc.argtypes = [_c_size_t]

calloc = _nexus.nx_calloc
calloc.restype = _c_void_p
calloc.argtypes = [_c_size_t, _c_size_t]

fmalloc = _nexus.nx_fmalloc
fmalloc.restype = _c_void_p
fmalloc.argtypes = [_c_size_t]

fcalloc = _nexus.nx_fcalloc
fcalloc.restype = _c_void_p
fcalloc.argtypes = [_c_size_t, _c_size_t]

frealloc = _nexus.nx_frealloc
frealloc.restype = _c_void_p
frealloc.argtypes = [_c_void_p, _c_size_t]

aligned_alloc = _nexus.nx_aligned_alloc
aligned_alloc.restype = _c_void_p
aligned_alloc.argtypes = [_c_size_t, _c_size_t]

faligned_alloc = _nexus.nx_faligned_alloc
faligned_alloc.restype = _c_void_p
faligned_alloc.argtypes = [_c_size_t, _c_size_t]

free = _nexus.nx_free
free.restype = None
free.argtypes = [_c_void_p]
