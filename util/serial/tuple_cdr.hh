#include <tuple>

// the key is that each function returns type 'Target'; as opposed to returning
// types representing an intermediate value (based on the current Tail or
// something)

// credit to
// Crazy Eddie @ http://stackoverflow.com/questions/10626856/how-to-split-a-tuple
template < typename Target, typename Tuple, int N, bool end >
struct builder
{
    template < typename ... Args >
    static Target
    create(Tuple &t, Args && ... args)
    {
        return builder<Target,Tuple, N+1, std::tuple_size<Tuple>::value == N+1>::
            create(t, std::forward<Args>(args)..., std::move(std::get<N>(t)));
    }
};

template < typename Target, typename Tuple, int N >
struct builder<Target,Tuple,N,true>
{
    template < typename ... Args >
    static Target
    create(Tuple &, Args && ... args)
    { 
        return Target(std::forward<Args>(args)...);
    }
};

template < typename Head, typename ... Tail >
std::tuple<Tail...>
tuple_cdr(std::tuple<Head,Tail...> &tpl)
{
    return builder<std::tuple<Tail...>, std::tuple<Head,Tail...>, 1,
            std::tuple_size<std::tuple<Head,Tail...>>::value == 1>::create(tpl);
}


