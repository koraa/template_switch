#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <functional>

#include "gtest/gtest.h"

#include "metafrog/template_switch.hpp"

using metafrog::template_switch;

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

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();

  return 0;
}
