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
#include <typeinfo>

/// Used to indicate that a parameter is not used. Must be
/// overwritten on non gcc compatible platforms
#ifndef METAFROG_ATTR_UNUSED
#define METAFROG_ATTR_UNUSED __attribute__ ((unused))
#endif

/// Check whether a type has a specific member: Generates an
/// unary template. This template checks whether the given
/// type has the specified member. The result will be stored
/// in value.
///
/// ```
/// METAFROG_DECLARE_MEMBER_TEST(has_hello, ::hello);
/// ...
/// bool b = has_hello<my_class>::value;
/// ```
///
/// @param The name of the template generated
/// @param The name of the member to check for with the
///   appropriate access operator (e.g. ->foo, ::bar or .baz)
#define METAFROG_DECLARE_MEMBER_TEST(name, member)         \
  template <typename T> class name {                       \
      template <typename C> static bool test(              \
        const std::type_info &x METAFROG_ATTR_UNUSED       \
            = typeid(C member ) );                         \
      template <typename C> static long test(...);         \
    public:                                                \
      enum { value = sizeof(test<T>(0)) == sizeof(char) }; \
  };

namespace metafrog {

/// Thrown by the default implementation of otherwise
class unknown_case : std::exception {
public:
  using std::exception::exception;
  const char* what() const noexcept {
    return "Unknown case was encountered in an instance "
            "of metafrog::template_switch";
  }
};

/// Convert runtime variables from a finite set of values to
/// compile time constants for use in templates.
///
/// Allows the creation of a functor (in this case
/// a function like object) that takes a runtime value from
/// set of values known at compile time and passes the value
/// as a template-argument to a function-template member
/// 'when'.
///
/// My motivation for writing this is that some problems can
/// be more easily stated using templates rather than
/// runtime c++; especially when using template
/// specialization (or as it's called at runtime, pattern
/// matching).
/// Specifically I used this pattern in a project where
/// I had to represent a run time type system with
/// conversions from those run time types to c++ types.
///
/// Declarations take the following form for non-variadic
/// functors:
///
/// ```
/// struct my_functor_t : template_switch<my_functor_t, std::string, int> {
///   typedef template_switch<my_functor_t, std::string, int> super;
///
///   // Our list of cases
///   typedef super::cases_< 24, 23, 5, 17 >::type cases;
///
///   // Optional; Custom case comparison; by default this redirects to `==`
///   constexpr bool compare(int a, int b) {
///     return a == b;
///   }
///
///   // Called when a valid case is encountered
///   template<int data> static inline std::string when() { /* ... */ }
///
///   // Optional; Called when an unknown case is encountered;
///   // raises an exception by default
///   static inline std::string otherwise(int data) { /* ... */ }
/// } call_me;
/// ```
///
/// In the example we see that we can basically declare
/// a template switch by declaring a struct that inherits
/// from template_switch. The template_switch template
/// instance is given the return type of the function, the
/// type of each case and the subclass so it can access the
/// subclasses types and functions.
///
/// Within that struct we need to declare a list of cases
/// and a template function which will be called with the
/// translated type.
///
/// Because the entire construct is essentially a fancy way
/// of declaring a function, we usually just create a single
/// instance of it at the bottom of the struct declaration.
/// This is also the reason why all the methods are static:
/// They are not meant to carry any state.
///
/// In this example all methods are declared constexpr or
/// inline: This is done, because all those functions are
/// usually very small. In the end, template_switch is
/// frequently just an alternative way of writing
/// a switch/case statement.
///
/// @tparam SubType The subclass inheriting from
///   template_switch<...>; this must be given so the subclass'
///   members and types can be accessed.
/// @tparam ReturnType The return type of the functor. Must
///   also be the return type of when() and otherwise()
/// @tparam CaseType The type of the cases. This must be
///   a type that can be used as a non-type template
///   parameter.
///
/// ### Advanced Usage
///
/// For examples of these usages see the tests!
///
/// **otherwise:** Optionally we can also declare
/// a otherwise function, which will be called if an invalid
/// (non case) runtime value was given. That function is
/// also given the `data` parameter, but still as a runtime
/// value.  If no otherwise function is declared, a built in
/// one will be used which raises an error.
///
/// **additional parameters:** Supported out of the box:
/// Just add the parameters to when() and otherwise() and
/// call the functor with the added parameters.  Take care
/// that otherwise() and when() have the same signatures
/// (except the first data parameter) if otherwise is used.
/// Else the behaviour is undefined.
///
/// **overloading:** Function overloading is easily
/// supported: Just add the additional signatures and it
/// should work.
/// The behaviour is undefined if otherwise() is overwritten
/// and provides a different set of overloads.
///
/// **variadic parameters:** Variadic parameters are also
/// supported: Just add a `static const bool variadic = true;`
/// to your struct declaration and let when and optionally
/// otherwise take the additional variadic parameters by
/// adding `typename... Args` to the template parameters and
/// `Args.. args` to the runtime parameters.
/// The behaviour is undefined if when(), otherwise() and
/// variadic=true are not consistently set.
/// Mixing fixed and variadic parameters is not supported.
///
/// **stateful functors:** Our functors are really struct
/// instances – objects. However they are only functions so
/// the call `()` operator can be overloaded and since
/// when(), otherwise() and compare() can not be used we
/// can not make use of any state in the functions.
/// If you still want to carry some state in the functions,
/// you could still overwrite the call operator and insert
/// a reference to the object as a parameter. Thus at least
/// when() and otherwise() could gain access to the state.
///
/// **generic programming:** Like any struct, the child can
/// of course be a template too. E.g. it could take the list
/// of cases from extern. Or the type of the return value,
/// or a template to be instantiated with the template
/// parameter given to when...
/// Also each template_switch instances provide case_type
/// and return_type as typedefs, which can be used in stead
/// of explicitly writing down those types.
///
/// ## How it works, internally
///
/// Of course, you can not really translate runtime values
/// to compile time constants; instead think of this as
/// a switch statement, where each cases contains a call to the
/// appropriate instantiation of the `template<...> ::when(...)`
/// and else just contains a call to `::otherwise(...)`.
///
/// But I lied to you again: Of course this is not really
/// based on a switch statement, because switch statements
/// only work on a fixed number of cases (fixed as in "you
/// need to write each case down manually"), so this does
/// not work.
/// Instead, internally template_switch iterates recursively
/// over each case calling compare to check whether we have
/// the correct case and finally calling otherwise() if we
/// didn't find anything.
///
/// This seems not very efficient, but since the list of
/// cases is known completely at compile time, the compiler
/// can actually very efficiently optimize this: In the best
/// case the compiler would recognize that this is another
/// form of case statement.
///
/// Looking at the assembler code this generates, gcc and clang
/// seem to do very good jobs at optimizing this at
/// optimization levels other than O1 and O0: For
/// compile-time constant input values, the entire
/// template_switch is resolved at compile time and no
/// comparison needs to be done at runtime.
/// For runtime values (those which can not possibly be
/// known at compile time) a series of if/if else statements
/// is generated.
///
/// No tests have been done to compare how similarly this
/// behaves to equivalent case statements.
template<typename SubType, typename ReturnType, typename CaseType>
struct template_switch {

  typedef SubType sub_type;

public: // types

  /// The type calls to this template_switch will return
  typedef ReturnType return_type;
  /// The type of each case
  typedef CaseType case_type;

  /// Wrapper around case_type so it can be stored in
  /// a type. Simply contains a static constant field
  /// 'value' storing the given case_type;
  template<case_type CaseV>
  struct case_ {
    static const case_type value = CaseV;
  };

  /// List of cases. This is just mpl vector with each
  /// element wrapped in case_
  template <case_type... Cases>
  struct cases_ {
    typedef boost::mpl::vector< case_<Cases>... > type;
  };

private: // Detail

  // TODO: Can we simply check whether ::when tk
  // TODO: Also check whether T::variadic == true
  /// Check used to determine whether the child class
  /// demands triadic support.
  METAFROG_DECLARE_MEMBER_TEST(is_variadic, ::variadic);

public: // Defaults for users

  /// Whether when and otherwise are variadic.
  /// Can be overwritten.
  /// If true, the when<...>::run and otherwise functions
  /// will be passed the expected return type and the
  /// parameters given as variadic template parameters.
  static const bool variadic = is_variadic<sub_type>::value;

  /// Default case compare. May be overwritten
  static constexpr bool compare(case_type a, case_type b) {
    return a == b;
  }

  /// Default otherwise clause. This will simply raise an
  /// exception. May be overwritten
  template<typename... Args>
  static case_type otherwise(...) {
    throw unknown_case();
  }

public: // call operator

  /// Call this switch statement!
  template<typename... Args>
  inline return_type operator()(case_type data, Args&&... args) {
    return match<
          boost::mpl::empty<typename sub_type::cases>::value
        , typename sub_type::cases
        , Args...
      >::run(
          std::forward<case_type>( data )
        , std::forward<Args>(args)... );
  }

private: // Detail: Implementation

  /// Proxy for calling the child's ::when and ::otherwise.
  /// Variadic version. Passes the variadic template
  /// parameters on to ::otherwise and ::when
  struct variadic_proxy {
    template <typename... Args>
    static inline return_type otherwise(case_type data, Args&&... args) {
      return sub_type::template otherwise<Args...>(
            std::forward( data )
          , std::forward<Args>(args)... );
    }

    template <case_type Data, typename... Args>
    static inline return_type when(Args&&... args) {
      return sub_type::template then< Data, Args...>( std::forward<Args>(args)... );
    }
  };

  /// Proxy for calling the child's ::when and ::otherwise.
  /// Static version; drops the variadic template arguments
  struct static_proxy {
    template <typename... Args>
    static inline return_type otherwise(case_type data, Args&&... args) {
      return sub_type::otherwise(
            std::forward<case_type>( data )
          , std::forward<Args>(args)... );
    }

    template <case_type Data, typename... Args>
    static inline return_type when(Args&&... args) {
      return sub_type::template when< Data >( std::forward<Args>(args)... );
    }
  };

  /// The proxy for ::when and ::otherwise this instance of
  /// the template will really use.
  /// Selects either variadic_proxy or static_proxy based on
  /// whether variadic support is requested by the child.
  typedef typename std::conditional<
    variadic
      , variadic_proxy
      , static_proxy
      >::type fitting_proxy;

  /// The actual 'case' statement, selecting what case to
  /// branch into.
  ///
  /// This is not really a case statement, but actually
  /// recursively iterates over the list of cases.
  /// However, since the list of cases at each step of the
  /// recursion is a compile time constant, and since match
  /// is marked inline and also since the default ::compare
  /// is a constexpr warpping ==, the compiler would
  /// interpret this as an actual case statement in the best
  /// case.
  ///
  /// But there's not way to gurantee ofc.
  template<bool CasesEmpty, typename Cases, typename... Args>
  struct match {
    static inline return_type run(case_type data, Args&&... args) {

      using namespace boost::mpl;

      // match -------------------
      if ( sub_type::compare(front< Cases >::type::value, data) )
        return fitting_proxy::template when<
            front< Cases >::type::value, Args...
          >(std::forward<Args>(args)... );

      // try next ----------------
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
    static inline return_type run(case_type data, Args&&... args) {
      return fitting_proxy::template otherwise<Args...>(
            std::forward<case_type>(data)
          , std::forward<Args>(args)... );
    }
  };

};

} // namespace metafrog

#endif
