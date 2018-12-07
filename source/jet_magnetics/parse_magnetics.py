from enum import Enum, auto
from collections import namedtuple

file_name = '/Users/jhollocombe/Projects/iter-plugins/source/exp2imas/mappings/JET/magnetics/68613/sensors_200c_detail.txt'

with open(file_name) as file:
    lines = file.readlines()


uid = 'jetppf'

section = None


class Section(Enum):
    FLUX = auto()
    SADDLE = auto()
    PICK_UP = auto()
    HALL = auto()
    OTHER = auto()


FluxLoop = namedtuple('FluxLoop', 'num r z factor rel_err abs_err name desc oct magn dda dtype source')
PickUp = namedtuple('PickUp', 'num r z pol_ang factor rel_err abs_err name desc oct tor_ang magn dda dtype')

flux_loops = []
saddle_loops = []
pickups = []


for line in lines:
    line = line.strip()

    if not line:
        continue

    print('>>> ' + line)

    if line.startswith('#END'):
        section = None
    elif line.startswith('#FLUX'):
        section = Section.FLUX
        continue
    elif line.startswith('#SADDLE'):
        section = Section.SADDLE
        continue
    elif line.startswith('#PICK-UP'):
        section = Section.PICK_UP
        continue
    elif line.startswith('#HALL'):
        section = Section.HALL
        continue
    elif line.startswith('#OTHER'):
        section = Section.OTHER
        continue

    tokens = [i.strip() for i in line.split(',')]

    if section == Section.FLUX:
        num, r, z, factor, rel_err, abs_err, name, desc, oct, magn, dda, dtype, *_ = tokens
        flux_loops.append(FluxLoop(num, (r, 0), (z, 0), factor, rel_err, abs_err, name, desc, oct, magn, dda, dtype, "PPF/MAGN/" + magn))

    if section == Section.SADDLE:
        num, r1, z1, r2, z2, factor, rel_err, abs_err, name, desc, oct, tor_ang, magn, dda, dtype, *_ = tokens
        saddle_loops.append(FluxLoop(num, (r1, r2), (z1, z2), factor, rel_err, abs_err, name, desc, oct, magn, dda, dtype, "PPF/MAGN/" + magn))

    if section == Section.PICK_UP:
        num, r, z, pol_ang, factor, rel_err, abs_err, name, desc, oct, tor_ang, magn, dda, dtype, *_ = tokens
        pickups.append(PickUp(num, r, z, pol_ang, factor, rel_err, abs_err, name, desc, oct, tor_ang, magn, dda, dtype))

flux_loops += [loop for loop in saddle_loops if loop.dda == 'MNJ3' and not loop.dtype.startswith('BSAD')]
flux_loops += sorted([loop for loop in saddle_loops if loop.dda == 'MNJ3' and loop.dtype.startswith('BSAD')], key=lambda loop: loop.dtype)

keep_pickups = [pickup for pickup in pickups if pickup.dda == 'MNJ3' or pickup.dtype.startswith('PLB')]
tan_coils = [pickup for pickup in pickups if pickup.dda == 'MNJ8' and (pickup.dtype.startswith('UPC') or pickup.dtype.startswith('UNC') or pickup.dtype.startswith('IC'))]
tpc_coils = [pickup for pickup in pickups if pickup.dda == 'MNJ1' and (pickup.dtype.startswith('TPC') or pickup.dtype.startswith('TNC'))]

keep_pickups += tan_coils
keep_pickups += tpc_coils

rs = []
for loop in keep_pickups:
    rs += [loop.r]

a = [4.2920, 4.0880, 3.7790, 3.3810, 2.8150, 2.2170, 1.8640, 1.7390, 1.6790, 1.6790, 1.7390, 1.8640, 2.2170, 2.8150, 3.3810, 3.7790, 4.0880, 4.2920, 3.0810, 3.0790, 2.9250, 2.8240, 2.7590, 2.5910, 2.5260, 2.4250, 2.3650, 2.2530, 2.2520, 3.0800, 3.0780, 2.9250, 2.8240, 2.7590, 2.5910, 2.5260, 2.4250, 2.3650, 2.2530, 2.2530, 3.8870, 3.9720, 4.0030, 3.9500, 3.8160, 3.6540, 3.4690, 2.3000, 2.5260, 2.9730, 2.1200, 2.3090, 2.5290, 2.7600, 1.8440, 1.7790, 1.7810, 2.9720, 2.9720, 2.6850, 2.6200, 2.3410, 2.3410, 2.1210, 2.9700, 2.9700, 2.7160, 2.5900, 2.3430, 2.3430, 2.1170]

print(len(rs))
print(list(float(i) for i in rs))
print(len(a))
print(a)
assert(list(float(i) for i in rs) == a)