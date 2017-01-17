// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include "PG.h"
#include "Session.h"

void Session::clear_backoffs()
{
  map<hobject_t,BackoffRef,hobject_t::BitwiseComparator> ls;
  {
    Mutex::Locker l(backoff_lock);
    ls.swap(backoffs);
  }
  for (auto& p : ls) {
    Backoff *b = p.second.get();
    Mutex::Locker l(b->lock);
    if (b->pg) {
      assert(b->session == this);
      assert(b->is_new() || b->is_acked());
      b->pg->rm_backoff(b);
      b->pg.reset();
      b->session.reset();
    } else if (b->session) {
      assert(b->session == this);
      assert(b->is_deleting());
      b->session.reset();
    }
  }
}
