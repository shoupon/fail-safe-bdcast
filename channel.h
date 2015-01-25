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
  Channel(Lookup* msg, Lookup* mac, int from, const vector<int>& tos);
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
  vector<bool> msg_exist_;

  int channel_id_;
  int src_site_id_;
  vector<int> dest_site_ids_;
  string channel_name_;
  string src_name_;
  vector<string> dest_names_;
  int num_combinations_;
};

class ChannelSnapshot: public StateSnapshot {
  friend class Channel;
public:
  ChannelSnapshot(const vector<bool>& exist)
      : ss_msg_(nullptr), ss_exist_(exist) {}
  ChannelSnapshot(const SiteMessage* site_msg, const vector<bool>& exist)
      : ss_msg_(new SiteMessage(*site_msg)) {
    ss_exist_ = exist;
  }
  ~ChannelSnapshot() {}
  int curStateId() const;
  string toString();
  int toInt();
  StateSnapshot* clone() const;
  bool match(StateSnapshot* other);

protected:
  unique_ptr<SiteMessage> ss_msg_;
  vector<bool> ss_exist_;
};

#endif // FAILSAFEBDCAST_CHANNEL_H
