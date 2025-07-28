from _mcubes import _marching_cubes, _marching_cubes_func
import numpy as np
def voxel_decimate(vertices: np.ndarray,
                   triangles: np.ndarray,
                   voxel_size: float,
                   area_eps: float = 0.0):
    """
    Parameters
    ----------
    vertices : (N, 3) float32/64
    triangles : (M, 3) int64/int32
    voxel_size : float
    area_eps : float, default 0.0
    Returns
    -------
    new_vertices : (N', 3)
    new_triangles : (M', 3)
    """
    V = vertices
    F = triangles

    if len(V) == 0 or len(F) == 0:
        return V.copy(), F.copy(), np.arange(len(V)), np.zeros(len(F), dtype=bool)

    grid = np.floor(V / voxel_size).astype(np.int64)
    uniq, labels = np.unique(grid, axis=0, return_inverse=True)
    counts = np.bincount(labels)
    new_vertices = np.zeros((len(uniq), 3), dtype=V.dtype)
    for i in range(3):
        new_vertices[:, i] = np.bincount(labels, weights=V[:, i]) / counts

    old2new = labels

    F_new = old2new[F]

    i0, i1, i2 = F_new[:, 0], F_new[:, 1], F_new[:, 2]
    non_dup = (i0 != i1) & (i1 != i2) & (i0 != i2)

    if area_eps > 0.0:
        v0 = new_vertices[i0]
        v1 = new_vertices[i1]
        v2 = new_vertices[i2]
        areas = 0.5 * np.linalg.norm(np.cross(v1 - v0, v2 - v0), axis=1)
        non_small = areas > area_eps
        keep = non_dup & non_small
    else:
        keep = non_dup

    new_triangles = F_new[keep]

    return new_vertices, new_triangles

def marching_cubes(volume, isovalue = 0.0, truncated_value = 0.0):
    vertices, triangles = _marching_cubes(volume, isovalue, truncated_value)
    vertices, triangles = voxel_decimate(vertices, triangles, 0.5)
    return vertices, triangles
def marching_cubes_func(lower, upper, numx, numy, numz, f, isovalue, truncated_value = 0.0):
    vertices, triangles = _marching_cubes_func(lower, upper, numx, numy, numz, f, isovalue, truncated_value, 1.0)
    vertices, triangles = voxel_decimate(vertices, triangles, 0.5)
    return vertices, triangles