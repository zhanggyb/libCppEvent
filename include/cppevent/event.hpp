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

#pragma once

#include <cppevent/abstract-trackable.hpp>
#include <cppevent/trackable.hpp>
#include <cppevent/delegate-connection.hpp>
#include <cppevent/chain-connection.hpp>

namespace CppEvent {

template<typename ... ParamTypes>
class Event: public AbstractTrackable
{
public:

  inline Event ();

  virtual ~Event ();

  /**
   * @brief Connect this event to a method of trackable object
   */
  template<typename T, void (T::*TMethod) (ParamTypes...)>
  void Connect (T* obj);

  /**
   * @brief Connect this event to a const method of trackable object
   */
  template<typename T, void (T::*TMethod) (ParamTypes...) const>
  void Connect (T* obj);

  void Connect (Event<ParamTypes...>& other);

  template<typename T, void (T::*TMethod) (ParamTypes...)>
  void DisconnectOne (T* obj);

  template<typename T, void (T::*TMethod) (ParamTypes...)>
  void DisconnectAll (T* obj);

  template<typename T, void (T::*TMethod) (ParamTypes...) const>
  void DisconnectOne (T* obj);

  template<typename T, void (T::*TMethod) (ParamTypes...) const>
  void DisconnectAll (T* obj);

  void DisconnectOne (Event<ParamTypes...>& other);

  void DisconnectAll (Event<ParamTypes...>& other);

  void DisconnectAll ();

  virtual void Invoke (ParamTypes ... Args);

protected:

  virtual void AuditDestroyingConnection (Connection* node) final;

private:

  Connection* v_;  // a node pointer to iterate through all nodes in fire()
  bool removed_;

};

template<typename ... ParamTypes>
inline Event<ParamTypes...>::Event ()
    : AbstractTrackable(), v_(0), removed_(false)
{
}

template<typename ... ParamTypes>
Event<ParamTypes...>::~Event ()
{
    RemoveAllConnections();
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...)>
void Event<ParamTypes...>::Connect (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {

    Connection* downstream = new Connection;

    Delegate<void, ParamTypes...> d =
        Delegate<void, ParamTypes...>::template from_method<T, TMethod>(obj);
    DelegateConnection<ParamTypes...>* upstream = new DelegateConnection<
        ParamTypes...>(d);

    Connection::LinkPair(upstream, downstream);

    this->PushBackConnection(upstream);
    add_connection(trackable_object, downstream);

    return;
  }
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...) const>
void Event<ParamTypes...>::Connect (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {

    Connection* downstream = new Connection;

    Delegate<void, ParamTypes...> d =
        Delegate<void, ParamTypes...>::template from_const_method<T, TMethod>(
            obj);
    DelegateConnection<ParamTypes...>* upstream = new DelegateConnection<
        ParamTypes...>(d);

    Connection::LinkPair(upstream, downstream);

    this->PushBackConnection(upstream);
    add_connection(trackable_object, downstream);

    return;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Connect (Event<ParamTypes...>& other)
{
  ChainConnection<ParamTypes...>* upstream = new ChainConnection<ParamTypes...>(
      &other);
  InvokableConnection<ParamTypes...>* downstream = new InvokableConnection<
      ParamTypes...>;
  Connection::LinkPair(upstream, downstream);
  this->PushBackConnection(upstream);
  add_connection(&other, downstream);
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...)>
void Event<ParamTypes...>::DisconnectOne (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateConnection<ParamTypes...>* conn = 0;
    for (Connection* p = tail_connection(); p; p = p->previous_connection()) {
      conn = dynamic_cast<DelegateConnection<ParamTypes...>*>(p);
      if (conn && conn->delegate().template equal<T, TMethod>(obj)) {
        delete conn;
        break;
      }
    }

    return;
  }
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...)>
void Event<ParamTypes...>::DisconnectAll (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateConnection<ParamTypes...>* conn = 0;
    for (Connection* p = tail_connection(); p; p = p->previous_connection()) {
      conn = dynamic_cast<DelegateConnection<ParamTypes...>*>(p);
      if (conn && conn->delegate().template equal<T, TMethod>(obj)) delete conn;
    }

    return;
  }
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...) const>
void Event<ParamTypes...>::DisconnectOne (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateConnection<ParamTypes...>* conn = 0;
    for (Connection* p = tail_connection(); p; p = p->previous_connection()) {
      conn = dynamic_cast<DelegateConnection<ParamTypes...>*>(p);
      if (conn && conn->delegate().template equal<T, TMethod>(obj)) {
        delete conn;
        break;
      }
    }
    return;
  }
}

template<typename ... ParamTypes>
template<typename T, void (T::*TMethod) (ParamTypes...) const>
void Event<ParamTypes...>::DisconnectAll (T* obj)
{
  Trackable* trackable_object = dynamic_cast<Trackable*>(obj);
  if (trackable_object) {
    DelegateConnection<ParamTypes...>* conn = 0;
    for (Connection* p = tail_connection(); p; p = p->previous_connection()) {
      conn = dynamic_cast<DelegateConnection<ParamTypes...>*>(p);
      if (conn && conn->delegate().template equal<T, TMethod>(obj)) delete conn;
    }
    return;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectOne (Event<ParamTypes...>& other)
{
  ChainConnection<ParamTypes...>* conn = 0;
  for (Connection* p = tail_connection(); p; p = p->previous_connection()) {
    conn = dynamic_cast<ChainConnection<ParamTypes...>*>(p);
    if (conn && conn->event() == (&other)) {
      delete conn;
      break;
    }
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAll (Event<ParamTypes...>& other)
{
  ChainConnection<ParamTypes...>* conn = 0;
  for (Connection* p = tail_connection(); p; p = p->previous_connection()) {
    conn = dynamic_cast<ChainConnection<ParamTypes...>*>(p);
    if (conn && conn->event() == (&other)) delete conn;
  }
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::DisconnectAll ()
{
  RemoveAllConnections();
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::Invoke (ParamTypes ... Args)
{
  v_ = this->head_connection();
  while (v_) {
    static_cast<InvokableConnection<ParamTypes...>*>(v_)->Invoke(Args...);
    if (removed_) {
      removed_ = false;
    } else {
      v_ = v_->next_connection();
    }
  }
  v_ = 0;
  removed_ = false;
}

template<typename ... ParamTypes>
void Event<ParamTypes...>::AuditDestroyingConnection (Connection* node)
{
  if (node == v_) {
    removed_ = true;
    v_ = v_->next_connection();
  }
}

}
