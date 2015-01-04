#ifndef TOOLS_H
#define TOOLS_H


#include <vector>


//#include "points.h"
//#include "paths.h"

#include <cassert>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <type_traits>

#include <QDebug>

template <class F>
void walk_graph( const auto& po, const auto& pa, F && f )
{
    for (const auto & p : pa) {
        assert( p < po.size() );

        std::forward<F>(f)(po[p]);
    }
}

template <class PointsType>
auto generate_points(size_t count, auto max_x, auto max_y)
{

    using Point =  typename PointsType::PointType;
    PointsType ret;

    std::srand(std::time(0));

    size_t i = 0;

    while ( i < count ) {
        auto new_x = rand() % max_x;
        auto new_y = rand() % max_y;

        auto iter = std::find_if( ret.begin(), ret.end(), [&](const auto& p){
             return ( p.x() == new_x && p.y() == new_y); } );

        if ( iter == ret.end() ) {
            ret.push_back( Point(new_x, new_y) );
            ++i;
        }

    }

    return ret;
}

template <class PathsType>
auto first_age_offspring(size_t count, size_t size)
{
    PathsType ret;

    srand(123);

    for (size_t i = 0; i<count; ++i) {
        typename PathsType::value_type pa;

        size_t j = 0;
        while (j < size) {
            auto num = rand() % size;
            if ( std::find( pa.begin(), pa.end(), num ) == pa.end()  ) {
                pa.push_back(num);
                ++j;
            }
        }

        ret.push_back( pa );
    }

    return ret;
}

inline double points_distance( const auto & p1, const auto& p2 )
{
    auto xx = p1.x() - p2.x();
    auto yy = p1.y() - p2.y();
    return sqrt(pow(xx, 2) + pow(yy, 2));
}

template <class PointsType, class PathsType>
double calc_path_length( const PointsType & po, const PathsType & pa )
{
    double ret = 0.0;
    using PointType =  typename PointsType::PointType;
    PointType prev_point;

    bool first_time = false;
    walk_graph( po, pa, [&]( const auto & p ){
        if (first_time) {
            first_time = false;
            prev_point = p;
        }
        else {
            ret += points_distance( p, prev_point );
            prev_point = p;
        }
    } );

    return ret;
}

template <class PointsType, class PathsType>
auto shortest_path( const PointsType& po, const PathsType & pa  )
{
    double min_length = std::numeric_limits<double>::max();

    assert(pa.size() > 0);
    typename PathsType::const_iterator min_path_iter = pa.begin();

    for ( auto iter = pa.begin(); iter != pa.end(); ++iter ) {
        auto cur_length = calc_path_length( po, *iter );
        if ( cur_length < min_length  ) {
            min_length = cur_length;
            min_path_iter = iter;
        }
    }
    return *min_path_iter;
}





#endif // TOOLS_H

