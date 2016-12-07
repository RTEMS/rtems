/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if !defined(_DL_LOAD_H_)
#define _DL_LOAD_H_

#ifdef __cplusplus
extern "C" {
#endif

void exception_base(bool throw_runtime);
void exception_dl(bool throw_runtime);

int dl_load_test(void);

#ifdef __cplusplus
  class dl_test_throw_me
  {
  public:
    dl_test_throw_me(const char* message) :
      message (message) {
    }
    dl_test_throw_me(const dl_test_throw_me& orig) :
      message (orig.message) {
    }
    dl_test_throw_me() :
      message (0) {
    }

    ~dl_test_throw_me() {
    }

    const char* what() const {
      return message;
    }

  private:
    const char* message;
  };
#endif

#ifdef __cplusplus
}
#endif

#endif
