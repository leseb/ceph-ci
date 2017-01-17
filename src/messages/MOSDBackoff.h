// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */


#ifndef CEPH_MOSDBACKOFF_H
#define CEPH_MOSDBACKOFF_H

#include "msg/Message.h"
#include "osd/osd_types.h"

class MOSDBackoff : public Message {
public:
  uint8_t op = 0;           ///< CEPH_OSD_BACKOFF_OP_*
  uint64_t id = 0;          ///< unique id within this session
  hobject_t begin, end;     ///< [) range to block, unless ==, block single obj
  ceph_tid_t first_tid = 0; ///< block everything after this tid
  uint32_t first_attempt = 0; ///< attempt for above request
  epoch_t osd_epoch = 0;

  MOSDBackoff() : Message(CEPH_MSG_OSD_BACKOFF) {}
  MOSDBackoff(uint8_t op_, uint64_t id_,
	      hobject_t begin_, hobject_t end_, ceph_tid_t t,
	      uint32_t a, epoch_t ep)
    : Message(CEPH_MSG_OSD_BACKOFF),
      op(op_),
      id(id_),
      begin(begin_),
      end(end_),
      first_tid(t),
      first_attempt(a),
      osd_epoch(ep) { }

  void encode_payload(uint64_t features) override {
    ::encode(op, payload);
    ::encode(id, payload);
    ::encode(begin, payload);
    ::encode(end, payload);
    ::encode(first_tid, payload);
    ::encode(first_attempt, payload);
    ::encode(osd_epoch, payload);
  }

  void decode_payload() override {
    auto p = payload.begin();
    ::decode(op, p);
    ::decode(id, p);
    ::decode(begin, p);
    ::decode(end, p);
    ::decode(first_tid, p);
    ::decode(first_attempt, p);
    ::decode(osd_epoch, p);
  }

  const char *get_type_name() const override { return "osd_backoff"; }

  void print(ostream& out) const override {
    out << "osd_backoff(" << ceph_osd_backoff_op_name(op)
	<< " id " << id
	<< " [" << begin << "," << end << ")"
	<< " tid " << first_tid
	<< " attempt " << first_attempt
	<< " epoch " << osd_epoch << ")";
  }
};

#endif
