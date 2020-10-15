# Configuration File

- [Example file](https://github.com/spof95/ESCenterlines/blob/master/ESCenterlines/clsettings.json)

There are various parameters defining the extended centerlines:

- `airport` and `runway` define the airport/runway configuration for which the extended centerline definition should be applied. Wildcards are possible using the string "*"
- `course` defines the course of the extended centerline. If it is set to 0, the course of the runway to which the centerline belongs to is chosen. If you want to set the centerline to point to the north, use course 360
- `fap` optionally defines the FAP for the specified runway. In this case, the centerline will pass throught the FAP. The course setting is ignored.
- `color` defines the color of the extended centerline
- `cl_elements` is an array defining the look of the elements building the extended centerline. Multiple element definitions are possible, they will be plotted sequentially starting from the runway threshold:
  - `dash` defines the length of a dash (in NM)
  - `gap` defines the length of a gap (in NM)
  - `number` defines how often the given dash-gap pair should be repeated
  - `starts_gap` defines if the block should start with a dash or a gap
- `cl_markers` is an array defining range ticks on the extended centerline. Multiple range tick definitions are possible:
  - `angle` defines the relative angle of the range tick to the course of the extended centerline
  - `d_airport` and `d_runway` allow selective hiding of the range tick if the specified airport/runway is active.
  - `direction` defines on which side of the extended centerline the range tick should be plotted.
  - `dist_cl` defines the lateral distance of the range tick to the extended centerline.
  - `dist_thr` defines the distance of the range tick to the runway threshold.
  - `length` defines the length of the range tick (in NM)
