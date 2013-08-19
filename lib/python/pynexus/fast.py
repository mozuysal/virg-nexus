import ctypes as _C

import __init__ as _NX

import image
import image_pyr
import keypoint

_nexus = _NX._nexus

_POINTER = _C.POINTER
_c_int = _C.c_int

detect_keypoints = _nexus.nx_fast_detect_keypoints
detect_keypoints.restype = _c_int
detect_keypoints.argtypes = [_c_int, keypoint.Ptr, image.Ptr, _c_int]

score_keypoints = _nexus.nx_fast_score_keypoints
score_keypoints.restype = None
score_keypoints.argtypes = [_c_int, keypoint.Ptr, image.Ptr, _c_int]

detect_keypoints_pyr = _nexus.nx_fast_detect_keypoints_pyr
detect_keypoints_pyr.restype = _c_int
detect_keypoints_pyr.argtypes = [_c_int, keypoint.Ptr, _c_int, keypoint.Ptr, image_pyr.Ptr, _c_int, _c_int]

suppress_keypoints = _nexus.nx_fast_suppress_keypoints
suppress_keypoints.restype = _c_int
suppress_keypoints.argtypes = [_c_int, keypoint.Ptr, _c_int, keypoint.Ptr]
