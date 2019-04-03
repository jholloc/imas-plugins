import imas
import numpy as np
import getpass

user = getpass.getuser()

ids = imas.ids(1000, 0)
ids.create_env(user, 'test', '3')

ids.magnetics.flux_loop.resize(10)
for i in range(10):
    ids.magnetics.flux_loop[i].position.resize(2)
    ids.magnetics.flux_loop[i].position[0].r = 0.1 * (i + 1)
    ids.magnetics.flux_loop[i].position[0].z = -0.1 * (i + 1)
    ids.magnetics.flux_loop[i].position[1].r = 10.0 * (i + 1)
    ids.magnetics.flux_loop[i].position[1].z = -10.0 * (i + 1)
    ids.magnetics.flux_loop[i].flux.time = np.linspace(0, 1, 1000, dtype=np.float64)
    ids.magnetics.flux_loop[i].flux.data = np.linspace(500, 1000, 1000, dtype=np.float64)

ids.magnetics.ids_properties.homogeneous_time = 0

ids.magnetics.put()
ids.close()
