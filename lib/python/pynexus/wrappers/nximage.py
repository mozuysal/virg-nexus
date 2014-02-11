import ctypes as _C

import pynexus as _NX
import pynexus.image as _Img

from pynexus.image_warp import BLACK as _NX_IMAGE_BG_MODE_BLACK
from pynexus.image_warp import REPEAT as _NX_IMAGE_BG_MODE_REPEAT
from pynexus.image_warp import MIRROR as _NX_IMAGE_BG_MODE_MIRROR
from pynexus.image_warp import NOISE as _NX_IMAGE_BG_MODE_NOISE
from pynexus.image_warp import WHITE as _NX_IMAGE_BG_MODE_WHITE

__all__ = [ "NXImage" ]

_POINTER = _C.POINTER
_c_ubyte = _C.c_ubyte
_c_int = _C.c_int
_c_size_t = _C.c_size_t
_c_float = _C.c_float
_c_char_p = _C.c_char_p

_buffer_from_memory = _C.pythonapi.PyBuffer_FromMemory
_buffer_from_memory.restype = _C.py_object

class NXImage(object):
    """Wrapper class for directly working with VIRG-Nexus images"""
    GRAYSCALE = _Img.GRAYSCALE
    RGBA = _Img.RGBA

    LOAD_AS_IS = _Img.LOAD_AS_IS
    LOAD_GRAYSCALE = _Img.LOAD_GRAYSCALE
    LOAD_RGBA = _Img.LOAD_RGBA

    BG_MODE_BLACK = _NX_IMAGE_BG_MODE_BLACK
    BG_MODE_REPEAT = _NX_IMAGE_BG_MODE_REPEAT
    BG_MODE_MIRROR = _NX_IMAGE_BG_MODE_MIRROR
    BG_MODE_NOISE = _NX_IMAGE_BG_MODE_NOISE
    BG_MODE_WHITE = _NX_IMAGE_BG_MODE_WHITE

    def __init__(self, w = None, h = None, image_type = _Img.GRAYSCALE):
        self.__ptr = _Img.alloc()
        if w and h:
            self.resize(w, h, image_type)

    def __del__(self):
        _Img.free(self.__ptr)

    def __len__(self):
        return self.width * self.height * self.n_channels

    def __str__(self):
        return "<NXImage: {}x{}x{}>".format(self.width, self.height, self.n_channels)

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
    def type(self):
        return self.__ptr.contents.type

    @property
    def row_stride(self):
        return self.__ptr.contents.row_stride

    @property
    def n_channels(self):
        return self.__ptr.contents.n_channels

    @property
    def data(self):
        return self.__ptr.contents.data

    def get_pixel(self, x, y):
        w = self.width
        h = self.height
        stride = self.row_stride
        if not NXImage.is_inside(self, x, y):
            raise IndexError('get_pixel: Coordinate {},{} is out of bounds for {}x{} image!'.format(x, y, self.width, self.height))
        else:
            return self.__ptr.contents.data[stride*y+x]

    def set_pixel(self, x, y, value):
        w = self.width
        h = self.height
        stride = self.row_stride
        if not NXImage.is_inside(self, x, y):
            raise IndexError('set_pixel: Coordinate {},{} is out of bounds for {}x{} image!'.format(x, y, self.width, self.height))

        value = _NX.uchar(value)
        self.__ptr.contents.data[stride*y+x] = value

    def sample_bilinear(self, xs, ys):
        xf = _c_float(xs)
        yf = _c_float(ys)
        return _Img.sample_bilinear(self.__ptr, xf, yf)

    def resize(self, w, h, image_type):
        _Img.resize(self.__ptr, w, h, NXImage.n_channels_by_type(image_type)*w, image_type)

    def release(self):
        _Img.release(self.__ptr)

    def save_pnm(self, filename):
        res = _Img.save_pnm(self.__ptr, filename)
        if res == _NX.OK:
            return True
        else:
            return False

    def load_pnm(self, filename, load_mode = None):
        if load_mode is None:
            load_mode = NX_IMAGE_LOAD_AS_IS
        res = _Img.load_pnm(self.__ptr, filename, load_mode)
        if res == _NX.OK:
            return True
        else:
            return False

    def copy(self):
        img = NXImage()
        _Img.copy(img.__ptr, self.__ptr)
        return img

    def copy_of(self, img):
        ptr = NXImage.ptr_of(img)
        _Img.copy(self.__ptr, ptr)

    def sub_rect(self, x, y, width, height):
        if not NXImage.is_rect_inside(self, x, y, width, height):
            raise IndexError('Sub-rectangle ({},{})-({}x{}) is out of image bounds ({}x{})!'.format(x, y, width, height,
                                                                                                    self.width, self.height))

        sub = NXImage()
        _Img.sub_rect_of(sub.ptr, self.ptr, x, y, width, height)
        return sub

    def sub_rect_of(self, img, x, y, width, height):
        ptr = NXImage.ptr_of(img)
        if not NXImage.is_rect_inside(ptr, x, y, width, height):
            raise IndexError('Sub-rectangle ({},{})-({}x{}) is out of image bounds ({}x{})!'.format(x, y, width, height,
                                                                                                    ptr.contents.width, ptr.contents.height))
        _Img.sub_rect_of(self.ptr, ptr, x, y, width, height)

    def swap(self, img):
        ptr = NXImage.ptr_of(img)
        _Img.swap(ptr, self.__ptr)

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

    def transform_affine(self, tm, bg_mode, res_img=None):
        ta = _c_float * 9;
        for i in xrange(9):
            ta[i] = tm[i]
        if res_img is None:
            res_img = NXImage()
        res_ptr = NXImage.ptr_of(res_img)
        _Img.transform_affine(res_ptr, self.__ptr, ta, bg_mode)
        return res_img

    def transform_affine_prm(self, scale, psi, theta, phi, bg_mode, res_img=None):
        if res_img is None:
            res_img = NXImage()
        res_ptr = NXImage.ptr_of(res_img)
        _Img.transform_affine_prm(res_ptr, self.__ptr, scale, psi, theta, phi, bg_mode)
        return res_img

    @staticmethod
    def n_channels_by_type(image_type):
        if image_type == _Img.GRAYSCALE:
            return 1
        elif image_type == _Img.RGBA:
            return 4
        else:
            return 0

    @staticmethod
    def as_buffer(img):
        ptr = NXImage.ptr_of(img)
        buff = _buffer_from_memory(ptr.contents.mem.contents.ptr,
                                   ptr.contents.mem.contents.size)
        return buff

    @staticmethod
    def ptr_of(img):
        if isinstance(img, NXImage):
            return img.__ptr
        else:
            return img

    @staticmethod
    def from_pnm(filename, load_mode = None):
        img = NXImage()
        r = img.load_pnm(filename, load_mode)
        if r:
            return img
        else:
            return None

    @staticmethod
    def is_inside(img, x, y):
        ptr = NXImage.ptr_of(img)
        w = ptr.contents.width
        h = ptr.contents.height
        if (x < 0) or (x >= w) or (y < 0) or (y >= h):
            return False

        return True

    @staticmethod
    def is_rect_inside(img, x, y, width, height):
        ptr = NXImage.ptr_of(img)
        w = ptr.contents.width
        h = ptr.contents.height
        if (x < 0) or (x + width > w) or (y < 0) or (y + height > h):
            return False

        return True

