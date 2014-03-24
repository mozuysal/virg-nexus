import ctypes as _C
from sys import platform as _platform

if _platform == "linux" or _platform == "linux2":
    _nexus_lib = 'libvirg-nexus.so'
elif _platform == "darwin":
    _nexus_lib = 'libvirg-nexus.dylib'
elif _platform == "win32":
    _nexus_lib = 'virg-nexus.dll'

_nexus = _C.cdll.LoadLibrary(_nexus_lib)

Bool = _C.c_int
Result = _C.c_int
uchar = _C.c_ubyte

(FALSE, TRUE) = (0, 1)
EXIT_FATAL = -99
OK = 0
FAIL = -1
SIMD_ALIGNMENT = 32
LOG_TAG = "VIRG-NEXUS"

__all__ = ["Bool", "Result", "uchar", "FALSE", "TRUE", "EXIT_FATAL", "OK", "FAIL", "SIMD_ALINGMENT", "LOG_TAG"]
