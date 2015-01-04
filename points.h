#ifndef POINTS_H
#define POINTS_H

#include <QPoint>
#include <vector>

#include <cstdlib>
#include <ctime>

class Points
{
public:
    using PointType = QPoint;

    Points();
    ~Points();



    const auto begin() const
    {
        return points_.begin();
    }

    const auto end() const
    {
        return points_.end();
    }

    auto size() const
    {
        return points_.size();
    }

    const PointType& operator[] ( const auto & arg  ) const
    {
        return points_[arg];
    }



    void push_back ( const auto & arg  )
    {
        points_.push_back(arg);
    }

private:
    std::vector<QPoint> points_;

};





#endif // POINTS_H
