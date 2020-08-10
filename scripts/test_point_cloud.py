# This Python file uses the following encoding: utf-8
import struct
import socket
import time
import numpy as np
import argparse
import os
from time import sleep
import fire


class Viewer:
    """
    message header
    1 = load points
    2 = clear points
    3 = reset view to fit all
    4 = set viewer property
    5 = get viewer property
    6 = print screen
    7 = wait for enter
    8 = load camera path animation
    9 = playback camera path animation
    10 = set per point attributes
    """
    def __init__(self, port: int):
        # start up viewer in separate process
        self._port_number = port

    def load_points(self, *args):
        positions = np.asarray(args[0], dtype=np.float32).reshape(-1, 3)
        colors = np.asarray(args[1], dtype=np.float32)
        self._load_positions(positions)
        self._load_colors(colors)

    def load_boxes(self, xyzwhl_array):
        pass

    def _load_positions(self, positions):
        # if no points, then done
        if positions.size == 0:
            return
        # construct message
        num_points = int(positions.size / 3)
        msg = struct.pack('b', 1) + struct.pack('i', num_points) + positions.tobytes()
        # send message to viewer
        self._send(msg)

    def _load_colors(self, colors):
        if colors.size == 0:
            return
        num_points = int(colors.size)
        msg = struct.pack('b', 10) + struct.pack('i', num_points) + colors.tostring()
        self._send(msg)

    def _send(self, msg):
        start = time.time()
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('localhost', self._port_number))
        totalSent = 0
        while totalSent < len(msg):
            sent = s.send(msg)
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalSent = totalSent + sent
        s.close()
        print("{} [sec]".format(time.time() - start))


def load_kitti(viewer_port: int, path="/media/zzhou/data-KITTI/object/training/velodyne/003363.bin"):
    pts_kitti = np.fromfile(path, dtype=np.float32, count=-1).reshape(-1, 4)
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
    pts_opengl = np.zeros_like(pts_kitti)
    pts_opengl[:, 0] = pts_kitti[:, 1]*(-1)
    pts_opengl[:, 1] = pts_kitti[:, 2]
    pts_opengl[:, 2] = pts_kitti[:, 0]*(-1)
    pts_opengl[:, 3] = pts_kitti[:, 3]

    print(np.max(pts_opengl[:, 3]), np.min(pts_opengl[:, 3]))

    test_view = Viewer(viewer_port)
    test_view.load_points(pts_opengl[:, :3], pts_opengl[:, 3])


def load_npy_folder(viewer_port: int, data_path):
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
        test_view = Viewer(viewer_port)
        test_view.load_points(pts_opengl[:, :3], pts_opengl[:, 3])
        sleep(0.1)


if __name__ == "__main__":
    fire.Fire(load_kitti)