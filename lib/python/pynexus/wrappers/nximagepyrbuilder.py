import ctypes as _C

import pynexus as _NX
import pynexus.image as _Img
import pynexus.image_pyr as _Pyr
import pynexus.image_pyr_builder as _Builder
from pynexus.wrappers.nximage import NXImage as _NXImg
from pynexus.wrappers.nximagepyr import NXImagePyr as _NXPyr

__all__ = [ "NXImagePyrBuilder" ]

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

class NXImagePyrBuilder(object):
    """Wrapper class for directly working with VIRG-Nexus image pyramid builders"""

    TYPE_NONE = _Builder.BUILDER_NONE
    TYPE_FAST = _Builder.BUILDER_FAST
    TYPE_FINE = _Builder.BUILDER_FINE
    TYPE_SCALED = _Builder.BUILDER_SCALED

    def __init__(self):
        self.__ptr = _Builder.alloc()

    def __del__(self):
        _Builder.free(self.__ptr)

    def __str__(self):
        return "<NXImagePyrBuilder: Type {}>".format(_Builder.TYPE_STRINGS[self.type])

    @property
    def ptr(self):
        return self.__ptr

    @property
    def type(self):
        return self.__ptr.contents.type

    @property
    def pyr_n_levels(self):
        return self.__ptr.contents.pyr_info.n_levels

    @property
    def pyr_n_octaves(self):
        return self.__ptr.contents.pyr_info.n_octaves

    @property
    def pyr_n_octave_steps(self):
        return self.__ptr.contents.pyr_info.n_octave_steps

    @property
    def pyr_scale_factor(self):
        return self.__ptr.contents.pyr_info.scale_factor

    @property
    def pyr_sigma0(self):
        return self.__ptr.contents.pyr_info.sigma0

    def set_fast(self, n_levels, sigma0 = None):
        if sigma0 is None:
            sigma0 = 0.0
        _Builder.set_fast(self.__ptr, n_levels, sigma0)

    def set_fine(self, n_octaves, n_octave_steps, sigma0 = None):
        if sigma0 is None:
            sigma0 = 0.0
        _Builder.set_fine(self.__ptr, n_octaves, n_octave_steps, sigma0)

    def set_scaled(self, n_levels, scale_factor, sigma0 = None):
        if sigma0 is None:
            sigma0 = 0.0
        _Builder.set_scaled(self.__ptr, n_levels, scale_factor, sigma0)

    def build(self, pyr, image):
        _Builder.build(self.__ptr, _NXPyr.ptr_of(pyr), _NXImage.ptr_of(image))

    def update(self, pyr):
        _Builder.build(self.__ptr, _NXPyr.ptr_of(pyr))

    def init_levels(self, pyr, width0, height0):
        _Builder.init_levels(self.__ptr, _NXPyr.ptr_of(pyr), width0, height0)

    def build_new(self, image):
        pyr = _NXPyr()
        self.build(pyr, image)
        return pyr

    @staticmethod
    def ptr_of(builder):
        if isinstance(builder, NXImagePyrBuilder):
            return builder.__ptr
        else:
            return builder

