#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <stdexcept>
#include <utility>

template <class T>
class Repository
{
private:
    std::vector<T> items;

public:
    void Add(T item)
    {
        items.push_back(std::move(item));
    }

    void Remove(int index)
    {
        CheckIndex(index);
        items.erase(items.begin() + index);
    }

    void Update(int index, T item)
    {
        CheckIndex(index);
        items[index] = std::move(item);
    }

    int Size() const
    {
        return static_cast<int>(items.size());
    }

    std::vector<T> GetAll() const
    {
        return items;
    }

    void Clear()
    {
        items.clear();
    }

private:
    void CheckIndex(int index) const
    {
        if (index < 0 || index >= Size())
        {
            throw std::out_of_range("repository index out of range");
        }
    }
};

#endif
