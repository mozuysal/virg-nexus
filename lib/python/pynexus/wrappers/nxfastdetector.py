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

    def __len__(self):
        return self.n_keys

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
    def compute_ori(self):
        return self.__ptr.contents.compute_ori

    @property
    def n_keys(self):
        return self.__ptr.contents.n_keys

    @property
    def threshold(self):
        return self.__ptr.contents.threshold

    @threshold.setter
    def threshold(self, value):
        self.__ptr.contents.threshold = value

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
        return self.n_keys

    def detect_pyr(self, pyr, max_n_keys = None):
        if max_n_keys is not None:
            self.resize(max_n_keys)
        pyr_ptr = _NXPyr.ptr_of(pyr)
        _FD.detect_pyr(self.__ptr, pyr_ptr)
        return self.n_keys

    def set_ori_param(self, compute_p, radius):
        if compute_p:
            cp = _NX.TRUE
        else:
            cp = _NX.FALSE
        _FD.set_ori_param(self.__ptr, cp, radius);

    def adapt_threshold(self):
        _FD.adapt_threshold(self.__ptr)
        return self.threshold

    @staticmethod
    def ptr_of(detector):
        if isinstance(detector, NXFastDetector):
            return detector.__ptr
        else:
            return detector

