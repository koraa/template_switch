#ifdef NDEBUG
#undef NDEBUG
#endif

#include <string>
#include <functional>
#include <utility>

#include "gtest/gtest.h"

#include "metafrog/template_switch.hpp"

using metafrog::template_switch;

// Helper for testing variadic cases below
template<typename First, typename... Args>
struct sum_ {
  static First apply(First first, Args&&... args) {
    return first + sum_<Args...>::apply( std::forward<Args>(args)... );
  }
};

template<typename Last>
struct sum_<Last>{
  static Last apply(Last last) {
    return last;
  }
};


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();

  return 0;
}

// Most Basic Test /////////////////////////////////////////
// Can create a basic template switch.

struct sis_ : template_switch<sis_, int, int> {
  typedef template_switch<sis_, int, int> super;
  typedef super::cases_<1,4,6,10>::type cases;

  template<int data> static int when() {
    return data * 2;
  }

} min_case;

TEST(TemplateSwitchTest, Minimum) {
  ASSERT_EQ( min_case(1),  2);
  ASSERT_EQ( min_case(4),  8);
  ASSERT_EQ( min_case(6), 12);
  ASSERT_EQ( min_case(10), 20);

  ASSERT_THROW( min_case(-2), metafrog::unknown_case);
};

// Plain Otherwise Test ////////////////////////////////////
// Can create a basic template switch with an otherwise
// statement.

struct otherwise_ : template_switch<otherwise_, size_t, size_t> {
  typedef template_switch<otherwise_, size_t, size_t> super;
  typedef super::cases_<9000, 10000>::type cases;

  template<size_t data> static size_t when() {
    return data + 1;
  }

  static size_t otherwise(size_t data) {
    return data - 1;
  }

} otherwise_case;

TEST(TemplateSwitchTest, OtherwiseAndSizeT) {
  ASSERT_EQ( otherwise_case(9000),  (size_t)9001);
  ASSERT_EQ( otherwise_case(10000),  (size_t)10001);

  ASSERT_NO_THROW(
    ASSERT_EQ( otherwise_case(8000),  (size_t)7999);
    ASSERT_EQ( otherwise_case(24),  (size_t)23);
  );
};

// Extra Parameters Test ///////////////////////////////////
// Can create a template switch taking extra parameters

struct extra_params_ : template_switch<extra_params_, int, int> {
  typedef template_switch<extra_params_, int, int> super;
  typedef super::cases_<20, 30>::type cases;

  template<int data> static int when(int extra) {
    return 1000 + extra + data;
  }

} extra_params;

TEST(TemplateSwitchTest, ExtraParams) {
  ASSERT_EQ( extra_params(20, 40),  1060);
  ASSERT_EQ( extra_params(30, -10),  1020);

  ASSERT_THROW( extra_params(42, 10), metafrog::unknown_case);
};

// Extra Parameters+Otherwise Test /////////////////////////
// Can create a template switch taking extra parameters and
// using an otherwise clause

struct extra_params_otherw_ : template_switch<extra_params_otherw_, int, int> {
  typedef template_switch<extra_params_otherw_, int, int> super;
  typedef super::cases_<20, 30>::type cases;

  template<int data> static int when(int extra) {
    return 1000 + extra + data;
  }

  static int otherwise(int data, int extra) {
    return data + extra;
  }

} extra_params_and_otherwise;

TEST(TemplateSwitchTest, ExtraParamsAndOtherwise) {
  ASSERT_EQ( extra_params_and_otherwise(20, 40),  1060);
  ASSERT_EQ( extra_params_and_otherwise(30, -10),  1020);

  ASSERT_NO_THROW(
    ASSERT_EQ( extra_params_and_otherwise(42, 3),  45);
    ASSERT_EQ( extra_params_and_otherwise(23, 5),  28);
  );
};

// Overload Test ///////////////////////////////////////////
// Can overload a template switch

struct overload_ : template_switch<overload_, int, int> {
  typedef template_switch<overload_, int, int> super;
  typedef super::cases_<20, 30>::type cases;

  template<int data> static int when() {
    return data;
  }

  template<int data> static int when(int a) {
    return data + a;
  }

  template<int data> static int when(int a, int b) {
    return data + a + b;
  }

  template<int data> static int when(char a) {
    return data - a;
  }

} overload_case;

TEST(TemplateSwitchTest, Overload) {
  ASSERT_EQ( overload_case(20),  20);
  ASSERT_EQ( overload_case(20, 40),  60);
  ASSERT_EQ( overload_case(30, 50, 20), 100);
  ASSERT_EQ( overload_case(30, (char)30), 0);

  ASSERT_THROW( overload_case(0), metafrog::unknown_case);
  ASSERT_THROW( overload_case(1, 10), metafrog::unknown_case);
  ASSERT_THROW( overload_case(2, 20, 30), metafrog::unknown_case);
  ASSERT_THROW( overload_case(3, (char)40), metafrog::unknown_case);
};

// Otherwise overload test /////////////////////////////////
// Can overload a template switch ::when and ::otherwise

struct overload_otr_ : template_switch<overload_otr_, int, int> {
  typedef template_switch<overload_otr_, int, int> super;
  typedef super::cases_<20, 30>::type cases;

  template<int data> static int when() {
    return data;
  }

  template<int data> static int when(int a) {
    return data + a;
  }

  static int otherwise(int data) {
    return data - 1000;
  }

  static int otherwise(int data, int a) {
    return data - 1000 - a;
  }

} overload_otherwise_case;

TEST(TemplateSwitchTest, OverloadOtherwise) {
  ASSERT_EQ( overload_otherwise_case(20),  20);
  ASSERT_EQ( overload_otherwise_case(20, 40),  60);

  ASSERT_NO_THROW(
    ASSERT_EQ( overload_otherwise_case(42),  42-1000);
    ASSERT_EQ( overload_otherwise_case(23, 5),  23-1000-5);
  );
};

// Variadic Test ///////////////////////////////////////////
// Can take variadic parameters

struct variadic_ : template_switch<variadic_, int, int> {
  typedef template_switch<variadic_, int, int> super;
  static const bool variadic = true;
  typedef super::cases_<20, 30>::type cases;

  template<int data, typename... Args>
  static int when(Args&&... args) {
    return sum_<int, Args...>::apply( data, std::forward<Args>(args)... );
  }

} variadic_case;

TEST(TemplateSwitchTest, Variadic) {
  ASSERT_EQ( variadic_case(20),  20);
  ASSERT_EQ( variadic_case(20, 40),  60);
  ASSERT_EQ( variadic_case(30, 50, 20), 100);
  ASSERT_EQ( variadic_case(30, 50, 20, 0, 0, 0, 1), 101);

  ASSERT_THROW( variadic_case(0), metafrog::unknown_case);
  ASSERT_THROW( variadic_case(1, 10), metafrog::unknown_case);
  ASSERT_THROW( variadic_case(2, 20, 30), metafrog::unknown_case);
};

// Variadic Otherwise Test /////////////////////////////////
// Can take variadic parameters with otherwise

struct variadic_otr_ : template_switch<variadic_otr_, int, int> {
  typedef template_switch<variadic_otr_, int, int> super;
  static const bool variadic = true;
  typedef super::cases_<20, 30>::type cases;

  template<int data, typename... Args>
  static int when(Args&&... args) {
    return sum_<int, Args...>::apply( data, std::forward<Args>(args)... );
  }

  template<typename... Args>
  static int otherwise(int data, Args&&... args) {
    return sum_<int, int, Args...>::apply( data, -1000, std::forward<Args>(args)... );
  }

} variadic_otherwise_case;

TEST(TemplateSwitchTest, VariadicOtherwise) {
  ASSERT_EQ( variadic_otherwise_case(20),  20);
  ASSERT_EQ( variadic_otherwise_case(20, 40),  60);
  ASSERT_EQ( variadic_otherwise_case(30, 50, 20), 100);
  ASSERT_EQ( variadic_otherwise_case(30, 50, 20, 0, 0, 0, 1), 101);

  ASSERT_EQ( variadic_otherwise_case(1),  -999);
  ASSERT_EQ( variadic_otherwise_case(2, 20),  2-1000+20);
  ASSERT_EQ( variadic_otherwise_case(3, 50, 20), 3-1000+50+20);
  ASSERT_EQ( variadic_otherwise_case(4, 50, 20, 0, 0, 0, 1), 4-1000+50+20+1);
};
