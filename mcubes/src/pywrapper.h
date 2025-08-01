
#ifndef _PYWRAPPER_H
#define _PYWRAPPER_H

#include <Python.h>
#include "pyarraymodule.h"

#include <vector>

PyObject* marching_cubes(PyArrayObject* arr, double isovalue, double truncated_value = 0);
PyObject* marching_cubes_func(PyObject* lower, PyObject* upper,
    int numx, int numy, int numz, PyObject* f, double isovalue, double truncated_value = 0);

#endif // _PYWRAPPER_H
