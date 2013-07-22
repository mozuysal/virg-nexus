from ctypes import *

nexus = cdll.LoadLibrary("../lib/libvirg-nexus.so")

(NX_FALSE, NX_TRUE) = (0, 1)

class NXMemBlock(Structure):
    _fields_ = [("ptr", POINTER(c_ubyte)),
                ("size", c_size_t),
                ("capacity", c_size_t),
                ("own_memory", c_int)]

    def __str__(self):
        return "<NXMemBlock: {} / {}>".format(self.size, self.capacity);

NXMemBlockPtr = POINTER(NXMemBlock)

nx_mem_block_alloc = nexus.nx_mem_block_alloc
nx_mem_block_alloc.argtypes = []
nx_mem_block_alloc.restype = NXMemBlockPtr

nx_mem_block_new = nexus.nx_mem_block_new
nx_mem_block_new.argtypes = [c_int, c_int]
nx_mem_block_new.restype = NXMemBlockPtr

nx_mem_block_free = nexus.nx_mem_block_free
nx_mem_block_free.argtypes = [NXMemBlockPtr]
nx_mem_block_free.restype = None

nx_mem_block_resize = nexus.nx_mem_block_resize
nx_mem_block_resize.argtypes = [NXMemBlockPtr, c_size_t]
nx_mem_block_resize.restype = None

nx_mem_block_release = nexus.nx_mem_block_release
nx_mem_block_release.argtypes = [NXMemBlockPtr]
nx_mem_block_release.restype = None

nx_mem_block_wrap = nexus.nx_mem_block_wrap
nx_mem_block_wrap.argtypes = [NXMemBlockPtr, POINTER(c_ubyte), c_size_t, c_int]
nx_mem_block_wrap.restype = None

nx_mem_block_swap = nexus.nx_mem_block_swap
nx_mem_block_swap.argtypes = [NXMemBlockPtr, NXMemBlockPtr]
nx_mem_block_swap.restype = None

nx_mem_block_copy0 = nexus.nx_mem_block_copy0
nx_mem_block_copy0.argtypes = [NXMemBlockPtr]
nx_mem_block_copy0.restype = NXMemBlockPtr

nx_mem_block_copy = nexus.nx_mem_block_copy
nx_mem_block_copy.argtypes = [NXMemBlockPtr, NXMemBlockPtr]
nx_mem_block_copy.restype = None

nx_mem_block_set_zero = nexus.nx_mem_block_set_zero
nx_mem_block_set_zero.argtypes = [NXMemBlockPtr]
nx_mem_block_set_zero.restype = None

(NX_IMAGE_GRAYSCALE, NX_IMAGE_RGBA) = (0, 1)
(NX_IMAGE_LOAD_AS_IS, NX_IMAGE_LOAD_GRAYSCALE, NX_IMAGE_LOAD_RGBA) = (-1, 0, 1)

class NXImage(Structure):
    _fields_ = [("width", c_int),
                ("height", c_int),
                ("type", c_int),
                ("n_channels", c_int),
                ("mem", NXMemBlockPtr),
                ("row_stride", c_int)]

    def __str__(self):
        return "<NXImage: {}x{}x{}>".format(self.width, self.height, self.n_channels);

NXImagePtr = POINTER(NXImage)

nx_image_alloc = nexus.nx_image_alloc
nx_image_alloc.argtypes = []
nx_image_alloc.restype = NXImagePtr

nx_image_new = nexus.nx_image_new
nx_image_new.argtypes = [c_int, c_int, c_int]
nx_image_new.restype = NXImagePtr

nx_image_new_gray = nexus.nx_image_new_gray
nx_image_new_gray.argtypes = [c_int, c_int]
nx_image_new_gray.restype = NXImagePtr

nx_image_new_rgba = nexus.nx_image_new_rgba
nx_image_new_rgba.argtypes = [c_int, c_int]
nx_image_new_rgba.restype = NXImagePtr

nx_image_free = nexus.nx_image_free
nx_image_free.argtypes = [NXImagePtr]
nx_image_free.restype = None

nx_image_resize = nexus.nx_image_resize
nx_image_resize.argtypes = [NXImagePtr, c_int, c_int, c_int, c_int]
nx_image_resize.restype = None

nx_image_release = nexus.nx_image_release
nx_image_release.argtypes = [NXImagePtr]
nx_image_release.restype = None

nx_image_copy0 = nexus.nx_image_copy0
nx_image_copy0.argtypes = [NXImagePtr]
nx_image_copy0.restype = NXImagePtr

nx_image_copy = nexus.nx_image_copy
nx_image_copy.argtypes = [NXImagePtr, NXImagePtr]
nx_image_copy.restype = None

nx_image_set_zero = nexus.nx_image_set_zero
nx_image_set_zero.argtypes = [NXImagePtr]
nx_image_set_zero.restype = None

nx_image_wrap = nexus.nx_image_wrap
nx_image_wrap.argtypes = [NXImagePtr, POINTER(c_ubyte), c_int, c_int, c_int, c_int, c_int]
nx_image_wrap.restype = None

nx_image_swap = nexus.nx_image_swap
nx_image_swap.argtypes = [NXImagePtr, NXImagePtr]
nx_image_swap.restype = None

nx_image_convert = nexus.nx_image_convert
nx_image_convert.argtypes = [NXImagePtr, NXImagePtr]
nx_image_convert.restype = None

nx_image_scale = nexus.nx_image_scale
nx_image_scale.argtypes = [NXImagePtr, NXImagePtr, c_float]
nx_image_scale.restype = None

nx_image_downsample = nexus.nx_image_downsample
nx_image_downsample.argtypes = [NXImagePtr, NXImagePtr]
nx_image_downsample.restype = None

nx_image_filter_buffer_alloc = nexus.nx_image_filter_buffer_alloc
nx_image_filter_buffer_alloc.argtypes = [c_int, c_int, c_float, c_float]
nx_image_filter_buffer_alloc.restype = POINTER(c_ubyte)

nx_image_smooth_si = nexus.nx_image_smooth_si
nx_image_smooth_si.argtypes = [NXImagePtr, NXImagePtr, c_float, c_float, POINTER(c_ubyte)]
nx_image_smooth_si.restype = None

nx_image_smooth_s = nexus.nx_image_smooth_s
nx_image_smooth_s.argtypes = [NXImagePtr, NXImagePtr, c_float, c_float, POINTER(c_ubyte)]
nx_image_smooth_s.restype = None

nx_image_xsave_pnm = nexus.nx_image_xsave_pnm
nx_image_xsave_pnm.argtypes = [NXImagePtr, c_char_p]
nx_image_xsave_pnm.restype = None

nx_image_xload_pnm = nexus.nx_image_xload_pnm
nx_image_xload_pnm.argtypes = [NXImagePtr, c_char_p, c_int]
nx_image_xload_pnm.restype = None
