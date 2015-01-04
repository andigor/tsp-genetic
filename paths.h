#ifndef PATHS_H
#define PATHS_H

#include <vector>


class Path
{
public:
    Path();
    ~Path();
    void push_back ( const auto & arg  )
    {
        paths_.push_back(arg);
    }


    const auto begin() const
    {
        return paths_.begin();
    }
    const auto end() const
    {
        return paths_.end();
    }

private:
    std::vector<std::size_t> paths_;
};

using Paths = std::vector<Path>;


#endif // PATHS_H
