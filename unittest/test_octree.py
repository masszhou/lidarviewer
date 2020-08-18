# This Python file uses the following encoding: utf-8
import struct
import socket
import time
import numpy as np


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
    def __init__(self, *args, **kwargs):
        # start up viewer in separate process
        self._port_number = 10086

    def load(self, *args, **kwargs):
        positions = np.asarray(args[0], dtype=np.float32).reshape(-1, 3)
        attr = args[1:]
        color_map = kwargs.get('color_map', 'jet')
        scale = kwargs.get('scale', None)
        self._load(positions)

    def _load(self, positions):
        # if no points, then done
        if positions.size == 0:
            return
        # construct message
        num_points = int(positions.size / 3)
        msg = struct.pack('b', 1) \
              + struct.pack('i', num_points) + positions.tostring()
        # send message to viewer
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


if __name__ == "__main__":
    pts = np.random.rand(100, 3) - 0.5
    pts = np.ceil(pts*10)
    print(pts)

    right = pts[:, 0] > 0
    top = pts[:, 1] > 0
    front = pts[:, 2] > 0
    left = np.logical_not(right)
    down = np.logical_not(top)
    back = np.logical_not(front)

    left_down = np.logical_and(left, down)
    left_top = np.logical_and(left, top)
    right_down = np.logical_and(right, down)
    right_top = np.logical_and(right, top)

    left_down_back = np.logical_and(left_down, back)
    left_down_front = np.logical_and(left_down, front)
    left_top_back = np.logical_and(left_top, back)
    left_top_front = np.logical_and(left_top, front)
    right_down_back = np.logical_and(right_down, back)
    right_down_front = np.logical_and(right_down, front)
    right_top_back = np.logical_and(right_top, back)
    right_top_front = np.logical_and(right_top, front)

    print("left_down_back: ", np.count_nonzero(left_down_back))
    print("left_down_front: ", np.count_nonzero(left_down_front))
    print("left_top_back: ", np.count_nonzero(left_top_back))
    print("left_top_front: ", np.count_nonzero(left_top_front))
    print("right_down_back: ", np.count_nonzero(right_down_back))
    print("right_down_front: ", np.count_nonzero(right_down_front))
    print("right_top_back: ", np.count_nonzero(right_top_back))
    print("right_top_front: ", np.count_nonzero(right_top_front))

    v = Viewer()
    v.load(pts)
