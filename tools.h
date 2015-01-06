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
void walk_graph( auto po, const auto& pa, F && f )
{
    for (const auto & p : pa) {
        assert( p < po.size() );

        std::forward<F>(f)(po[p]);

        po.erase(po.begin() + p);
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


void check_path(auto po, auto pa)
{
    walk_graph(po, pa, [&](auto ){
        //nothing walk_graph do all the things
    });
}

template <class PathsType>
auto first_age_offspring(auto po, size_t count, size_t size)
{
    PathsType ret;

    for (size_t i = 0; i<count; ++i) {
        typename PathsType::value_type pa;

        while (pa.size() < size) {
            auto num = (rand() % (size - pa.size()));
            pa.push_back(num);
            check_path(po, pa);
        }
        ret.push_back(pa);
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
auto shortest_path_num_and_length( const PointsType& po, const PathsType & pa  )
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
    return std::make_pair(num, min_length);
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
auto simple_crossover(auto po, const PathType& p1, const PathType& p2 )
{

    assert( p1.size() == p2.size() );
    assert( p1.size() > 1 );
    PathType ret;

    auto split_point = rand() % (p1.size() - 1);
    while ( split_point == 0 )
        split_point = rand() % (p1.size() - 1);


    PathType genome1, genome2;

    genome1.insert( genome1.begin(), p1.begin(), p1.begin() + split_point );
    genome2.insert( genome2.begin(), p2.begin() + split_point, p2.end() );

//    std::cout << "----------------------" << std::endl;

//    print_path( genome1 );
//    print_path( genome2 );

    ret.insert( ret.end(), genome1.begin(), genome1.end() );

    ret.insert( ret.end(), genome2.begin(), genome2.end());

    //print_path(ret);

    assert (ret.size() == p1.size() );

    check_path(po, ret);

    return ret;
}

template <class PathsType>
PathsType crossover( auto po, const PathsType& pa, size_t needed_size  )
{
    PathsType ret;


    while ( ret.size() <  needed_size) {
        auto id1 = rand() % pa.size();
        auto id2 = id1;
        while ( id2 == id1 )
            id2 = rand() % pa.size();

        auto child = simple_crossover( po, pa[id1] , pa[id2] );
        ret.push_back(child);
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


void print_paths_with_best(const auto & po, const auto& pas)
{
    //print_paths(pas);
    size_t num, length;
    std::tie( num, length ) = shortest_path_num_and_length<decltype(po), decltype(pas)>( po, pas );
    std::cout << "The best is: " <<  num << " with length: "  << length << std::endl;
}


template <class PointsType, class PathsType>
PathsType fitness( const PointsType& po, PathsType ret, size_t count )
{


    for ( size_t i = 0; i<count; ++i  ) {
        auto id1 = rand() % ret.size();
        auto id2 = rand() % ret.size();

        auto p1 = ret[ id1 ];
        auto p2 = ret[ id2 ];

        auto len1 = calc_path_length( po, p1 );
        auto len2 = calc_path_length( po, p2 );

        if ( len1 < len2  ) {
            ret.erase( ret.begin() + id1 );
        }
        else if ( len1 > len2 ){
            ret.erase( ret.begin() + id2 );
        }
        if ( ret.size() <=2  )
            break;
    }

    for (const auto& p : ret)
        check_path(po, p);

    return ret;

}


#endif // TOOLS_H

