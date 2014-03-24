import ctypes as _C

import pynexus as _NX
import pynexus.keypoint as _Key
import pynexus.keypointvector as _KeyVector
import pynexus.fast_detector as _FD

from pynexus.wrappers.nximage import NXImage as _NXImg
from pynexus.wrappers.nximagepyr import NXImagePyr as _NXPyr

__all__ = [ "NXFastDetector" ]

class NXFastDetector(object):
    """Wrapper class for VIRG-Nexus FAST keypoint extraction"""
    def __init__(self):
        self.__ptr = _FD.alloc()

    def __del__(self):
        _FD.free(self.__ptr)

    def __str__(self):
        return "<NXFastDetector: threshold: {}>".format(self.threshold)

    @property
    def ptr(self):
        return self.__ptr;

    @property
    def compute_ori(self):
        return self.__ptr.contents.compute_ori

    @property
    def threshold(self):
        return self.__ptr.contents.threshold

    @threshold.setter
    def threshold(self, value):
        self.__ptr.contents.threshold = value

    def detect(self, keys, img, max_n_keys = None):
        img_ptr = _NXImg.ptr_of(img)
        if max_n_keys is None:
            max_n_keys = len(keys)
        n_keys = _FD.detect(self.__ptr, max_n_keys, _KeyVector.key_ptr_of(keys), img_ptr)
        return n_keys

    def detect_pyr(self, keys, pyr, max_n_keys = None, n_pyr_key_levels = -1):
        pyr_ptr = _NXPyr.ptr_of(pyr)
        if max_n_keys is None:
            max_n_keys = len(keys)
        n_keys = _FD.detect_pyr(self.__ptr, max_n_keys, _KeyVector.key_ptr_of(keys), pyr_ptr, n_pyr_key_levels)
        return n_keys

    def set_ori_param(self, compute_p, radius):
        if compute_p:
            cp = _NX.TRUE
        else:
            cp = _NX.FALSE
        _FD.set_ori_param(self.__ptr, cp, radius);

    def adapt_threshold(self, n_keys, max_n_keys):
        _FD.adapt_threshold(self.__ptr, n_keys, max_n_keys)
        return self.threshold

    @staticmethod
    def ptr_of(detector):
        if isinstance(detector, NXFastDetector):
            return detector.__ptr
        else:
            return detector

