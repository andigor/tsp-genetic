#ifndef TOOLS_H
#define TOOLS_H


#include <vector>
#include <iostream>

#include <cassert>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <type_traits>

#include <QDebug>
#include <QtConcurrent/QtConcurrent>

inline auto my_rand()
{
    static thread_local drand48_data d;
    static thread_local bool initialized;
    if (!initialized) {
        srand48_r(123, &d);
        initialized = 1;
    }
    long int value;
    lrand48_r( &d, &value );

    return value;
}


//should be replaced with iterators
template <class F>
void walk_graph( auto po, const auto& pa, F && f )
{
    for (const auto & p : pa) {
        assert( p < po.size() );

        std::forward<F>(f)(po[p]);
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
        auto new_x = my_rand() % max_x;
        auto new_y = my_rand() % max_y;

        auto iter = std::find_if( ret.begin(), ret.end(), [&](const auto& p){
             return ( p.x() == new_x && p.y() == new_y); } );

        if ( iter == ret.end() ) {
            ret.push_back( Point(new_x, new_y) );
        }
    }

    return ret;
}


void check_path_abs(const auto& po, auto pa)
{
    return; //for speed

    for ( const auto& p:pa ) {
        assert( p < po.size() );
    }
    //check for duplicates
    std::sort(pa.begin(), pa.end() );
    auto end = std::unique( pa.begin(), pa.end() );
    if ( end != pa.end() ) {
        for ( auto p : pa  ) {
            std::cout << p << " ";
        }
        std::cout << std::endl;
        assert(false);
    }
}

template <class PathsType>
auto first_age_offspring(auto po, size_t count, size_t size)
{
    PathsType ret;

    for (size_t i = 0; i<count; ++i) {
        typename PathsType::value_type pa;

        while (pa.size() < size) {
            auto num = my_rand() % size;
            auto iter = std::find( pa.begin(), pa.end(), num );
            if ( iter == pa.end() ) {
                pa.push_back(num);
                check_path_abs(po, pa);
            }
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
    if ( my_rand() % 100 < mutation_probablilty ) {
//        auto l1 = calc_path_length( po, ret );
        //auto swap_count = my_rand() % ret.size();
        //for ( size_t i = 0 ; i<swap_count; ++i) {


        auto pos1 = my_rand() % ret.size();
        auto pos2 = pos1;
        while ( pos2 == pos1 )
            pos2 = my_rand()  % ret.size();

        std::swap( ret[pos1], ret[pos2] );

        check_path_abs(po, ret);
        //}
    }
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

    auto pairs1 = generate_pairs_from_abs( p1 );
    auto pairs2 = generate_pairs_from_abs( p2 );


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
        auto num = my_rand() % p1.size();
        if ( std::find(ret.begin(), ret.end(), num) == ret.end())
            ret.push_back(num);
    }






    assert( ret.size() == p1.size() );
    assert( ret.size() == p2.size() );


    check_path_abs(po, ret);
//   {
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

    auto func = [&](auto sz){
        PathsType p;
        while ( p.size() < sz ) {
            auto id1 = my_rand() % pa.size();
            auto id2 = id1;
            while ( id2 == id1 )
                id2 = my_rand() % pa.size();

            auto child = greedy_crossover2( po, pa[id1] , pa[id2], mutation_perc_prob );

            p.push_back(child);
        }
        return p;
    };

    auto add = func(needed_size);
    ret.insert( ret.end(), add.begin(), add.end());

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

        auto id1 = my_rand() % ret.size();
        auto id2 = id1;
        while ( id2 == id1 )
            id2 = my_rand() % ret.size();

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
        check_path_abs(po, p);

    //std::cout << "after tournament " << ret.size() <<" left" << std::endl;

    return ret;

}


#endif // TOOLS_H

