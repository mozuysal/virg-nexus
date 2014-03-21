import ctypes as _C

import pynexus as _NX
import pynexus.image as _Img
import pynexus.image_pyr as _Pyr
from pynexus.wrappers.nximage import NXImage as _NXImg

__all__ = [ "NXImagePyr", "NXImagePyrLevel" ]

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

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

    def level(self, level_id):
        if (level_id >= 0) and (level_id < self.n_levels):
            return NXImagePyrLevel(self, level_id)
        else:
            raise IndexError('level: level index {} is out of bounds for {} level pyramid!'.format(level_id, self.n_levels))

    @staticmethod
    def ptr_of(pyr):
        if isinstance(pyr, NXImagePyr):
            return pyr.__ptr
        else:
            return pyr

