import ctypes as _C

import pynexus as _NX
import pynexus.brief_extractor as _BE

from pynexus.wrappers.nximagepyr import NXImagePyr

__all__ = [ "NXBriefExtractor" ]

_POINTER = _C.POINTER
_c_int = _C.c_int
_c_float = _C.c_float

class NXBriefExtractor(object):
    """Wrapper class for descriptor computation with VIRG-Nexus BRIEF extractor"""
    def __init__(self, n_octets = None, radius = None):
        self.__ptr = _BE.alloc()
        if n_octets and radius:
            self.resize(n_octets, radius)

    def __del__(self):
        _BE.free(self.__ptr)

    def __len__(self):
        return self.n_octets

    def __str__(self):
        return "<NXBriefExtractor: {} bits at radius {}>".format(self.n_octets * 8, self.radius);

    @property
    def ptr(self):
        return self.__ptr

    @property
    def n_octets(self):
        return self.__ptr.contents.n_octets

    @property
    def radius(self):
        return self.__ptr.contents.radius

    @property
    def offsets(self):
        return self.__ptr.contents.offsets

    @property
    def offset_limits(self):
        return self.__ptr.contents.offset_limits

    @property
    def pyr_level_offset(self):
        return self.__ptr.contents.pyr_level_offset

    @pyr_level_offset.setter
    def pyr_level_offset(self, value):
        self.__ptr.contents.pyr_level_offset = value

    def set_offsets(self, offsets):
        i = 0
        i_limit = self.n_octets*8*4 - 1
        for off in offsets:
            if i > i_limit:
                raise IndexError
            self.offsets[i] = offsets[i]
            i = i + 1
        self.update_limits()

    def resize(self, n_octets, radius):
        _BE.resize(self.__ptr, n_octets, radius)
        self.desc_type = _NX.uchar * self.n_octets

    def randomize(self):
        _BE.randomize(self.__ptr)

    def update_limits(self):
        _BE.update_limits(self.__ptr)

    def check_point_pyr(self, pyr, x, y, level):
        pyr_ptr = NXImagePyr.ptr_of(pyr)
        res = _BE.check_point_pyr(self.__ptr, pyr_ptr, x, y, level)
        if res == _NX.TRUE:
            return True
        else:
            return False

    def compute_pyr(self, pyr, x, y, level, desc):
        pyr_ptr = NXImagePyr.ptr_of(pyr)
        if self.check_point_pyr(pyr_ptr, x, y, level):
            res = _BE.compute_pyr(self.__ptr, pyr_ptr, x, y, level, desc)
            return True
        else:
            return False

    def new_desc_buffer(self):
        return self.desc_type()

    def compute_pyr_for_keys(self, pyr, n_keys, keys):
        desc_map = {}
        for key in keys:
            desc = self.new_desc_buffer()
            if self.compute_pyr(pyr, key.x, key.y, key.level, desc):
                desc_map[key.id] = desc
        return desc_map

    @staticmethod
    def ptr_of(be):
        if isinstance(be, NXBriefExtractor):
            return be.__ptr
        else:
            return be

    @staticmethod
    def descriptor_distance(n_octets, desc0, desc1):
        return _BE.descriptor_distance(n_octets, desc0, desc1)
