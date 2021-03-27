#pragma once

#include <tuple>
#include <type_traits>

//
// Extract object type, return type, and parameters from member function
//

template<typename F>
struct member_fn_t;

template<typename R, typename C, typename... Args>
struct member_fn_t<R(C::*)(Args...)> {
   using arglist_t = std::tuple<Args...>;
   using cls_t = C;
   using ret_t = R;
};

//
// Apply a template template "function" to all types in a std::tuple
//

template<template<typename> class MF,
   typename Tuple>
   struct apply_to_tuple;

template<template<typename> class MF,
   typename... Elements>
   struct apply_to_tuple<MF, std::tuple<Elements...>> {
   using type = std::tuple<MF<Elements>...>;
};

//
// Filter types in a std::tuple by a template template "predicate"
//

template<template<typename> class Predicate,
   typename Sequence>
   struct filter;

template<template<typename> class Predicate,
   typename Head,
   typename... Elements>
   struct filter<Predicate, std::tuple<Head, Elements...>> {
   using type = std::conditional_t<Predicate<Head>::value,
      decltype(std::tuple_cat(std::declval<std::tuple<Head>>(),
         std::declval<typename filter<Predicate, std::tuple<Elements...>>::type>())),
      typename filter<Predicate, std::tuple<Elements...>>::type>;
};

// terminate recursion
template<template<typename> class Predicate>
struct filter<Predicate, std::tuple<>> {
   using type = std::tuple<>;
};
