#include "lidar/sector.h"



Sector Sector::intersect(const Sector &other) const
{
    const Sector* a = this;
    const Sector* b = &other;
    if (a->_start > b->_start) std::swap(a, b);

    if (b->_start <= a->_end) {
        return Sector(b->_start, std::min(a->_end, b->_end));
    } else if (b->_end >= a->_start + 2.0f * PI) {
        return Sector(a->_start + 2.0f * PI,
                      std::min(a->_end + 2.0f * PI, b->_end));
    } else
        return Sector();
}

bool Sector::contains(float angle) const{
    angle = normalize(angle);
    if (angle >= _start)
        return angle <= _end;
    else
        return angle + 2.0f * PI <= _end;
}

void Sector::normalize(float &start, float &end)
{
    bool full_circle = end - start >= 2.0f * PI;
    if (full_circle) {
        start = 0;
        end = 2.0f * PI;
    } else {
        start = normalize(start);
        end = normalize(end);
        if (end < start) end += 2.0f * PI;
    }
}

float Sector::normalize(float x)
{
    return x - floorf(x / (2.0f * PI)) * 2.0f * PI;
}
