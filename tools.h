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





//should be replaced with iterators
template <class F>
void walk_graph( auto po, const auto& pa, F && f )
{
    for (const auto & p : pa) {
        assert( p < po.size() );

        std::forward<F>(f)(po[p]);

        po.erase(po.begin() + p);
    }
}


template <class F>
void walk_two_graphs( const auto& po, const auto& pa1, const auto& pa2, F && f )
{
    assert ( pa1.size() == pa2.size() );

    auto po1 = po;
    auto po2 = po;

    auto iter1 = pa1.begin();
    auto iter2 = pa2.begin();

    while ( iter1 != pa1.end() && iter2 != pa2.end()) {
        assert( *iter1 < po1.size() );
        assert( *iter2 < po2.size() );

        std::forward<F>(f)(po1[*iter1], *iter1, po2[*iter2], *iter2);

        po1.erase(po1.begin() + *iter1);
        po2.erase(po2.begin() + *iter2);
        ++iter1;
        ++iter2;
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
PathType mutate(const auto& po, PathType ret, const auto& mutation_probablilty )
{
    if ( rand() % 100 < mutation_probablilty ) {
        auto l1 = calc_path_length( po, ret );
        //auto swap_count = rand() % ret.size();
        //for ( size_t i = 0 ; i<swap_count; ++i) {
            auto pos = rand() % ret.size();
            auto val = rand() % (ret.size() - pos);
            ret[pos] = val;
        //}
        auto l2 = calc_path_length( po, ret );
        //std::cout << "mutation before: " << l1 << " after: " << l2 << std::endl;
        if (  l2 < l1 ) {
            //std::cout << "Good mutation!!! before: " << l1 << " after " << l2 << std::endl;
        }
    }
    return ret;
}

template <class PathType>
auto simple_crossover(const auto& po, const PathType& p1, const PathType& p2, const auto& mutation_probablilty )
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

//    auto p1_length = calc_path_length( po, p1 );
//    auto p2_length = calc_path_length( po, p2 );

//    auto child_length = calc_path_length( po, ret );

    //std::cout << "crossover p1: " << p1_length << " p2: " << p2_length << " child: " << child_length << std::endl;

//    if ( child_length > p1_length && child_length > p2_length ) {
//        std::cout << "degradation after crossover comparing to all parents!" << std::endl;
//    }
//    else if ( child_length > p1_length  ) {
//        std::cout << "degradation after crossover comparing to first parent!" << std::endl;
//    }
//    else if ( child_length > p2_length  ) {
//        std::cout << "degradation after crossover comparing to second parent!" << std::endl;
//    }
//    else {

//    }

    ret = mutate(po, ret, mutation_probablilty);
    //print_path(ret);



    check_path(po, ret);

    return ret;
}

template <class PointsType, class PathType>
auto greedy_crossover(const PointsType& po, const PathType& p1, const PathType& p2, const auto& mutation_probablilty )
{

    assert( p1.size() == p2.size() );
    assert( p1.size() > 1 );
    PathType ret;

    typename PointsType::PointType prev_po1, prev_po2;
    bool first_time = true;
    walk_two_graphs(po, p1, p2, [&](const auto& po1, const auto num1, const auto& po2, const auto& num2){
        if ( first_time ) {
            first_time = false;
            ret.push_back(num1);
        }
        else {
            auto l1 = points_distance( po1, prev_po1 );
            auto l2 = points_distance( po2, prev_po2 );

            if ( l1 < l2 ) {
                ret.push_back( num1 );
            }
            else {
                ret.push_back( num2 );
            }
        }
        prev_po1 = po1;
        prev_po2 = po2;
    });
    assert( ret.size() == p1.size() );
    assert( ret.size() == p2.size() );

    check_path(po, ret);

//    auto p1_length = calc_path_length(po, p1);
//    auto p2_length = calc_path_length(po, p2);
//    auto child_length = calc_path_length(po, ret);

//    if ( child_length > p1_length && child_length > p2_length ) {
//        std::cout << "degradation 1 2-> p1 " << p1_length << " p2 " << p2_length << " child "  << child_length << std::endl;
//    }
//    else if ( child_length > p1_length  ) {
//        std::cout << "degradation 1  -> p1 "<< p1_length << " p2 " << p2_length << " child "  << child_length << std::endl;
//    }
//    else if ( child_length > p2_length  ) {
//        std::cout << "degradation 2  -> p1 "<< p1_length << " p2 " << p2_length << " child "  << child_length << std::endl;
//    }
//    else {
//        std::cout << "good crossover -> p1 " << p1_length << " p2 " << p2_length << " child "  << child_length << std::endl;
//    }

    ret = mutate(po, ret, mutation_probablilty);

    return ret;
}



void check_path_abs(const auto& po, auto pa)
{
    for ( const auto& p:pa ) {
        assert( p < po.size() );
    }
    //check for duplicates
    std::sort(pa.begin(), pa.end() );
    auto end = std::unique( pa.begin(), pa.end() );
    if ( end != pa.end() ) {
//        for ( auto p : pa  ) {
//            std::cout << p << " ";
//        }
//        std::cout << std::endl;
        assert(false);
    }
}

template <class PathType>
PathType rel_to_abs(auto po,const PathType& pa)
{
    PathType ret;

    std::vector<typename PathType::ValueType> map;
    for ( size_t i = 0; i<pa.size(); ++i ) {
        map.push_back(i);
    }

    for ( size_t i = 0; i<pa.size(); ++i ) {
        assert( pa[i] < map.size() );
        ret.push_back( map[pa[i]] );
        map.erase(map.begin() + pa[i]);
    }

    check_path_abs(po, ret);

    return ret;
}

template <class PathType>
PathType abs_to_rel(auto po, const PathType& pa)
{
    std::vector<typename PathType::ValueType> map;
    for ( size_t i = 0; i<pa.size(); ++i  ) {
        map.push_back(i);
    }

    PathType ret;

    for ( size_t i = 0; i<pa.size(); ++i ) {
        assert( map[pa[i]] != std::numeric_limits<size_t>::max() );
        auto rel_num = map[pa[i]];
        ret.push_back(rel_num);
        map[pa[i]] = std::numeric_limits<size_t>::max();
        size_t counter = 0;
        for ( size_t j = 0; j<pa.size();++j ) {
            if ( map[j] != std::numeric_limits<size_t>::max() ) {
                map[j] = counter++;
            }
        }
    }

    check_path( po, ret );

    return ret;
}

template<class PathType>
auto generate_pairs_from_abs( const PathType& pa )
{
    assert(pa.size( ) > 1);

    std::vector<std::pair<typename PathType::ValueType, typename PathType::ValueType>> ret;
    auto iter = pa.begin();
    auto first = *iter;
    ++iter;
    for (  ;iter != pa.end(); ++iter ) {
        auto second = *iter;
        ret.push_back( std::make_pair( first, second )  );
        first = second;
    }

    return ret;
}


template <class PointsType, class PathType>
auto greedy_crossover2(const PointsType& po, const PathType& p1, const PathType& p2, const auto& mutation_probablilty )
{
    assert( p1.size() == p2.size() );
    assert( p1.size() > 1 );
    PathType ret;

    PathType p1_abs = rel_to_abs(po, p1);
    PathType p2_abs = rel_to_abs(po, p2);

//    std::cout<<"before paring " << std::endl;
//    for ( auto p: p1_abs ) {
//        std::cout << p <<" ";
//    }
//    std::cout << std::endl;


//    for ( auto p: p2_abs ) {
//        std::cout << p <<" ";
//    }
//    std::cout << std::endl;

    auto pairs1 = generate_pairs_from_abs( p1_abs );
    auto pairs2 = generate_pairs_from_abs( p2_abs );


//    {
//        std::cout << "before paring" << std::endl;
//        for ( auto p: pairs1 ) {
//            std::cout << "(" << p.first << " " << p.second << ") ";
//        }
//        std::cout << std::endl;
//        for ( auto p: pairs2 ) {
//            std::cout << "(" << p.first << " " << p.second << ") ";
//        }
//        std::cout << std::endl;

//    }


/*    std::sort( pairs1.begin(), pairs1.end(), [](const auto& p1, const auto& p2){
        return p1.first < p2.first;
    }  );

    std::sort( pairs2.begin(), pairs2.end(), [](const auto& p1, const auto& p2){
        return p1.first < p2.first;
    }  );*/




    decltype(pairs1) best_pairs;

    for ( const auto & p : pairs1 ) {
        auto iter = std::find_if( pairs2.begin(), pairs2.end(), [&](const auto& arg){
            return arg.first == p.first;
        }  );
        if ( iter != pairs2.end() )    {
            auto l1 = points_distance(po[p.first], po[p.second]);
            auto l2 = points_distance(po[iter->first], po[iter->second]);
            if (l1 < l2) {
                best_pairs.push_back( p );
            }
            else /*if( l2 < l1 ) */{
                best_pairs.push_back( *iter );
            }
        }
        else {

        }
    }

    //std::cout << "best pairs: " << std::endl;
//    std::cout << "taken pairs: ";
    for ( const auto& bp : best_pairs  ) {
        //std::cout << "(" << bp.first << " " << bp.second << ") " ;

        if ( std::find(ret.begin(), ret.end(), bp.first ) == ret.end() &&
             std::find(ret.begin(), ret.end(), bp.second ) == ret.end() ) {
            ret.push_back( bp.first );
            ret.push_back( bp.second );
//            std::cout << "(" << bp.first << " " << bp.second << ") ";
        }
    }
//    std::cout << std::endl;

    while ( ret.size() < p1.size() ) {
        auto num = rand() % p1.size();
        if ( std::find(ret.begin(), ret.end(), num) == ret.end())
            ret.push_back(num);
    }

//    {
//        std::cout << "after paring" << std::endl;
//        for ( auto p: pairs1 ) {
//            std::cout << "(" << p.first << " " << p.second << ") ";
//        }
//        std::cout << std::endl;
//        for ( auto p: pairs2 ) {
//            std::cout << "(" << p.first << " " << p.second << ") ";
//        }
//        std::cout << std::endl;

//    }


//    std::cout << "child before to rel: " << std::endl;
//    for ( const auto & r: ret) {
//        std::cout << r << " ";
//    }
//    std::cout << std::endl << " po size:" << po.size() << std::endl;



    ret = abs_to_rel(po, ret);

//    std::cout << "child after to rel: " << std::endl;
//    for ( const auto & r: ret) {
//        std::cout << r << " ";
//    }
//    std::cout << std::endl << " po size:" << po.size() << std::endl;


//    print_path(po, ret);


//    auto ret1 = ret;
//    ret1 = rel_to_abs(po, ret1);
//    ret1 = abs_to_rel(po, ret1);

//    for ( size_t i = 0; i<ret.size();++i   ) {
//        assert( ret[i] == ret1[i] );
//    }


    //std::cout << std::endl;



    assert( ret.size() == p1.size() );
    assert( ret.size() == p2.size() );


    check_path(po, ret);
//    {
//        auto p1_length = calc_path_length(po, p1);
//        auto p2_length = calc_path_length(po, p2);
//        auto child_length = calc_path_length(po, ret);

//        if ( child_length > p1_length && child_length > p2_length ) {
//            std::cout << "degradation 1 2-> p1 " << p1_length << " p2 " << p2_length << " child "  << child_length << std::endl;
//        }
//        else if ( child_length > p1_length  ) {
//            std::cout << "degradation 1  -> p1 "<< p1_length << " p2 " << p2_length << " child "  << child_length << std::endl;
//        }
//        else if ( child_length > p2_length  ) {
//            std::cout << "degradation 2  -> p1 "<< p1_length << " p2 " << p2_length << " child "  << child_length << std::endl;
//        }
//        else {
//            std::cout << "good crossover -> p1 " << p1_length << " p2 " << p2_length << " child "  << child_length << std::endl;
//        }
//    }
    ret = mutate(po, ret, mutation_probablilty);

    return ret;
}




template <class PathsType>
PathsType crossover(const auto& po, const PathsType& pa, size_t needed_size, auto mutation_perc_prob  )
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

        auto child = greedy_crossover2( po, pa[id1] , pa[id2], mutation_perc_prob );
        ret.push_back(child);
    }
    return ret;
}



void print_path(const auto &po, const auto& pa)
{
    std::cout << "[";
    std::string sep;
    for ( auto p : pa ) {
        std::cout << sep << p;
        sep = " ";
    }
    std::cout << "]" << " " << calc_path_length(po, pa) << std::endl;
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
    //std::cout << "start tournnament" << std::endl;
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

        //print_path(po, p1);
        //std::cout << "(" << id1 << ") vs (" << id2 << ")"<<std::endl;
        //print_path(po, p2);

        auto len1 = calc_path_length( po, p1 );
        auto len2 = calc_path_length( po, p2 );

        if ( len1 > len2  ) {
            //std::cout << "first (" << id1 << ") killed" << std::endl;
            //std::cout << "killed " << len1 << std::endl;
            ret.erase( ret.begin() + id1 );
        }
        else if ( len2 > len1 ){
            //std::cout << "second (" << id2 << ") killed" << std::endl;
            //std::cout << "killed " << len2 << std::endl;
            ret.erase( ret.begin() + id2 );
        }
    }

    for (const auto& p : ret)
        check_path(po, p);

    //std::cout << "after tournament " << ret.size() <<" left" << std::endl;

    return ret;

}


#endif // TOOLS_H

