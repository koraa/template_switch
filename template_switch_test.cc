#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <functional>
#include "metafrog/template_switch.hpp"

using metafrog::template_switch;

struct sis_ : template_switch<sis_, int, int> {
  typedef template_switch<sis_, int, int> super;
  typedef super::cases_<1,4,6,10>::type cases;

  template<int data> static int when() {
    return data * 2;
  }

} min_case;

template<typename Er>
void assert_raise(std::function<void()> code) {
  bool caught = false;

  try {
    code();
  } catch (Er &er) {
    caught = true;
  }

  assert( caught );
}

int main() {
  assert( min_case(1) ==  2);
  assert( min_case(4) ==  8);
  assert( min_case(6) == 12);
  assert( min_case(10) == 20);

  assert_raise<metafrog::unknown_case>([](){
    min_case(-2);
  });

  return 0;
}
