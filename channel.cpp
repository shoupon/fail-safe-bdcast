//
//  channel.cpp
//  fail-safe-bdcast
//
//  Created by Shou-pon Lin on 11/17/14
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "channel.h"

Channel::Channel(Lookup* msg, Lookup* mac, int from, const vector<int>& tos)
    : StateMachine(msg, mac), src_site_id_(from) {
  stringstream src_ss;
  src_ss << SITE_NAME << "(" << src_site_id_ << ")";
  src_name_ = src_ss.str();
  stringstream dest_ids_ss;

  num_combinations_ = 1;
  int k = 0;
  for (auto i : tos) {
    if (k++)
      dest_ids_ss << ",";
    dest_ids_ss << i;
    stringstream dest_ss;
    dest_ss << SITE_NAME << "(" << i << ")";
    dest_names_.push_back(string());
    dest_ss >> dest_names_.back();
    num_combinations_ <<= 1;
    msg_exist_.push_back(true);
  }
  stringstream channel_ss;
  channel_ss << CHANNEL_NAME << "(" << src_site_id_ << ")";
  channel_name_ = channel_ss.str();

  setId(machineToInt(channel_name_));
  reset();
}

int Channel::transit(MessageTuple *inMsg, vector<MessageTuple*> &outMsgs,
                     bool &high_prob, int startIdx) {
  outMsgs.clear();

  if (startIdx >= num_combinations_ || startIdx < 0)
    return -1;

  string msg = IntToMessage(inMsg->destMsgId());
  if (msg == SITEMSG) {
    // only allow one message in channel at a time
    // Site does not send more than one message in one period
    if (msg_in_transit_)
      assert(false);

    assert(typeid(*inMsg) == typeid(SiteMessage));
    SiteMessage* sMsg = dynamic_cast<SiteMessage*>(inMsg);
    msg_in_transit_.reset(new SiteMessage(*sMsg));

    if (startIdx) {
      high_prob = false;
    } else {
      high_prob = true;
    }

    int k = startIdx;
    for (int i = 0; i < msg_exist_.size(); ++i) {
      if (k & 1)
        msg_exist_[i] = false;
      else
        msg_exist_[i] = true;
      k >>= 1;
    }

    return startIdx + 1;
  } else if (msg == DEADLINE) {
    if (!startIdx) {
      if (msg_in_transit_) {
        high_prob = false;
      } else {
        high_prob = true;
      }
      reset();
      return 2;
    } else {
      return -1;
    }
  } else {
    assert(false); // there shouldn't be other type of messages
  }
}

int Channel::nullInputTrans(vector<MessageTuple*>& outMsgs,
                            bool& high_prob, int startIdx) {
  outMsgs.clear();
  high_prob = true;

  if (startIdx)
    return -1;

  if (!msg_in_transit_)
    return -1;
  for (int i = 0; i < msg_exist_.size(); ++i) {
    if (msg_exist_[i]) {
      outMsgs.push_back(new SiteMessage(0, machineToInt(dest_names_[i]),
                                        0, msg_in_transit_->destMsgId(),
                                        macId(),
                                        msg_in_transit_->getSequenceNumber()));
    }
  }
  reset();
  return 3;
} 

void Channel::restore(const StateSnapshot *snapshot) {
  assert(typeid(*snapshot) == typeid(ChannelSnapshot));
  const ChannelSnapshot *css = dynamic_cast<const ChannelSnapshot*>(snapshot);
  assert(msg_exist_.size() == css->ss_exist_.size());
  if (css->ss_msg_) {
    msg_in_transit_.reset(new SiteMessage(*(css->ss_msg_)));
  } else {
    msg_in_transit_.reset(0);
  }
  msg_exist_ = css->ss_exist_;
}

StateSnapshot* Channel::curState() {
  if (msg_in_transit_)
    return new ChannelSnapshot(msg_in_transit_.get(), msg_exist_);
  else
    return new ChannelSnapshot(msg_exist_);
}

void Channel::reset() {
  msg_in_transit_.reset(0);
  for (int i = 0; i < msg_exist_.size(); ++i)
    msg_exist_[i] = true;
}

int ChannelSnapshot::curStateId() const {
  if (ss_msg_)
    return 1;
  else 
    return 0;
}

string ChannelSnapshot::toString() const {
  if (ss_msg_)
    return stringify(ss_msg_->toString());
  else
    return stringify("");
}

string ChannelSnapshot::toReadable() const {
  if (ss_msg_)
    return stringify(ss_msg_->toReadable());
  else
    return stringify("");
}


int ChannelSnapshot::toInt() {
  return ss_msg_->subjectId();
}

StateSnapshot* ChannelSnapshot::clone() const {
  if (ss_msg_)
    return new ChannelSnapshot(ss_msg_.get(), ss_exist_);
  else
    return new ChannelSnapshot(ss_exist_);
}

bool ChannelSnapshot::match(StateSnapshot* other) {
  assert(typeid(*other) == typeid(ChannelSnapshot));
  ChannelSnapshot *other_ss = dynamic_cast<ChannelSnapshot*>(other);
  if (!ss_msg_)
    return !other_ss->ss_msg_;
  else
    return *ss_msg_ == *(other_ss->ss_msg_);
}

string ChannelSnapshot::stringify(const string& msg_str) const {
  stringstream ss;
  if (ss_msg_) {
    int k = 0;
    for (int i = 0; i < ss_exist_.size(); ++i) {
      if (k++)
        ss << ",";
      ss << ss_exist_[i];
    }
    return string("[") + msg_str + "]:" + ss.str();
  } else {
    return "[]";
  }
}

