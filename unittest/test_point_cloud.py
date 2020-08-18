import numpy as np
import os
from time import sleep
import fire
from typing import Optional
from lidarviewer import LidarViewer


def load_kitti(path: str = "/media/zzhou/data-KITTI/object/training/velodyne/000050.bin", viewer_port: Optional[int] = None):
    pts_kitti = np.fromfile(path, dtype=np.float32, count=-1).reshape(-1, 4)
    print(pts_kitti.shape)
    # Kitti Lidar coordiantes
    #
    #           (up)
    #             z x (front)
    #             |/
    # (left) y -- v

    # opengl right handed coordinate systems
    #     +y
    #      |
    #      v -- +x
    #     /
    #   +z
    # camera look from z axis = driving direction in kitti lidar coordinates

    print(np.max(pts_kitti[:, 3]), np.min(pts_kitti[:, 3]))

    test_view = LidarViewer(viewer_port)
    test_view.load_points(pts_kitti[:, :3], pts_kitti[:, 3])
    sleep(3)
    test_view.clear()


def load_npy_folder(data_path, viewer_port: Optional[int] = None):
    filenames = []
    if os.path.isdir(data_path):
        filenames = [os.path.join(data_path, f)
                     for f in os.listdir(data_path)
                     if f.endswith((".npy"))]
    else:
        filenames = [data_path, ]
    filenames.sort()

    for each in filenames:
        pts_passat = np.load(each)
        # pts_passat = np.load("/home/zzhou/devel/multi_tasks_network/ROS/bin/1532609167456003890.npy")
        pts_opengl = np.zeros_like(pts_passat)
        pts_opengl[:, 0] = pts_passat[:, 1]*(-1)
        pts_opengl[:, 1] = pts_passat[:, 2]
        pts_opengl[:, 2] = pts_passat[:, 0]*(-1)
        pts_opengl[:, 3] = pts_passat[:, 3]
        pts_opengl[:, :3] = pts_opengl[:, :3]*2
        test_view = LidarViewer(viewer_port)
        test_view.load_points(pts_opengl[:, :3], pts_opengl[:, 3])
        sleep(0.1)


if __name__ == "__main__":
    fire.Fire(load_kitti)
