
# distutils: language = c++
# cython: embedsignature = True

# from libcpp.vector cimport vector
import numpy as np

# Define PY_ARRAY_UNIQUE_SYMBOL
cdef extern from "pyarray_symbol.h":
    pass

cimport numpy as np

np.import_array()

cdef extern from "pywrapper.h":
    cdef object c_marching_cubes "marching_cubes"(np.ndarray, double, double) except +
    cdef object c_marching_cubes_func "marching_cubes_func"(tuple, tuple, int, int, int, object, double, double) except +



def _marching_cubes(np.ndarray volume, float isovalue, float truncated_value):
    
    verts, faces = c_marching_cubes(volume, isovalue, truncated_value)
    verts.shape = (-1, 3)
    faces.shape = (-1, 3)
    return verts, faces

def _marching_cubes_func(tuple lower, tuple upper, int numx, int numy, int numz, object f, float isovalue, float truncated_value):
    
    if any(l_i >= u_i for l_i, u_i in zip(lower, upper)):
        raise ValueError("lower coordinates cannot be larger than upper coordinates")
    
    if numx < 2 or numy < 2 or numz < 2:
        raise ValueError("numx, numy, numz cannot be smaller than 2")

    verts, faces = c_marching_cubes_func(lower, upper, numx, numy, numz, f, isovalue, truncated_value)
    verts.shape = (-1, 3)
    faces.shape = (-1, 3)
    return verts, faces
