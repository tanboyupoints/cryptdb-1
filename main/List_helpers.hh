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

template <typename Type> List<Type>
accumList(List_iterator<Type> it,
          std::function<List<Type>(List<Type>, Type *)> op)
{
    List<Type> accum;

    for (Type *element = it++; element ; element = it++) {
        accum = op(accum, element);
    }

    return accum;
}

template <typename T> List<T> *
vectorToList(std::vector<T*> v) {
    List<T> *const lst = new (current_thd->mem_root) List<T>;
    for (auto it : v) {
        lst->push_back(it);
    }

    return lst;
}

template <typename T, typename F> List<T>
filterList(List_iterator<T> it, F op) {
    List<T> new_list;
    T* element = it++;

    for (; element ; element = it++) {
        if (true == op(element)) {
            new_list.push_back(element);
        }
    }

    return new_list;
}
