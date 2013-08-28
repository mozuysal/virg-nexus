import ctypes as _C

import pynexus as _NX
import pynexus.image as _Img
import pynexus.image_pyr as _Pyr
from pynexus.wrappers.nximage import NXImage as _NXImg

__all__ = [ "NXImagePyr", "NXImagePyrInfo", "NXImagePyrLevel" ]

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

class NXImagePyrInfo(object):
    """Wrapper class for extracting information on VIRG-Nexus image pyramids"""
    def __init__(self, pyr):
        ptr = NXImagePyr.ptr_of(pyr)

        if ptr.contents.type == NX_IMAGE_PYR_FAST:
            inf = ptr.contents.info.fast
            self.type = NX_IMAGE_PYR_FAST
            self.width0 = inf.width0
            self.height0 = inf.height0
            self.sigma0 = inf.sigma0
        elif ptr.contents.type == NX_IMAGE_PYR_FINE:
            inf = ptr.contents.info.fine
            self.type = NX_IMAGE_PYR_FINE
            self.width0 = inf.width0
            self.height0 = inf.height0
            self.n_octaves = inf.n_octaves
            self.n_octave_steps = inf.n_octave_steps
            self.sigma0 = inf.sigma0
        elif ptr.contents.type == NX_IMAGE_PYR_SCALED:
            inf = ptr.contents.info.scaled
            self.type = NX_IMAGE_PYR_SCALED
            self.width0 = inf.width0
            self.height0 = inf.height0
            self.scale_factor = inf.scale_factor
            self.sigma0 = inf.sigma0

class NXImagePyrLevel(object):
    """Wrapper class for VIRG-Nexus image pyramid levels"""
    def __init__(self, pyr, level_id):
        self.__pyr_ptr = NXImagePyr.ptr_of(pyr)
        self.level_id = level_id

    def __str__(self):
        return self.img.contents.__str__() + ' scale = {}'.format(self.scale)

    def __len__(self):
        img = self.img
        if img:
            return img.__len__()
        return 0

    def __next__(self):
        next_level_id = self.level_id + 1
        if self.__pyr_ptr.contents.n_levels > next_level_id:
            self.level_id = next_level_id
            return self
        else:
            raise StopIteration

    def next(self):
        return self.__next__()

    def __safe_get_prop(self, prop_name):
        if self.__pyr_ptr.contents.n_levels > self.level_id:
            return getattr(self.__pyr_ptr.contents.levels[self.level_id], prop_name)
        else:
            raise IndexError('level index {} is no more valid for this pyramid!'.format(self.level_id))

    @property
    def img(self):
        return self.__safe_get_prop('img')

    @property
    def scale(self):
        return self.__safe_get_prop('scale')

    @property
    def sigma(self):
        return self.__safe_get_prop('sigma')


class NXImagePyr(object):
    """Wrapper class for directly working with VIRG-Nexus image pyramids"""

    FAST = _Pyr.FAST
    FINE = _Pyr.FINE
    SCALED = _Pyr.SCALED

    def __init__(self):
        self.__ptr = _Pyr.alloc()

    def __del__(self):
        _Pyr.free(self.__ptr)

    def __len__(self):
        return self.n_levels

    def __getitem__(self, index):
        return self.level(index)

    def __iter__(self):
        return NXImagePyrLevel(self, -1)

    def __str__(self):
        return "<NXImagePyr: {} Levels>".format(self.n_levels)

    @property
    def ptr(self):
        return self.__ptr

    @property
    def n_levels(self):
        return self.__ptr.contents.n_levels

    @property
    def levels(self):
        return self.__ptr.contents.levels

    @property
    def type(self):
        return self.__ptr.contents.type

    @property
    def info(self):
        return NXImagePyrInfo(self)

    def level(self, level_id):
        if (level_id >= 0) and (level_id < self.n_levels):
            return NXImagePyrLevel(self, level_id)
        else:
            raise IndexError('level: level index {} is out of bounds for {} level pyramid!'.format(level_id, self.n_levels))

    def resize_fast(self, width, height, n_levels, sigma0 = None):
        if sigma0 is None:
            sigma0 = 0.0
        _Pyr.resize_fast(self.__ptr, width, height, n_levels, sigma0)

    def resize_fine(self, width, height, n_octaves, n_octave_steps, sigma0 = None):
        if sigma0 is None:
            sigma0 = 0.0
        _Pyr.resize_fine(self.__ptr, width, height, n_octaves, n_octave_steps, sigma0)

    def resize_scaled(self, width, height, n_levels, scale_f, sigma0 = None):
        if sigma0 is None:
            sigma0 = 0.0
        _Pyr.resize_scaled(self.__ptr, width, height, n_levels, scale_f, sigma0)

    def update(self):
        _Pyr.update(self.__ptr)

    def compute(self, img):
        ptr = _NXImg.ptr_of(img)
        if not (ptr.contents.type == _NXImg.GRAYSCALE): raise TypeError('Image must be grayscale!')
        _Pyr.compute(self.__ptr, ptr)

    def compute_fast(self, img, n_levels, sigma0 = None):
        if not (_NXImg.ptr_of(img).contents.type == _NXImg.GRAYSCALE): raise TypeError('Image must be grayscale!')
        self.resize_fast(img.width, img.height, n_levels, sigma0)
        self.compute(img)

    def compute_fine(self, img, n_octaves, n_octave_steps, sigma0 = None):
        if not (_NXImg.ptr_of(img).contents.type == _NXImg.GRAYSCALE): raise TypeError('Image must be grayscale!')
        self.resize_fine(img.width, img.height, n_octaves, n_octave_steps, sigma0)
        self.compute(img)

    def compute_scaled(self, img, n_levels, scale_f, sigma0 = None):
        if not (_NXImg.ptr_of(img).contents.type == _NXImg.GRAYSCALE): raise TypeError('Image must be grayscale!')
        self.resize_scaled(img.width, img.height, n_levels, scale_f, sigma0)
        self.compute(img)

    def copy(self):
        pyr = NXImagePyr()
        _Pyr.copy(pyr.__ptr, self.__ptr)
        return pyr

    @staticmethod
    def ptr_of(pyr):
        if isinstance(pyr, NXImagePyr):
            return pyr.__ptr
        else:
            return pyr

