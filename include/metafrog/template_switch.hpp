#ifndef METAFROG_TEMPLATE_SWITCH_HEADER
#define METAFROG_TEMPLATE_SWITCH_HEADER

#include <boost/mpl/vector.hpp>

#include <utility>
#include <type_traits>

namespace metafrog {

class unknown_case : std::exception {
public:
  using std::exception::exception;
  const char* what() const noexcept {
    return "Unknown case was encountered in an instance "
            "of metafrog::template_switch";
  }
};

template<typename SubType, typename CaseType>
struct template_switch {
public: // types

  typedef SubType sub_type;
  typedef CaseType case_type;

  template<case_type CaseV>
  struct case_ {
    static const case_type type = CaseV;
  };

  template <case_type... Cases>
  struct cases_ {
    typedef boost::mpl::vector< case_<Cases>... > type;
  };

public: // Defaults for users

  static const bool variadic = false;
  
  inline bool compare(case_type a, case_type b) {
    return a == b;
  }

  /// Default otherwise clause. This will simply raise an
  /// exception if an unknown case is encountered.
  template<typename... Args>
  static case_type otherwise(...) {
    throw unknown_case();
  }

public: // call operator

  template<typename Return, typename... Args>
  inline Return operator()(case_type&& data, Args&&... args) {
    match<typename sub_type::cases, Return, Args...>(
        std::forward( data ), std::forward<Args>(args)... );
  }

private: // Detail
  
  struct variadic_proxy {
    template <typename Return, typename... Args>
    static inline Return otherwise(case_type&& data, Args&&... args) {
      return sub_type::template otherwise<Return, Args...>(
            std::forward( data )
          , std::forward<Args>(args)... );
    }

    template <case_type Data, typename Return, typename... Args>
    static inline Return when(Args&&... args) {
      typedef typename sub_type::template then< Data > then_t;
      return then_t::template run<Return, Args...>( std::forward<Args>(args)... );
    }
  };

  struct static_proxy {
    template <typename Return, typename... Args>
    static inline Return otherwise(case_type&& data, Args&&... args) {
      return sub_type::otherwise(
            std::forward( data )
          , std::forward<Args>(args)... );
    }

    template <case_type Data, typename Return, typename... Args>
    static inline Return when(Args&&... args) {
      typedef typename sub_type::template then< Data > then_t;
      return then_t::run( std::forward<Args>(args)... );
    }
  };

  typedef typename std::conditional<
    sub_type::variadic
      , variadic_proxy
      , static_proxy
      > fitting_proxy;

  template<typename Cases, typename Return, typename... Args>
  static inline Return match(case_type&& data, Args&&... args) {

    using namespace boost::mpl;

    // empty
    if ( empty< Cases >::type )
      return fitting_proxy::template otherwise<
          Return, Args...
        >(std::forward(data), std::forward<Args>(args)... );

    // match
    else if ( sub_type::compare(front< Cases >::type, data) )
      return fitting_proxy::template when< 
          typename front< Cases >::type, Return, Args...
        >(std::forward<Args>(args)... );

    // try next
    else
      return match<
            pop_front< Cases >::type // (pop the one we just tested)
          , Return, Args...
        >( std::forward(data), std::forward<Args>(args)... );
  }

};

} // namespace metafrog

#endif
