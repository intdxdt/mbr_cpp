#include <cmath>
#include <cassert>
#include <array>
#include <vector>
#include <sstream>
#include <optional>
#include <iomanip>
#include <functional>

#include "include/mutil.h"
#include "include/pt.h"

#ifndef MBR_MBR_H
#define MBR_MBR_H
namespace mbr {
    template<typename T>
    struct MBR {
        T minx;
        T miny;
        T maxx;
        T maxy;

        MBR() : minx(0), miny(0), maxx(0), maxy(0) {}

        MBR(T minx, T miny, T maxx, T maxy) :
                minx(min(minx, maxx)), miny(min(miny, maxy)),
                maxx(max(minx, maxx)), maxy(max(miny, maxy)) {}

        MBR(T minx_, T miny_, T maxx_, T maxy_, bool raw) :
                minx(raw ? minx_ : min(minx_, maxx_)),
                miny(raw ? miny_ : min(miny_, maxy_)),
                maxx(raw ? maxx_ : max(minx_, maxx_)),
                maxy(raw ? maxy_ : max(miny_, maxy_)) {
        }

        explicit MBR(const std::array<T, 4> &bounds) :
                minx(min(bounds[0], bounds[2])),
                miny(min(bounds[1], bounds[3])),
                maxx(max(bounds[0], bounds[2])),
                maxy(max(bounds[1], bounds[3])) {
        }

        explicit MBR(const Pt<T> &pt) :
                minx(pt.x),
                miny(pt.y),
                maxx(pt.x),
                maxy(pt.y) {
        }

        MBR(const Pt<T> &a, const Pt<T> &b) {
            *this = MBR(a.x, a.y, b.x, b.y);
        }

        MBR(const std::array<T, 4> &bounds, bool raw) {
            *this = MBR(bounds[0], bounds[1], bounds[2], bounds[3], raw);
        }

        template<typename U>
        MBR<U> as() {
            auto bounds = *this;
            return MBR<U>{
                    static_cast<U>(bounds.minx),
                    static_cast<U>(bounds.miny),
                    static_cast<U>(bounds.maxx),
                    static_cast<U>(bounds.maxy),
                    true};
        }

        bool operator<(const MBR<T> &other) {
            auto d = minx - other.minx;
            if (feq(d, 0)) {
                d = miny - other.miny;
            }
            return d < 0;
        }

        [[using gnu : const, always_inline, hot]] [[nodiscard]]
        MBR<T> &bbox() { return *this; }

        [[using gnu : const, always_inline, hot]] [[nodiscard]]
        MBR<T> clone() { return *this; }

        [[using gnu : const, always_inline, hot]] [[nodiscard]]
        inline T width() const { return maxx - minx; }

        [[using gnu : const, always_inline, hot]] [[nodiscard]]
        inline T height() const { return maxy - miny; }

        [[using gnu : const, always_inline, hot]] [[nodiscard]]
        inline T area() const { return height() * width(); }

        std::vector<Pt<T>> as_poly_array() {
            return {
                    Pt<T>{minx, miny},
                    Pt<T>{minx, maxy},
                    Pt<T>{maxx, maxy},
                    Pt<T>{maxx, miny},
                    Pt<T>{minx, miny},
            };
        }

        std::array<T, 4> as_array() const {
            return std::array<T, 4>{minx, miny, maxx, maxy};
        }

        std::tuple<T, T, T, T> as_tuple() {
            return std::tuple<T, T, T, T>{
                    minx, miny, maxx, maxy
            };
        }

        std::pair<Pt<T>, Pt<T>> llur() {
            return std::make_pair<Pt<T>, Pt<T>>({minx, miny}, {maxx, maxy});
        }

        ///Compare equality of two minimum bounding box
        [[using gnu : const, always_inline, hot]] [[nodiscard]]
        bool equals(const MBR<T> &other) const {
            return eqls(maxx, other.maxx) &&
                   eqls(maxy, other.maxy) &&
                   eqls(minx, other.minx) &&
                   eqls(miny, other.miny);
        }

        ///Checks if bounding box can be represented as a point,
        /// has both width and height as 0.
        [[using gnu : const, always_inline, hot]] [[nodiscard]]
        bool is_point() const {
            return feq(height(), 0) && feq(width(), 0);
        }

        ///Contains bonding box
        ///is true if mbr completely contains other, boundaries may touch
        bool contains(const MBR<T> &other) const {
            return (other.minx >= minx) &&
                   (other.miny >= miny) &&
                   (other.maxx <= maxx) &&
                   (other.maxy <= maxy);
        }

        ///contains x, y
        [[using gnu : const, always_inline, hot]] [[nodiscard]]
        bool contains(T x, T y) const {
            return (x >= minx) &&
                   (x <= maxx) &&
                   (y >= miny) &&
                   (y <= maxy);
        }

        ///Completely contains bonding box
        ///is true if mbr completely contains other
        /// without touching boundary
        bool completely_contains(const MBR<T> &other) const {
            return (other.minx > minx) &&
                   (other.miny > miny) &&
                   (other.maxx < maxx) &&
                   (other.maxy < maxy);
        }

        ///completely_contains_xy is true if mbr completely
        /// contains location with {x, y} without touching boundary
        bool completely_contains(T x, T y) const {
            return (x > minx) &&
                   (x < maxx) &&
                   (y > miny) &&
                   (y < maxy);
        }

        ///Create new bounding box by translating by dx and dy.
        MBR<T> translate(T dx, T dy) const {
            return {minx + dx, miny + dy, maxx + dx, maxy + dy};
        }

        ///Computes the center of minimum bounding box - (x, y)
        Pt<T> center() const {
            auto d = static_cast<T>(2);
            return Pt<T>{(minx + maxx) / d, (miny + maxy) / d};
        }

        ///Checks if bounding box intersects other
        [[using gnu : const, always_inline, hot]]
        inline bool intersects(const MBR<T> &other) const {
            //not disjoint
            return !(other.minx > maxx ||
                     other.maxx < minx ||
                     other.miny > maxy ||
                     other.maxy < miny);
        }

        ///intersects point
        [[using gnu : const, always_inline, hot]]
        bool intersects(T x, T y) const {
            return contains(x, y);
        }

        ///intersects point
        [[using gnu : const, always_inline, hot]]
        bool intersects(const Pt<T> &pt1, const Pt<T> &pt2) const {
            auto minq = min(pt1.x, pt2.x);
            auto maxq = max(pt1.x, pt2.x);

            if (minx > maxq || maxx < minq) {
                return false;
            }

            minq = min(pt1.y, pt2.y);
            maxq = max(pt1.y, pt2.y);

            // not disjoint
            return !(miny > maxq || maxy < minq);
        }

        ///Test for disjoint between two mbrs
        [[using gnu : const, always_inline, hot]]
        bool disjoint(const MBR<T> &other) const {
            return !intersects(other);
        }

        ///Computes the intersection of two bounding box
        std::optional<MBR<T>> intersection(const MBR<T> &other) const{
            if (disjoint(other)) {
                return std::nullopt;
            }
            auto minx_ = minx > other.minx ? minx : other.minx;
            auto miny_ = miny > other.miny ? miny : other.miny;
            auto maxx_ = maxx < other.maxx ? maxx : other.maxx;
            auto maxy_ = maxy < other.maxy ? maxy : other.maxy;
            return MBR<T>{minx_, miny_, maxx_, maxy_};
        }


        ///Expand include other bounding box
        MBR<T> &expand_to_include(const MBR<T> &other) {
            minx = min(other.minx, minx);
            miny = min(other.miny, miny);

            maxx = max(other.maxx, maxx);
            maxy = max(other.maxy, maxy);
            return *this;
        }


        ///Expand to include x,y
        MBR<T> &expand_to_include(T x, T y) {
            if (x < minx) {
                minx = x;
            }
            else if (x > maxx) {
                maxx = x;
            }

            if (y < miny) {
                miny = y;
            }
            else if (y > maxy) {
                maxy = y;
            }
            return *this;
        }

        ///Expand by delta in x and y
        MBR<T> &expand_by_delta(T dx, T dy) {
            auto minx_ = minx - dx, miny_ = miny - dy;
            auto maxx_ = maxx + dx, maxy_ = maxy + dy;

            minx = min(minx_, maxx_);
            miny = min(miny_, maxy_);
            maxx = max(minx_, maxx_);
            maxy = max(miny_, maxy_);

            return *this;
        }


        ///computes dx and dy for computing hypot
        Pt<T> distance_dxdy(const MBR<T> &other) const {
            T dx{0};
            T dy{0};

            // find closest edge by x
            if (maxx < other.minx) {
                dx = other.minx - maxx;
            }
            else if (minx > other.maxx) {
                dx = minx - other.maxx;
            }

            // find closest edge by y
            if (maxy < other.miny) {
                dy = other.miny - maxy;
            }
            else if (miny > other.maxy) {
                dy = miny - other.maxy;
            }

            return Pt<T>{dx, dy};
        }


        ///Distance computes the distance between two mbrs
        double distance(const MBR<T> &other) const {
            if (intersects(other)) {
                return 0.0;
            }
            auto o = distance_dxdy(other);
            return std::hypot(o.x, o.y);
        }

        ///distance square computes the squared distance
        ///between bounding boxes
        double distance_square(const MBR<T> &other) const {
            if (intersects(other)) {
                return 0.0;
            }
            auto o = distance_dxdy(other);
            return (o.x * o.x) + (o.y * o.y);
        }

        ///WKT : wkt string of mbr as polygon
        [[nodiscard]] std::string wkt() const {
            std::ostringstream ss;
            ss << "POLYGON (("
               << f2str(minx) << " " << f2str(miny) << ", "
               << f2str(minx) << " " << f2str(maxy) << ", "
               << f2str(maxx) << " " << f2str(maxy) << ", "
               << f2str(maxx) << " " << f2str(miny) << ", "
               << f2str(minx) << " " << f2str(miny)
               << "))";
            return ss.str();
        }

        ///Operator : + : Union
        MBR<T> operator+(const MBR<T> &other) const {
            return {
                    min(other.minx, minx),
                    min(other.miny, miny),
                    max(other.maxx, maxx),
                    max(other.maxy, maxy),
                    true
            };
        }

        ///Operator : | or + : Union
        MBR<T> operator|(const MBR<T> &other) const {
            return *this + other;
        }

        ///Operator : & : intersection
        std::optional<MBR<T>> operator&(const MBR<T> &other) const {
            return intersection(other);
        }

        ///Operator : equals
        inline bool operator==(const MBR<T> &other) {
            return equals(other);
        }

        ///Operator : not equal
        bool operator!=(const MBR<T> &other) {
            return !(this->equals(other));
        }

    private:
        [[nodiscard]] std::string f2str(double f, int precision = 12) const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(precision) << f;
            auto s = ss.str();
            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
            if (s.back() == '.') {
                s.pop_back();
            }
            return s;
        }

        template<typename U>
        U min(U a, U b) const {
            if constexpr (std::is_integral<T>::value) {
                return b < a ? b : a;
            }
            else {
                return std::fmin(a, b);
            };
        }

        template<typename U>
        U max(U a, U b) const {
            if constexpr (std::is_integral<T>::value) {
                return b > a ? b : a;
            }
            else {
                return std::fmax(a, b);
            };
        }


        template<typename U>
        bool eqls(U a, U b) const {
            if constexpr (std::is_integral<T>::value) {
                return a == b;
            }
            else {
                return feq(a, b);
            };
        }

    };
}
#endif //MBR_MBR_H
