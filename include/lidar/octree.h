// references
// 1. pptk project
// 2. simple octree, https://github.com/brandonpelfrey/SimpleOctree/blob/master/Octree.h

#ifndef OCTREE_H
#define OCTREE_H
#include <vector>
#include <cstdint>  // restrict type length
#include "octree_node.h"

#include "camera.h"
#include "camera_frustum.h"


class Octree final
{
public:

    enum ProjectionMode { PERSPECTIVE, ORTHOGRAPHIC };

    Octree();
    ~Octree();

    void buildTree(std::vector<float>& point_xyz, std::vector<float>& point_size, uint32_t max_leaf_size = 64);
    OctreeNode *buildTreeHelper(uint32_t* ptr_indices,
                                uint32_t* ptr_octant_id,
                                const unsigned int count,
                                const std::vector<float> cube_corner,
                                const float cube_size);

    void deleteTree(OctreeNode* root);

private:
    // computer the center points for the octree node
    std::vector<float> computeCentroid(const uint32_t* ptr_indices, const uint32_t num_pts);
    std::vector<float> computeCentroid(std::vector<OctreeNode*> children);

    // during building the octree, we will computer the new center points from the octree node/box
    // we append these new central points on original point list, and record the index w.r.t to the point list
    std::size_t appendCentroidToPointList(const std::vector<float>& value);

    /*!
     * \brief getOctantContainingPoint
     *
     * right-hand coordinates was used to describe faces'name, the same with openGL canvas coordinates
     * octant_id  face_name           mask
     * 0          left_bottom_back:   000
     * 1          left_bottom_front:  001
     * 2          left_top_back:      010
     * 3          left_top_front:     011
     * 4          right_bottom_back:  100
     * 5          right_bottom_front: 101
     * 6          right_top_back:     110
     * 7          right_top_front:    111
     * \param point_idx, index from pointcloud block, which refered by _ptr_point_xyz
     * \param origin, center point of current octree node
     * \return int, octant id, from [0,1,2,3,4,5,6,7]
     */
    uint32_t getOctantContainingPoint(const uint32_t point_idx, const std::vector<float>& origin) const;

    /*!
     * \brief partition to 2 parts by swaping element in (*ptr_indices) based on (*ptr_octant_id)
     *        similar to bidirection sort
     *
     * partition to left/right when bit = 2 -> mask =0b0100
     * partition to down/top   when bit = 1 -> mask =0b0010
     * partition to back/front when bit = 0 -> mask =0b0001
     */
    uint32_t partition(uint32_t* ptr_indices, uint32_t* ptr_octant_id, const uint32_t num_pts, const uint32_t bit);

    /*!
     * \brief partitionXYZ, recursively reorder all points into 8 octant by x, y, z axis successively
     *
     * the final reordered sequence is
     * left-down-back, left-down-front, left-top-back, left-top-front, right-down-back, right-down-front, right-top-back, right-top-front
     */
    void partitionXYZ(uint32_t* ptr_child_counts, uint32_t* ptr_indices, uint32_t* ptr_octant_id, uint32_t num_pts, uint32_t bit = 2);

    /*!
     * \brief pointsAreIdentical, return false when least 2 points are different to avoid logical dead lock
     *
     * for example, when coordinates are integer or ceiled float, there could be 33 points with same coordinates,
     * but the max capacity of leaf are 32, thus the octree will always try to be fined to split these 33 points.
     * and it will never succeed, since these 33 points are identical.
     *
     * another solution is setting max depth for octree.
     *
     * the better way is using both check to get a optimal octree structure
     */
    bool pointsAreIdentical(const unsigned int* indices, const unsigned int count);

private:
    uint32_t _max_leaf_capacity;  // each node can hold max _max_leaf_capacity pts
    uint32_t _num_points;  // there are total _num_points for this PointCloud instance
    OctreeNode* _ptr_root_node;
    std::vector<float>* _ptr_point_xyz;  // octree do not copy point clouds, but directly operate on given pointer
    std::vector<float>* _ptr_point_size;

    std::vector<float> _lower_left_corner;
    float _cube_size;

    // scratch space for buildTreeHelper
    // index range is up to to 4,294,967,295, due to uint32_t
    std::vector<uint32_t> _indices;
    std::vector<uint32_t> _indices_r;
    std::vector<uint32_t> _octant_id;  // 0,1,2,3,4,5,6,7

};

#endif // OCTREE_H
