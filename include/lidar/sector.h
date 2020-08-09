#ifndef SECTOR_H
#define SECTOR_H
#include <iostream>
#include <math.h>
#define PI 3.14159265359f

class Sector
{
public:
    Sector();
    Sector(float start, float end);

    operator bool();
    bool empty() const;

    /**
     * @brief returns true if angle is in sector (including _start and _end)
     */
    bool contains(float angle) const;

    /**
     * @brief returns the sector intersection between this and other
     */
    Sector intersect(const Sector& other) const;

    float getStart() const;
    float getEnd() const;

    static float rad2deg(float rad);
    static float deg2rad(float deg);


    friend std::ostream& operator<<(std::ostream&, const Sector&);
private:
    /**
     * @brief normalize  _start in [0, 2*pi) and _end in [_start, _start+2*pi]
     */
    static void normalize(float& start, float& end);

    /**
     * @brief normalizes angle x to be in [0, 2*pi)
     */
    static float normalize(float x);

    float _start;
    float _end;
};

inline Sector::Sector(): _start(0.0f), _end(-1.0f) { }
inline Sector::Sector(float start, float end): _start(start), _end(end) { normalize(_start, _end);}
inline Sector::operator bool() { return !empty(); }
inline bool Sector::empty() const { return _start == 0.0f && (_end + 1.0f) == 0.0f; }
inline float Sector::getStart() const { return _start; }
inline float Sector::getEnd() const { return _end; }
inline float Sector::rad2deg(float rad) { return rad / PI * 180.0f; }
inline float Sector::deg2rad(float deg) { return deg / 180.0f * PI; }

#endif // SECTOR_H
