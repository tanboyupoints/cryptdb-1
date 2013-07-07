#include <vector>

#include <sql_list.h>

// Helper functions for doing functional things to List<T> structures.
// MySQL may have already implemented these somewhere?
template <typename T, typename F> void
eachList(List_iterator<T> it, F op) {
    T* element = it++;
    for (; element ; element = it++) {
        op(element);
    }

    return;
}

template <typename T, typename F> List<T>
mapList(List_iterator<T> it, F op) {
    List<T> newList;
    T* element = it++;
    for (; element ; element = it++) {
        newList.push_back(op(element));
    }

    return newList;
}

// A bit off.
template <typename T, typename F, typename O> O
reduceList(List_iterator<T> it, O init, F op) {
    List<T> newList;
    O accum = init;
    T* element = it++;

    for (; element ; element = it++) {
        accum = op(accum, element);
    }

    return accum;
}

template <typename T> List<T>
vectorToList(std::vector<T*> v) {
    List<T> lst;
    for (auto it : v) {
        lst.push_back(it);
    }

    return lst;
}

