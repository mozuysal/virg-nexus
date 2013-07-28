import ctypes as _C

import pynexus as _NX
import pynexus.keypoint as _Key
import pynexus.fast_detector as _FD

from pynexus.wrappers.nximage import NXImage as _NXImg
from pynexus.wrappers.nximagepyr import NXImagePyr as _NXPyr

__all__ = [ "NXFastDetector" ]

class NXFastDetector(object):
    """Wrapper class for VIRG-Nexus FAST keypoint extraction"""
    def __init__(self, max_n_keys = None, initial_work_size = None):
        if max_n_keys is not None:
            if initial_work_size is None:
                initial_work_size = 0 # use default work size

            self.__ptr = _FD.new(max_n_keys, initial_work_size)
        else:
            self.__ptr = _FD.alloc()

    def __del__(self):
        _FD.free(self.__ptr)

    def __str__(self):
        return "<NXFastDetector: {} of {} maximum, threshold: {}>".format(self.n_keys, self.max_n_keys, self.threshold)

    @property
    def ptr(self):
        return self.__ptr;

    @property
    def max_n_keys(self):
        return self.__ptr.contents.max_n_keys

    @property
    def n_keys(self):
        return self.__ptr.contents.n_keys

    @property
    def threshold(self):
        return self.__ptr.contents.threshold

    @threshold.setter
    def threshold(self, value):
        return self.__ptr.contents.threshold = value

    @property
    def keys(self):
        return self.__ptr.contents.keys

    def resize(self, max_n_keys, initial_work_size = None):
        if initial_work_size is None:
            initial_work_size = 0
        _FD.resize(self.__ptr, max_n_keys, initial_work_size)

    def detect(self, img, max_n_keys = None):
        if max_n_keys is not None:
            self.resize(max_n_keys)
        img_ptr = _NXImg.ptr_of(img)
        _FD.detect(self.__ptr, img_ptr)

    def detect_pyr(self, pyr, max_n_keys = None):
        if max_n_keys is not None:
            self.resize(max_n_keys)
        pyr_ptr = _NXPyr.ptr_of(pyr)
        _FD.detect(self.__ptr, pyr_ptr)

    def adapt_threshold(self):
        _FD.adapt_threshold(self.__ptr)

    @staticmethod
    def ptr_of(detector):
        if isinstance(detector, NXFastDetector):
            return detector.__ptr
        else:
            return detector

