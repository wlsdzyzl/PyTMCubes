
import numpy as np
import mcubes


# Create a data volume
X, Y, Z = np.mgrid[:100, :100, :50]
radius = 40
## compute signed voxel distance to the half-sphere
u1 = ((X - 50)**2 + (Y - 50) ** 2 + (Z- 50) ** 2)**0.5 - radius
xy_dist = ((X - 50)**2 + (Y - 50) ** 2)**0.5 - radius
u2 = np.sign(xy_dist) * (xy_dist**2 + Z ** 2)**0.5
u = np.concatenate((u1, u2), axis=-1)
# Extract the 0-isosurface with truncated values
vertices1, triangles1 = mcubes.marching_cubes(u, isovalue = 0.0, truncated_value = 5)
mcubes.export_obj(vertices1, triangles1, "half_sphere.obj", "MyHalfSphere")
# Extract the 0-isosurface without truncate values
vertices1, triangles1 = mcubes.marching_cubes(u, isovalue = 0.0)
mcubes.export_obj(vertices1, triangles1, "half_sphere_wo_truncation.obj", "MyHalfSphere")





