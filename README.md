# 1. Overview
* derived from https://github.com/heremaps/pptk
* modifed for own usage
* only cmake build tested
* test environment, qt 5.15.0, python 3.8

# 2.Install
```
$ cd ${project_dir}
$ mkdir build
$ cd build
$ cmake -DBUILD_EXAMPLES=True ..
$ make -j8
```

# 3. usage
```
$ cd ${project_dir}
$ ./build/bin/example_lidarviewer
[Viewer]: TCP server set up on port  42230
$ python lidarviewer/test_point_cloud.py 42230
```

# 4. working as debug tools
* some initial results on KITTI data

<img src="./doc/imgs/kitti_2011_09_26_0023.png"  width="900" />