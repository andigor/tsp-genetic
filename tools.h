#ifndef TOOLS_H
#define TOOLS_H


#include <vector>
#include <iostream>

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

    while ( ret.size() < count ) {
        auto new_x = rand() % max_x;
        auto new_y = rand() % max_y;

        auto iter = std::find_if( ret.begin(), ret.end(), [&](const auto& p){
             return ( p.x() == new_x && p.y() == new_y); } );

        if ( iter == ret.end() ) {
            ret.push_back( Point(new_x, new_y) );
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
auto shortest_path_num( const PointsType& po, const PathsType & pa  )
{
    double min_length = std::numeric_limits<double>::max();

    assert(pa.size() > 0);
    size_t num = 0;

    for ( size_t i = 0; i < pa.size(); ++i ) {
        auto cur_length = calc_path_length( po, pa[i] );
        if ( cur_length < min_length  ) {
            min_length = cur_length;
            num = i;
        }
    }
    return num;
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
auto simple_crossover(const PathType& p1, const PathType& p2 )
{
    srand(time(NULL));
    assert( p1.size() == p2.size() );
    assert( p1.size() > 1 );
    PathType ret;


    using GensType = PathType;

    while ( p1.size() > 1  ) {
      GensType g(2);
      std::copy(p1.begin(), p1.begin() + 1, g.begin()  );

    }

    return ret;
}

template <class PathsType>
PathsType crossover( const PathsType& pa, size_t size  )
{
    PathsType ret;
    srand(time(NULL));

    for ( size_t i = 0; i<size; ++i ) {
        auto id1 = rand() % ret.size();
        auto id2 = id1;
        while ( id2 == id1 )
            id2 = rand() % ret.size();

        std::tie( ret[id1], ret[id2] ) = simple_crossover( ret[i] , ret[i + size] );
    }
    return ret;
}



void print_path(const auto& pa)
{
    std::cout << "[";
    std::string sep;
    for ( auto p : pa ) {
        std::cout << sep << p;
        sep = " ";
    }
    std::cout << "]" << std::endl;
}

void print_paths(const auto& pas)
{
    for (const auto pa : pas) {
        print_path(pa);
    }
}


void print_paths_best(const auto & po, const auto& pas)
{
    print_paths(pas);
    std::cout << "The best is: " << shortest_path_num<decltype(po), decltype(pas)>( po, pas ) << std::endl;
}


template <class PointsType, class PathsType>
PathsType fitness( const PointsType& po, PathsType ret, size_t rounds )
{
    srand(time(NULL));

    for ( size_t i = 0; i<rounds; ++i  ) {
        auto id1 = rand() % ret.size();
        auto id2 = rand() % ret.size();

        auto p1 = pa[ id1 ];
        auto p2 = pa[ id2 ];

        auto len1 = calc_path_length( po, p1 );
        auto len2 = calc_path_length( po, p2 );

        if ( len1 < len2  ) {
            ret.erase( ret.begin() + id1 );
        }
        else if ( len1 > len2 ){
            ret.erase( ret.begin() + id2 );
        }
    }
    return ret;

}


#endif // TOOLS_H

