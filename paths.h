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

    template <class ... T>
    auto insert ( T&& ...  t)
    {
        return paths_.insert(std::forward<T>(t)...);
    }

    auto begin()
    {
        return paths_.begin();
    }
    auto end()
    {
        return paths_.end();
    }


    auto begin() const
    {
        return paths_.begin();
    }
    const auto end() const
    {
        return paths_.end();
    }
    auto size () const
    {
        return paths_.size();
    }

    std::size_t& operator[] ( const auto & arg  )
    {
        return paths_[arg];
    }

    const std::size_t& operator[] ( const auto & arg  ) const
    {
        return paths_[arg];
    }
private:
    std::vector<std::size_t> paths_;
};

using Paths = std::vector<Path>;


#endif // PATHS_H
