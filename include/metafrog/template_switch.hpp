#ifndef METAFROG_TEMPLATE_SWITCH_HEADER
#define METAFROG_TEMPLATE_SWITCH_HEADER

// TODO: Can we get rid of the MPL dependency? Use tuples?
// Only needed for iterating the list of cases.
#include <boost/mpl/vector.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>

#include <utility>
#include <type_traits>
#include <exception>

namespace metafrog {

class unknown_case : std::exception {
public:
  using std::exception::exception;
  const char* what() const noexcept {
    return "Unknown case was encountered in an instance "
            "of metafrog::template_switch";
  }
};

template<typename SubType, typename ReturnType, typename CaseType>
struct template_switch {
public: // types

  typedef SubType sub_type;
  typedef ReturnType return_type;
  typedef CaseType case_type;

  template<case_type CaseV>
  struct case_ {
    static const case_type value = CaseV;
  };

  template <case_type... Cases>
  struct cases_ {
    typedef boost::mpl::vector< case_<Cases>... > type;
  };

private: // Detail

  template <typename T> class is_variadic
  {
    template <typename C> static bool test(
      const std::type_info &x __attribute__ ((unused))
          = typeid(C::variadic) );
    template <typename C> static long test(...);

    public:
    // TODO: Also check whether T::variadic == true
    enum { value = sizeof(test<T>(0)) == sizeof(char) };
  };


public: // Defaults for users

  /// Whether when and otherwise are variadic.
  /// Can be overwritten.
  /// If true, the when<...>::run and otherwise functions
  /// will be passed the expected return type and the
  /// parameters given as variadic template parameters.
  static const bool variadic = is_variadic<sub_type>::value;
  
  /// Default case compare. May be overwritten
  static inline bool compare(case_type a, case_type b) {
    return a == b;
  }

  /// Default otherwise clause. This will simply raise an
  /// exception if an unknown case is encountered.
  template<typename... Args>
  static case_type otherwise(...) {
    throw unknown_case();
  }

public: // call operator

  template<typename... Args>
  inline return_type operator()(case_type&& data, Args&&... args) {
    return match<
          boost::mpl::empty<typename sub_type::cases>::value
        , typename sub_type::cases
        , Args...
      >::run(
          std::forward<case_type>( data )
        , std::forward<Args>(args)... );
  }

private: // Detail: Implementation
  
  struct variadic_proxy {
    template <typename... Args>
    static inline return_type otherwise(case_type&& data, Args&&... args) {
      return sub_type::template otherwise<Args...>(
            std::forward( data )
          , std::forward<Args>(args)... );
    }

    template <case_type Data, typename... Args>
    static inline return_type when(Args&&... args) {
      return sub_type::template then< Data, Args...>( std::forward<Args>(args)... );
    }
  };

  struct static_proxy {
    template <typename... Args>
    static inline return_type otherwise(case_type&& data, Args&&... args) {
      return sub_type::otherwise(
            std::forward<case_type>( data )
          , std::forward<Args>(args)... );
    }

    template <case_type Data, typename... Args>
    static inline return_type when(Args&&... args) {
      return sub_type::template when< Data >( std::forward<Args>(args)... );
    }
  };

  typedef typename std::conditional<
    variadic
      , variadic_proxy
      , static_proxy
      >::type fitting_proxy;

  template<bool CasesEmpty, typename Cases, typename... Args>
  struct match {
    static inline return_type run(case_type&& data, Args&&... args) {

      using namespace boost::mpl;

      // match
      if ( sub_type::compare(front< Cases >::type::value, data) )
        return fitting_proxy::template when< 
            front< Cases >::type::value, Args...
          >(std::forward<Args>(args)... );

      // try next
      else
        return match<
              size< Cases >::value == 1
              // (pop the one we just tested)
            , typename pop_front< Cases >::type 
            , Args...
          >::run( 
              std::forward<case_type>(data)
            , std::forward<Args>(args)... );
    }
  };

  // No match found
  template<typename Cases, typename... Args>
  struct match<true, Cases, Args...> {
    static inline return_type run(case_type&& data, Args&&... args) {
      return fitting_proxy::template otherwise<Args...>(
            std::forward<case_type>(data)
          , std::forward<Args>(args)... );
    }
  };

};

} // namespace metafrog

#endif
