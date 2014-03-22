import ctypes as _C

import pynexus as _NX
import pynexus.keypoint as _Key
import pynexus.keypointvector as _KV

__all__ = [ "NXKeypointVector" ]

class NXKeypointIterator(object):
    """Iterator for keypoint vectors"""
    def __init__(self, vector):
        self.__vector = vector
        self.__next_id = -1

    def __next__(self):
        self.__next_id += 1
        if self.__vector.size > self.__next_id:
            return self.__vector.key_ptr[self.__next_id]
        else:
            raise StopIteration

    def next(self):
        return self.__next__()

class NXKeypointVector(object):
    """Wrapper class for VIRG-Nexus keypoint vectors"""
    def __init__(self):
        self.__ptr = _KV.alloc()

    def __del__(self):
        _KV.free(self.__ptr)

    def __str__(self):
        return "<NXKeypointVector: {} / {} keypoints>".format(self.size, self.capacity)

    def __len__(self):
        return self.size

    def __getitem__(self, index):
        if (self.size > index) and (index >= 0):
            return self.key_ptr[index]
        else:
            raise IndexError('Keypoint index {} is out of bounds, {} keypoints present'.format(index, self.size))

    def __iter__(self):
        return NXKeypointIterator(self)

    @property
    def ptr(self):
        return self.__ptr;

    @property
    def size(self):
        return self.__ptr.contents.size

    @property
    def capacity(self):
        return self.__ptr.contents.capacity

    @property
    def key_ptr(self):
        return self.__ptr.contents.data

    def release(self):
        _KV.release(self.__ptr)

    def reserve(self, new_capacity):
        _KV.reserve(self.__ptr, new_capacity)

    def resize(self, new_size):
        _KV.resize(self.__ptr, new_size)

    def clear(self):
        _KV.clear(self.__ptr)

    def copy_of(self, key_vector):
        _KV.copy(self.__ptr, NXKeypointVector.ptr_of(key_vector))

    def push_back(self):
        return _KV.push_back(self.__ptr)

    def get_key(self, index):
        if index < self.size:
            return self.key_ptr[index]
        else:
            raise IndexError('Keypoint index {} is out of bounds, {} keypoints present'.format(index, self.size))

    def set_key(self, index, x, y, level, scale, sigma, score = 0.0, ori = 0.0, key_id = None, xs = None, ys = None):
        if key_id is None:
            key_id = index
        if xs is None:
            xs = x
        if ys is None:
            ys = y
        k = self.get_key(index)
        values = (x, y, xs, ys, level, scale, sigma, score, ori, key_id)
        k.x, k.y, k.xs, k.ys, k.level, k.scale, k.sigma, k.score, k.ori, k.id = values

    @staticmethod
    def ptr_of(key_vec):
        if isinstance(key_vec, NXKeypointVector):
            return key_vec.__ptr
        else:
            return key_vec

    @staticmethod
    def key_ptr_of(key_vec):
        if isinstance(key_vec, NXKeypointVector):
            return key_vec.key_ptr
        elif isinstance(key_vec, _KV.Struct):
            return key_vec.contents.data
        else:
            return key_vec

