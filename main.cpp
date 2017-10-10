#include <boost/type_index.hpp>
#include <iostream>
#include <tuple>
#include <vector>

template <typename Tuple, std::size_t N>
auto unroll( typename std::enable_if<N == 0, Tuple>::type T ) {
  return std::tuple_cat( std::get<0>( T ) );
}
template <typename Tuple, std::size_t N>
auto unroll( typename std::enable_if<N != 0, Tuple>::type T ) {
  return std::tuple_cat( unroll<Tuple, N - 1>( T ), std::get<N>( T ) );
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
  using boost::typeindex::type_id_with_cvr;
  std::tuple<std::tuple<double, float>, std::tuple<int>> r( std::tuple<double, float>( 3.14, 2.f * 3.14f ), ( 42 ) );
  auto unrolled = unroll( r );
  helperclass<decltype( r )> instance( r );

  std::cout << "r is           " << type_id_with_cvr<decltype( r        )>().pretty_name() << std::endl;
  std::cout << "unrolled is    " << type_id_with_cvr<decltype( unrolled )>().pretty_name() << std::endl;
  helperclass<decltype( r )>::FlatTuple t( 3.1, 2.2f, 23 );

  return 0;
}
