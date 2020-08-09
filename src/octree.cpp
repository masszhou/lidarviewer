// references
// 1. pptk project
// 2. simple octree, https://github.com/brandonpelfrey/SimpleOctree/blob/master/Octree.h

#include "lidar/octree.h"
#include "lidar/box3.h"

#include <QtDebug>

#include <ctime>


Octree::Octree():
    _max_leaf_capacity(10),
    _num_points(0),
    _ptr_root_node(nullptr),
    _ptr_point_xyz(nullptr),
    _lower_left_corner(3, 0.0f),
    _cube_size(0.0f)
{

}

Octree::~Octree()
{
    deleteTree(_ptr_root_node);
}

void Octree::buildTree(std::vector<float> &point_xyz, std::vector<float> &point_size, uint32_t max_leaf_size)
{
    deleteTree(_ptr_root_node);

    _max_leaf_capacity = max_leaf_size;
    _num_points = static_cast<uint32_t>(point_xyz.size()) / 3;
    _ptr_point_xyz = &point_xyz;  // reference data, which owned by PointCloud
    _ptr_point_size = &point_size;  // reference data, which owned by PointCloud

    // let non-centroid points have zero size
    point_size.resize(_num_points, 0.0f);  // resize and fill with 0 ?

    if (_num_points == 0) {
        _ptr_root_node = nullptr;
        return;
    }

    // ================================
    // compute bounding cube
    // ================================
    Box3<float> box;
    box.addPoints(&point_xyz[0], _num_points);
    float cube_center[3] = {0.5f * (box.x(0) + box.x(1)),
                            0.5f * (box.y(0) + box.y(1)),
                            0.5f * (box.z(0) + box.z(1))};
    float cube_size = std::max(box.x(1) - box.x(0), std::max(box.y(1) - box.y(0), box.z(1) - box.z(0)));
    float cube_corner[3] = {cube_center[0] - 0.5f * cube_size,
                            cube_center[1] - 0.5f * cube_size,
                            cube_center[2] - 0.5f * cube_size};
    for (size_t dim = 0; dim < 3; dim++)
        _lower_left_corner[dim] = cube_corner[dim];
    _cube_size = cube_size;


    // ================================
    // prepare scratch space
    // ================================
    _indices.clear();
    _indices.reserve(_num_points);
    for (uint32_t i = 0; i < _num_points; i++)
        _indices.push_back(i);
    _octant_id.resize(_num_points, 0);


    // ---- begin recursion insert data into octree
    _ptr_root_node = buildTreeHelper(_indices.data(), _octant_id.data(), _num_points, _lower_left_corner, _cube_size);


    // ---- reorder points
    _indices_r.resize(_num_points);
    std::vector<float> temp_xyz;
    temp_xyz.reserve(point_xyz.size());
    for (unsigned int i = 0; i < _num_points; i++) {
        for (unsigned int dim = 0; dim < 3; dim++) {
            temp_xyz.push_back(point_xyz[3 * _indices[i] + dim]);
        }
        _indices_r[_indices[i]] = i;
    }
    for (unsigned int i = _num_points; i < point_xyz.size() / 3; i++) {
        for (unsigned int dim = 0; dim < 3; dim++) {
            temp_xyz.push_back(point_xyz[3 * i + dim]);
        }
    }
    point_xyz.swap(temp_xyz);
}

OctreeNode *Octree::buildTreeHelper(uint32_t *ptr_indices,  // &_indices[0] from std::vector<unsigned int> _indices;
                                    uint32_t *ptr_octant_id,  // &_labels[0] from std::vector<unsigned char>
                                    const unsigned int num_pts,
                                    const std::vector<float> cube_corner,
                                    const float cube_size)
{
    // each node can hold max _max_leaf_capacity pts,
    // when there are more pts inserted to this node,
    // split the node into 8 sub-nodes recursively,
    // until each node's capacity not exceed


    OctreeNode* ptr_node;
    if (num_pts == 0) {
        // if no data, then we dont need node, then we are done
        ptr_node = nullptr;
        return ptr_node;

    } else if (pointsAreIdentical(ptr_indices, num_pts) || num_pts <= _max_leaf_capacity) {
        // If this capacity of this node doesn't exceed
        // and it is a leaf, then we're done!
        // or remained points are identical, then this is a leaf, then we rae done

        // create leaf node
        ptr_node = new OctreeNode();
        //node->point_index_offset = indices - &_indices[0];  // 这个地址操作是什么鬼？
        ptr_node->point_count = num_pts;
        ptr_node->is_leaf = true;

        // compute centroid
        std::vector<float> centroid_xyz = computeCentroid(ptr_indices, num_pts);
        ptr_node->centroid_index = appendCentroidToPointList(centroid_xyz);
        _ptr_point_size->push_back(cube_size);

        return ptr_node;
    } else {
        // If there are more point than node capcacity
        // and it is a inner node, then we create children nodes

        // create inner node recursively
        ptr_node = new OctreeNode();
        ptr_node->point_index_offset = ptr_indices - &_indices[0];  // record offset of pointer
        ptr_node->point_count = num_pts;
        ptr_node->is_leaf = false;

        // compute cube center
        std::vector<float> cube_center = {0.0f, 0.0f, 0.0f};
        for (uint32_t dim = 0; dim < 3; dim++)
            cube_center[dim] = cube_corner[dim] + 0.5f * cube_size;

        {
//            clock_t begin = clock();
            // label points by child nodes (xyz encoding)
            for (uint32_t i = 0; i < num_pts; i++) {
                ptr_octant_id[i] = getOctantContainingPoint(ptr_indices[i], cube_center);  // in [0,1,2,3,4,5,6,7]
            }
//            clock_t end = clock();
//            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//            qDebug() <<  "[octree]: n_pts=" << num_pts << ", assign octant id time =" << elapsed_secs;
        }

        // partition points by labels
        uint32_t ptr_child_counts[8];  // pointer offset
        partitionXYZ(ptr_child_counts, ptr_indices, ptr_octant_id, num_pts);

        // recurse on children
        uint32_t* ptr_indices_cur = ptr_indices;  // copy pointer
        uint32_t* ptr_octant_id_cur = ptr_octant_id;
        for (unsigned int i = 0; i < 8; i++) {
            std::vector<float> child_corner = {(i & 4) == 0 ? cube_corner[0] : cube_center[0],
                                               (i & 2) == 0 ? cube_corner[1] : cube_center[1],
                                               (i & 1) == 0 ? cube_corner[2] : cube_center[2]};
            ptr_node->children[i] = buildTreeHelper(ptr_indices_cur,
                                                    ptr_octant_id_cur,
                                                    ptr_child_counts[i],
                                                    child_corner,
                                                    0.5f * cube_size);
            ptr_indices_cur += ptr_child_counts[i];
            ptr_octant_id_cur += ptr_child_counts[i];
        }

        // compute centroid
        std::vector<float> centroid_xyz = computeCentroid(ptr_node->children);
        ptr_node->centroid_index = appendCentroidToPointList(centroid_xyz);
        _ptr_point_size->push_back(cube_size);
    }
    return ptr_node;
}

void Octree::deleteTree(OctreeNode *root)
{
    if (!root) return;
    if (root->is_leaf) {
        delete root;
    } else {
        for (size_t i = 0; i < 8; i++) deleteTree(root->children[i]);
    }
}

std::vector<float> Octree::computeCentroid(std::vector<OctreeNode *> children)
{
    // compute centroid from child node centroids
    // assumes children are valid
    std::vector<float>& point_xyz = *_ptr_point_xyz;  // define a reader
    std::vector<float> centroid_xyz(3, 0.0f);
    unsigned int count = 0;
    for (unsigned int i = 0; i < 8; i++) {
        if (children[i] == nullptr) continue;
        float* ptr_child_centroid_xyz = &point_xyz[3 * children[i]->centroid_index];
        unsigned int child_count = children[i]->point_count;
        for (unsigned int dim = 0; dim < 3; dim++) {
            centroid_xyz[dim] += ptr_child_centroid_xyz[dim] * child_count;
        }
        count += child_count;
    }
    for (unsigned int dim = 0; dim < 3; dim++) {
        centroid_xyz[dim] /= count;
    }
    return centroid_xyz;
}

std::vector<float> Octree::computeCentroid(const unsigned int *indices, const unsigned int count)
{
    std::vector<float> pos{0.0f, 0.0f, 0.0f};
    for (unsigned int i = 0; i < count; i++) {
        for (unsigned int dim = 0; dim < 3; dim++) {
            pos[dim] += (*_ptr_point_xyz)[3 * indices[i] + dim];
        }
    }
    for (unsigned int dim = 0; dim < 3; dim++) {
        pos[dim] /= count;
    }
    return pos;
}

size_t Octree::appendCentroidToPointList(const std::vector<float> &pt_xyz)
{
    size_t index = _ptr_point_xyz->size() / 3;
    for (unsigned int dim = 0; dim < 3; dim++) {
        (*_ptr_point_xyz).push_back(pt_xyz[dim]);
    }
    return index;
}



uint32_t Octree::getOctantContainingPoint(const uint32_t point_idx, const std::vector<float>& origin) const
{
    // x->point[0], y->point[1], z->point[2]
    uint32_t oct = 0;
    if((*_ptr_point_xyz)[3 * point_idx + 0] >= origin[0]) oct |= 4;
    if((*_ptr_point_xyz)[3 * point_idx + 1] >= origin[1]) oct |= 2;
    if((*_ptr_point_xyz)[3 * point_idx + 2] >= origin[2]) oct |= 1;
    return oct;
}

uint32_t Octree::partition(uint32_t *p_indices, uint32_t *p_octant_id, const uint32_t num_pts, const uint32_t bit)
{
    if (num_pts == 0) return 0;
    uint32_t left = 0;
    uint32_t right = num_pts - 1;
    uint32_t mask = 1u << bit;
    for (;;) {
        while (left < num_pts && (p_octant_id[left] & mask) == 0) left++;
        while (right > 0 && (p_octant_id[right] & mask)) right--;
        if (left >= right || right == 0) break;  // avoid later right-- overflow
        std::swap(p_indices[left], p_indices[right]);
        std::swap(p_octant_id[left], p_octant_id[right]);
        left++;
        right--;  // dangerous for unsigned int, when below 0, it will overflow to max 4,294,967,295
    }
    return left;
}

// recursive 3 times
void Octree::partitionXYZ(uint32_t *p_child_counts, uint32_t *p_indices, uint32_t *p_octant_id, uint32_t num_pts, uint32_t bit)
{
    // bit: 2 - x, 1 - y, 0 - z
    unsigned int left = partition(p_indices, p_octant_id, num_pts, bit);  // partition along x axis
    if (bit == 0) {
        p_child_counts[0] = left;
        p_child_counts[1] = num_pts - left;
    } else {
        partitionXYZ(p_child_counts, p_indices, p_octant_id, left, bit - 1);
        unsigned int offset = 1 << bit;
        partitionXYZ(p_child_counts + offset, p_indices + left, p_octant_id + left, num_pts - left, bit - 1);
    }
}

bool Octree::pointsAreIdentical(const unsigned int *indices, const unsigned int count)
{
    if (count == 1) return true;
    std::vector<float>& points = *_ptr_point_xyz;  // reader
    uint32_t first_index = indices[0];
    float first_point[3] = {points[3 * first_index + 0],
                            points[3 * first_index + 1],
                            points[3 * first_index + 2]};
    for (uint32_t i = 1; i < count; i++)
      for (uint32_t dim = 0; dim < 3; dim++)
        if (points[3 * indices[i] + dim] - first_point[dim] != 0.0f) return false;
    return true;
}


