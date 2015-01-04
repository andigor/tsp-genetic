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

//merge shortest and longest

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


template <class PointsType, class PathsType>
auto longest_path( const PointsType& po, const PathsType & pa  )
{
    double max_length = std::numeric_limits<double>::min();

    assert(pa.size() > 0);
    typename PathsType::const_iterator max_path_iter = pa.begin();

    for ( auto iter = pa.begin(); iter != pa.end(); ++iter ) {
        auto cur_length = calc_path_length( po, *iter );
        if ( cur_length > max_length  ) {
            max_length = cur_length;
            max_path_iter = iter;
        }
    }
    return *max_path_iter;
}



template <class PathType>
auto simple_crossover( PathType p1, PathType p2 )
{
    assert( p1.size() == p2.size() );

    const size_t size = p1.size() / 2;
    size_t swaped_count = 0;
    while (swaped_count < size) {
        size_t val_pos = 0;
        bool found = false;
        for ( auto iter = p2.begin(); iter != p2.end(); ++iter ) {
            if ( *iter == p1[swaped_count] ) {
                found = true;
                std::swap( p1[swaped_count], *iter );
                ++swaped_count;
                break;
            }
            ++val_pos;
        }

        assert ( found );
    }

    return std::make_pair(p1, p2);
}

template <class PathsType>
PathsType crossover( PathsType ret  )
{
    std::reverse( ret.begin(), ret.end() );
    auto size = ret.size() / 2;
    for ( size_t i = 0; i<size; ++i ) {
        std::tie( ret[i], ret[i+size] ) = simple_crossover( ret[i] , ret[i + size] );
    }
    return ret;
}



template <class PointsType, class PathsType>
PathsType fitness( const PointsType& po, const PathsType& pa )
{
    PathsType ret;

    srand(321);

    while ( ret.size() < pa.size() ) {
        auto p1 = pa[ rand() % pa.size() ];
        auto p2 = pa[ rand() % pa.size() ];

        auto len1 = calc_path_length( po, p1 );
        auto len2 = calc_path_length( po, p2 );

        if ( len1 < len2  ) {
            ret.push_back( p1 );
        }
        else {
            ret.push_back( p2 );
        }
    }

    return ret;
}


#endif // TOOLS_H

