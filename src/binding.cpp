/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Freeman Zhang <zhanggyb@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cppevent/abstract-trackable.hpp>
#include <cppevent/binding.hpp>
#include <cppevent/token.hpp>

#ifdef DEBUG
#include <cassert>
#endif

namespace CppEvent {

Binding::~Binding ()
{
  if (trackable_object) {

    if (previous)
      previous->next = next;
    else
      trackable_object->first_binding_ = next;

    if (next)
      next->previous = previous;
    else
      trackable_object->last_binding_ = previous;

  } else {

    if (previous) previous->next = next;
    if (next) next->previous = previous;

  }

  previous = 0;
  next = 0;

  if (token) {
#ifdef DEBUG
    assert(token->binding == this);
#endif
    token->binding = 0;
    delete token;
    token = 0;
  }
}

}