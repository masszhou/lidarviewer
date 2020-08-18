from pathlib import Path
import subprocess
from typing import Optional
import random
import numpy as np
import struct
import socket
import time
import select


class LidarViewer:
    """
    message header
    11 = reserve socket port
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
    def __init__(self, socket_port: Optional[int] = None, debug=False):
        self._exec_file_name = "lidarviewer_exec"
        self._exec_abs_path = str(Path(__file__).parent / self._exec_file_name)
        # start up viewer in separate process
        if socket_port is None:
            self._socket_port = random.randint(30000, 60000)
        else:
            self._socket_port = socket_port

        args = (self._exec_abs_path, str(self._socket_port))
        self._process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=(None if debug else subprocess.PIPE))

    def load_np_file(self, filepath: str):
        """
        :param filepath: e.g. "/media/zzhou/data-KITTI/object/training/velodyne/000050.bin"
        """
        pts_kitti = np.fromfile(filepath, dtype=np.float32, count=-1).reshape(-1, 4)
        self.load_points(pts_kitti[:, :3], pts_kitti[:, 3])

    def load_points(self, *args):
        positions = np.asarray(args[0], dtype=np.float32).reshape(-1, 3)
        colors = np.asarray(args[1], dtype=np.float32)
        self._load_positions(positions)
        self._load_colors(colors)

    def load_boxes(self, xyzwhl_array):
        pass

    def clear(self):
        """ Removes the current point cloud in the viewer
        """
        msg = struct.pack('b', 2)
        self._send(msg)

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
        msg = struct.pack('b', 10) + struct.pack('i', num_points) + colors.tobytes()
        self._send(msg)

    def _send(self, msg, msg_name=""):
        start = time.time()
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('localhost', self._socket_port))
        totalSent = 0
        while totalSent < len(msg):
            sent = s.send(msg)
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalSent = totalSent + sent
        s.close()
        print("{}:{} [sec]".format(msg_name, time.time() - start))

    def _recv(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('localhost', self._socket_port))
        s.setblocking(False)
        ready = select.select([s], [], [], 3)  # timeout in 3 sec
        if ready[0]:
            data = s.recv(10086)  # bytestring
            port = struct.unpack("<L", data)[0]
            print(f"server port:{port}")
        s.close()
