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
auto simple_crossover(auto po, const PathType& p1, const PathType& p2, const auto& mutation_probablilty )
{

    assert( p1.size() == p2.size() );
    assert( p1.size() > 1 );
    PathType ret;

    auto split_point = rand() % (p1.size() - 1);
    while ( split_point == 0 )
        split_point = rand() % (p1.size() - 1);


    PathType genome1_1, genome1_2, genome2_1, genome2_2;

    genome1_1.insert( genome1_1.begin(), p1.begin(), p1.begin() + split_point );
    genome1_2.insert( genome1_2.begin(), p1.begin() + split_point, p1.end()  );

    genome2_1.insert( genome2_1.begin(), p2.begin(), p2.begin() + split_point );
    genome2_2.insert( genome2_2.begin(), p2.begin() + split_point, p2.end()  );



//    std::cout << "----------------------" << std::endl;

//    print_path( genome1 );
//    print_path( genome2 );
    {
        PathType child1, child2;

        child1.insert( child1.end(), genome1_1.begin(), genome1_1.end() );
        child1.insert( child1.end(), genome2_2.begin(), genome2_2.end());

        child2.insert( child2.end(), genome2_1.begin(), genome2_1.end() );
        child2.insert( child2.end(), genome1_2.begin(), genome1_2.end());

        auto l1 = calc_path_length(po, child1);
        auto l2 = calc_path_length(po, child2);
        ret = l1 < l2 ? child1 : child2;
        assert (ret.size() == p1.size() );
        assert (ret.size() == p2.size() );
    }

    auto p1_length = calc_path_length( po, p1 );
    auto p2_length = calc_path_length( po, p2 );

    auto child_length = calc_path_length( po, ret );

    std::cout << "crossover p1: " << p1_length << " p2: " << p2_length << " child: " << child_length << std::endl;

    if ( child_length > p1_length && child_length > p2_length ) {
        std::cout << "degradation after crossover comparing to all parents!" << std::endl;
    }
    else if ( child_length > p1_length  ) {
        std::cout << "degradation after crossover comparing to first parent!" << std::endl;
    }
    else if ( child_length > p2_length  ) {
        std::cout << "degradation after crossover comparing to second parent!" << std::endl;
    }

    if ( rand() % 100 <= mutation_probablilty ) {
        //auto swap_count = rand() % ret.size();
        //for ( size_t i = 0 ; i<swap_count; ++i) {
            auto pos = rand() % ret.size();
            auto val = rand() % (ret.size() - pos);
            ret[pos] = val;
        //}
        auto l = calc_path_length( po, ret );
        std::cout << "length after mutation: " << l << std::endl;
    }

    //print_path(ret);



    check_path(po, ret);

    return ret;
}

template <class PathsType>
PathsType crossover( auto po, const PathsType& pa, size_t needed_size, auto mutation_perc_prob  )
{
    assert(needed_size > 0);

    PathsType ret;

    size_t num, len;
    std::tie(num, len) = shortest_path_num_and_length( po, pa );
    ret.push_back( pa[num] );

    while ( ret.size() <  needed_size) {
        auto id1 = rand() % pa.size();
        auto id2 = id1;
        while ( id2 == id1 )
            id2 = rand() % pa.size();

        auto child = simple_crossover( po, pa[id1] , pa[id2], mutation_perc_prob );
        ret.push_back(child);
    }
    return ret;
}



void print_path(const auto /*&po*/, const auto& pa)
{
    std::cout << "[";
    std::string sep;
    for ( auto p : pa ) {
        std::cout << sep << p;
        sep = " ";
    }
    std::cout << /*"]" << " " << calc_path_length(po, pa) << */std::endl;
}

void print_paths(const auto& po, const auto& pas)
{
    for (const auto pa : pas) {
        print_path(po, pa);
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
    std::cout << "start tournnament" << std::endl;
    for ( size_t i = 0; i<count; ++i  ) {
        if ( ret.size() <=2  ) {
            break;
        }

        auto id1 = rand() % ret.size();
        auto id2 = id1;
        while ( id2 == id1 )
            id2 = rand() % ret.size();

        auto p1 = ret[ id1 ];
        auto p2 = ret[ id2 ];

        print_path(po, p1);
        std::cout << "(" << id1 << ") vs (" << id2 << ")"<<std::endl;
        print_path(po, p2);

        auto len1 = calc_path_length( po, p1 );
        auto len2 = calc_path_length( po, p2 );

        if ( len1 > len2  ) {
            std::cout << "first (" << id1 << ") killed" << std::endl;
            ret.erase( ret.begin() + id1 );
        }
        else if ( len2 > len1 ){
            std::cout << "second (" << id2 << ") killed" << std::endl;
            ret.erase( ret.begin() + id2 );
        }
    }

    for (const auto& p : ret)
        check_path(po, p);

    std::cout << "after tournament " << ret.size() <<" left" << std::endl;

    return ret;

}


#endif // TOOLS_H

