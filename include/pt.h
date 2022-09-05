#include <cmath>
#include <cassert>
#include <array>

#ifndef PT_PT_H
#define PT_PT_H

template<typename T>
struct Pt {
    T x;
    T y;

    ///Operator : equals
    inline bool equals(const Pt<T> &other) {
        return feq(x, other.x) && feq(y, other.y);
    }

    ///Operator : equals
    inline bool operator==(const Pt<T> &other) {
        return equals(other);
    }

    ///Operator : not equal
    inline bool operator!=(const Pt<T> &other) {
        return !equals(other);
    }

    ///As Array
    std::array<T, 2> as_array() {
        return {x, y};
    }
};

template<typename T>
struct Pt3d {
    T x;
    T y;
    T z;

    ///Operator : equals
    inline bool equals(const Pt3d<T> &other) {
        return feq(x, other.x) && feq(y, other.y) && feq(z, other.z);
    }

    ///Operator : equals
    inline bool operator==(const Pt3d<T> &other) {
        return equals(other);
    }

    ///Operator : not equal
    inline bool operator!=(const Pt3d<T> &other) {
        return !equals(other);
    }

    ///As Array
    std::array<T, 3> as_array() {
        return {x, y, z};
    }
};

#endif //PT_PT_H
