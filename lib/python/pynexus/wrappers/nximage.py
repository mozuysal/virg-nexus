import ctypes as _C

import pynexus as _NX
import pynexus.image as _Img

__all__ = [ "NXImage", "NX_IMAGE_GRAYSCALE", "NX_IMAGE_RGBA", "NX_IMAGE_LOAD_AS_IS", "NX_IMAGE_LOAD_GRAYSCALE", "NX_IMAGE_LOAD_RGBA" ]

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

_buffer_from_memory = _C.pythonapi.PyBuffer_FromMemory
_buffer_from_memory.restype = _C.py_object

NX_IMAGE_GRAYSCALE = _Img.GRAYSCALE
NX_IMAGE_RGBA = _Img.RGBA

NX_IMAGE_LOAD_AS_IS = _Img.LOAD_AS_IS
NX_IMAGE_LOAD_GRAYSCALE = _Img.LOAD_GRAYSCALE
NX_IMAGE_LOAD_RGBA = _Img.LOAD_RGBA

class NXImage(object):
    """Wrapper class for directly working with VIRG-Nexus images"""
    def __init__(self, w = None, h = None, image_type = _Img.GRAYSCALE):
        self.__ptr = _Img.alloc()
        if w and h:
            self.resize(w, h, image_type)

    def __del__(self):
        _Img.free(self.__ptr)

    def __str__(self):
        return "<NXImage: {}x{}x{}>".format(self.width, self.height, self.n_channels);

    @property
    def ptr(self):
        return self.__ptr;

    @property
    def width(self):
        return self.__ptr.contents.width;

    @property
    def height(self):
        return self.__ptr.contents.height;

    @property
    def type(self):
        return self.__ptr.contents.type;

    @property
    def row_stride(self):
        return self.__ptr.contents.row_stride;

    @property
    def n_channels(self):
        return self.__ptr.contents.n_channels;

    def as_buffer(self):
        buff = _buffer_from_memory(self.__ptr.contents.mem.contents.ptr,
                                   self.__ptr.contents.mem.contents.size)
        return buff

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

        value = _NX.uchar(value)
        self.__ptr.contents.data[stride*y+x] = value

    def resize(self, w, h, image_type):
        _Img.resize(self.__ptr, w, h, NXImage.n_channels_by_type(image_type)*w, image_type)

    def save_pnm(self, filename):
        res = _Img.save_pnm(self.__ptr, filename)
        if res == _NX.OK:
            return True
        else:
            return False

    def load_pnm(self, filename, load_mode):
        res = _Img.load_pnm(self.__ptr, filename, load_mode)
        if res == _NX.OK:
            return True
        else:
            return False

    def copy(self):
        img = NXImage()
        _Img.copy(img.__ptr, self.__ptr)
        return img

    def swap(self, img):
        _Img.swap(img.__ptr, self.__ptr)

    def set_zero(self):
        _Img.set_zero(self.__ptr)

    def scale(self, scale_factor):
        img = NXImage()
        sf = _c_float(scale_factor)
        _Img.scale(img.__ptr, self.__ptr, sf)
        return img

    def downsample(self):
        img = NXImage()
        _Img.downsample(img.__ptr, self.__ptr)
        return img

    def downsample_aa_x(self):
        img = NXImage()
        _Img.downsample_aa_x(img.__ptr, self.__ptr)
        return img

    def downsample_aa_y(self):
        img = NXImage()
        _Img.downsample_aa_y(img.__ptr, self.__ptr)
        return img

    def smooth(self, sigma_x, sigma_y):
        img = NXImage()
        sx = _c_float(sigma_x)
        sy = _c_float(sigma_y)
        buff = _POINTER(_NX.uchar)() # NULL pointer
        _Img.smooth_s(img.__ptr, self.__ptr, sx, sy, buff)
        return img

    def smooth_self(self, sigma_x, sigma_y):
        sx = _c_float(sigma_x)
        sy = _c_float(sigma_y)
        buff = _POINTER(_NX.uchar)() # NULL pointer
        _Img.smooth_s(self.__ptr, self.__ptr, sx, sy, buff)

    def convert(self, image_type):
        img = NXImage(1, 1, image_type)
        _Img.convert(img.__ptr, self.__ptr)
        return img

    @staticmethod
    def n_channels_by_type(image_type):
        if image_type == _Img.GRAYSCALE:
            return 1
        elif image_type == _Img.RGBA:
            return 4
        else:
            return 0
