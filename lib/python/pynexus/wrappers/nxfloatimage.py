import ctypes as _C

import pynexus as _NX
import pynexus.image as _Img
import pynexus.float_image as _FImg

from pynexus.wrappers.nximage import *

__all__ = [ "NXFloatImage" ]

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

class NXFloatImage(object):
    """Wrapper class for directly working with VIRG-Nexus floating point images"""
    def __init__(self, w = None, h = None, n_ch = 1):
        self.__ptr = _FImg.alloc()
        if w and h:
            self.resize(w, h, n_ch)

    def __del__(self):
        _FImg.free(self.__ptr)

    def __str__(self):
        return "<NXFloatImage: {}x{}x{}>".format(self.width, self.height, self.n_channels)

    @property
    def ptr(self):
        return self.__ptr

    @property
    def width(self):
        return self.__ptr.contents.width

    @property
    def height(self):
        return self.__ptr.contents.height

    @property
    def row_stride(self):
        return self.__ptr.contents.row_stride

    @property
    def n_channels(self):
        return self.__ptr.contents.n_channels

    def get_pixel(self, x, y):
        w = self.width
        h = self.height
        stride = self.row_stride
        if (x < 0) or (x >= w) or (y < 0) or (y >= h):
            return None
        else:
            return self.__ptr.contents.data[stride*y+x]

    def set_pixel(self, x, y, value):
        w = self.width
        h = self.height
        stride = self.row_stride
        if (x < 0) or (x >= w) or (y < 0) or (y >= h):
            return

        value = _c_float(value)
        self.__ptr.contents.data[stride*y+x] = value

    def resize(self, w, h, n_ch):
        _FImg.resize(self.__ptr, w, h, n_ch, 0)

    def save_raw(self, filename):
        res = _FImg.save_raw(self.__ptr, filename)
        if res == _NX.OK:
            return True
        else:
            return False

    def load_raw(self, filename):
        res = _FImg.load_raw(self.__ptr, filename)
        if res == _NX.OK:
            return True
        else:
            return False

    def copy(self):
        fimg = NXFloatImage()
        _FImg.copy(fimg.__ptr, self.__ptr)
        return fimg

    def copy_of(self, fimg):
        ptr = NXFloatImage.ptr_of(fimg)
        _FImg.copy(self.__ptr, ptr)

    def swap(self, fimg):
        ptr = NXFloatImage.ptr_of(fimg)
        _FImg.swap(ptr, self.__ptr)

    def set_zero(self):
        _FImg.set_zero(self.__ptr)

    def smooth(self, sigma_x, sigma_y):
        fimg = NXFloatImage()
        sx = _c_float(sigma_x)
        sy = _c_float(sigma_y)
        buff = _POINTER(_c_float)() # NULL pointer
        _FImg.smooth_s(fimg.__ptr, self.__ptr, sx, sy, buff)
        return fimg

    def smooth_self(self, sigma_x, sigma_y):
        sx = _c_float(sigma_x)
        sy = _c_float(sigma_y)
        buff = _POINTER(_c_float)() # NULL pointer
        _FImg.smooth_s(self.__ptr, self.__ptr, sx, sy, buff)

    def from_image(self, img):
        ptr = NXImage.ptr_of(img)
        _FImg.from_uchar(self.__ptr, ptr)

    def to_image(self):
        img = NXImage()
        _FImg.to_uchar(self.__ptr, img.ptr)
        return img

    def spline_coeff_of(self, img):
        ptr = NXImage.ptr_of(img)
        _FImg.spline_coeff_of(self.__ptr, ptr)

    @staticmethod
    def ptr_of(fimg):
        if isinstance(fimg, NXFloatImage):
            return fimg.__ptr
        else:
            return fimg

    @staticmethod
    def from_raw(filename):
        fimg = NXFloatImage()
        r = fimg.load_raw(filename)
        if r:
            return fimg
        else:
            return None

