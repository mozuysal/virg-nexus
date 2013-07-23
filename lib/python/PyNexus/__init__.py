import ctypes as _C

_nexus = _C.cdll.LoadLibrary("../lib/libvirg-nexus.so")

Bool = _C.c_int
Result = _C.c_int
uchar = _C.c_ubyte

(FALSE, TRUE) = (0, 1)
EXIT_FATAL = -99
OK = 0
FAIL = -1
SIMD_ALIGNMENT = 32
LOG_TAG = "VIRG-NEXUS"

__all__ = ["MemBlock", "Image", "Bool", "Result", "uchar", "FALSE", "TRUE", "EXIT_FATAL", "OK", "FAIL", "SIMD_ALINGMENT", "LOG_TAG"]
