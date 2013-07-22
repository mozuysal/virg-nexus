import ctypes as _C

__all__ = ["MemBlock", "Image"]

_nexus = _C.cdll.LoadLibrary("../lib/libvirg-nexus.so")

(FALSE, TRUE) = (0, 1)
