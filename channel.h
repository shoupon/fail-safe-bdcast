//
//  channel.h
//  fail-safe-bdcast
//
//  Created by Shou-pon Lin on 11/17/14
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#ifndef FAILSAFEBDCAST_CHANNEL_H
#define FAILSAFEBDCAST_CHANNEL_H

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

#include "../prob_verify/statemachine.h"
#include "../prob_verify/sync.h"
#include "identifiers.h"
#include "site.h"

class ChannelSnapshot;

class Channel: public StateMachine {
public:
  Channel(Lookup* msg, Lookup* mac, int from, int to);
  ~Channel() {}
  int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
              bool& high_prob, int startIdx = 0);
  int nullInputTrans(vector<MessageTuple*>& outMsgs,
                     bool& high_prob, int startIdx = 0);
  void restore(const StateSnapshot *snapshot);
  StateSnapshot* curState();
  void reset();

private:
  unique_ptr<SiteMessage> msg_in_transit_;
  int channel_id_;
  int src_site_id_;
  int dest_site_id_;
  string channel_name_;
  string src_name_;
  string dest_name_;
};

class ChannelSnapshot: public StateSnapshot {
  friend class Channel;
public:
  ChannelSnapshot(): ss_msg_(nullptr) {}
  ChannelSnapshot(const SiteMessage* site_msg)
      : ss_msg_(new SiteMessage(*site_msg)) {}
  ~ChannelSnapshot() {}
  int curStateId() const;
  string toString();
  int toInt();
  StateSnapshot* clone() const;
  bool match(StateSnapshot* other);

protected:
  unique_ptr<SiteMessage> ss_msg_;
};
