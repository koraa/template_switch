// Use this to generate assembly for performance evaluation.
//
// The CONSTANT_TEMPLATE_CASE macro is used as an argument:
// * When set to 0x1, 0x4, 0x6 or 0x10 this will be a valid
//   case and it's value times two will be returned
// * When set to any other value, the it is an unknown case
//   and min_case should return 23.
// * If it is "argc", the value will be the number of
//   parameters (preventing the compiler from optimizing the
//   entire template_switch away)

#include "metafrog/template_switch.hpp"
using metafrog::template_switch;

struct sw : template_switch<sw, int, int> {
  typedef template_switch<sw, int, int> super;
  typedef super::cases_<0x1,0x4,0x6,0x10>::type cases;

  template<int data> inline static int when() {
    return data * 2;
  }

  inline static int otherwise(...) {
    return 23;
  }
} min_case;

int main(int argc METAFROG_ATTR_UNUSED, char **argv METAFROG_ATTR_UNUSED) {
  return min_case( CONSTANT_TEMPLATE_CASE );
}
