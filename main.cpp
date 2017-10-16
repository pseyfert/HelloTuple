#include <boost/type_index.hpp>
#include <iostream>
#include <tuple>
#include <vector>

#include <functional>
#include <type_traits>
#include <utility>

using boost::typeindex::type_id_with_cvr;

// https://stackoverflow.com/questions/13101061/detect-if-a-type-is-a-stdtuple
// https://bitbucket.org/martinhofernandes/wheels (CC0)
template <typename T>
using Invoke = typename T::type;

//! Meta-boolean type with parameters for when dependent contexts are needed
template <bool B, typename...>
struct dependent_bool_type : std::integral_constant<bool, B> {};

//! Boolean integral_constant alias
template <bool B, typename... T>
using Bool = Invoke<dependent_bool_type<B, T...>>;

template <typename T, template <typename...> class Template>
struct is_specialization_of : Bool<false> {};
template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : Bool<true> {};

template <typename T>
T stripoff( T&& t ) {
  return t;
}

template <typename Tuple, std::size_t N>
auto unroll(
    typename std::enable_if<
        N == 0 && is_specialization_of<decltype( stripoff( std::get<0>( std::declval<Tuple>() ) ) ), std::tuple>::value,
        Tuple>::type T ) {
  auto retval = std::tuple_cat( std::get<0>( T ) );
  return retval;
}

template <typename Tuple, std::size_t N>
auto unroll( typename std::enable_if<
             N == 0 &&
                 !is_specialization_of<decltype( stripoff( std::get<0>( std::declval<Tuple>() ) ) ), std::tuple>::value,
             Tuple>::type T ) {
  return std::tuple_cat( std::make_tuple( std::get<0>( T ) ) );
}

template <typename Tuple, std::size_t N>
auto unroll(
    typename std::enable_if<
        N != 0 && is_specialization_of<decltype( stripoff( std::get<N>( std::declval<Tuple>() ) ) ), std::tuple>::value,
        Tuple>::type T ) {
  auto retval = std::tuple_cat( unroll<Tuple, N - 1>( T ), std::get<N>( T ) );
  return retval;
}

template <typename Tuple, std::size_t N>
auto unroll( typename std::enable_if<
             N != 0 &&
                 !is_specialization_of<decltype( stripoff( std::get<N>( std::declval<Tuple>() ) ) ), std::tuple>::value,
             Tuple>::type T ) {
  auto retval = std::tuple_cat( unroll<Tuple, N - 1>( T ), std::make_tuple( std::get<N>( T ) ) );
  return retval;
}

template <typename Tuple>
auto unroll( Tuple T ) {
  return unroll<Tuple, std::tuple_size<Tuple>::value - 1>( T );
}

template <typename DeepTuple>
class helperclass {
 public:
  using FlatTuple = decltype( unroll( std::declval<DeepTuple>() ) );
  helperclass( DeepTuple D ) { m_holder.push_back( unroll( D ) ); }

 private:
  std::vector<FlatTuple> m_holder;
};

int main() {
  std::tuple<std::tuple<double, float>, std::tuple<int>> r( std::tuple<double, float>( 3.14, 2.f * 3.14f ), ( 42 ) );
  auto unrolled = unroll( r );
  helperclass<decltype( r )> instance( r );

  std::cout << "r is           " << type_id_with_cvr<decltype( r )>().pretty_name() << std::endl;
  std::cout << "unrolled is    " << type_id_with_cvr<decltype( unrolled )>().pretty_name() << std::endl;
  helperclass<decltype( r )>::FlatTuple t( 3.1, 2.2f, 23 );
  std::tuple<float, float> s( 2.2f, 3.3f );
  helperclass<decltype( s )>::FlatTuple u( 3.1f, 2.2f );
  //  helperclass<std::tuple<float,float>>::FlatTuple s( 3.1, 2.2f );

  return 0;
}
