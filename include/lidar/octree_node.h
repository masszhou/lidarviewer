// created by Zhiliang Zhou 2018-2019
// https://github.com/masszhou/lidarviewer

#ifndef LIDARVIEWER_OCTREE_NODE_H
#define LIDARVIEWER_OCTREE_NODE_H
#include <vector>
#include <cstdint>

struct OctreeNode {
    // during building the octree, we will generate new center points from octree structure
    // we append these new central points on original point list, and record the index w.r.t to the point list
    std::size_t centroid_index;
    long point_index_offset;  // pointer offset
    uint32_t point_count;
    bool is_leaf;
    std::vector<float> origin;
    std::vector<float> half_dimension;
    std::vector<OctreeNode* > children;
    /* childen index given by 3-bit code: xyz, where z is least significant bit
       Children follow a predictable pattern to make accesses simple.
       Here, - means less than 'origin' in that dimension, + means greater than.
       child:	0 1 2 3 4 5 6 7
       x:      - - - - + + + +
       y:      - - + + - - + +
       z:      - + - + - + - +
          y
          |
          |
         / ---- x
        /
       z
       value 0  left_bottom_back:   000
       value 1  left_bottom_front:  001
       value 2  left_top_back:      010
       value 3  left_top_front:     011
       value 4  right_bottom_back:  100
       value 5  right_bottom_front: 101
       value 6  right_top_back:     110
       value 7  right_top_front:    111
     */

    OctreeNode():
        centroid_index(0),
        point_index_offset(0),
        point_count(0),
        is_leaf(true),
        origin(3, 0.0f),
        half_dimension(3, 0.0f),
        children(8, nullptr)
    {
    }
    ~OctreeNode(){
        for (auto each: children){
            if (each != nullptr){
                delete each;
                each = nullptr;
            }
        }
    }
};
#endif // LIDARVIEWER_OCTREE_NODE_H
