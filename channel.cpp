//
//  channel.cpp
//  fail-safe-bdcast
//
//  Created by Shou-pon Lin on 11/17/14
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "channel.h"

Channel::Channel(Lookup* msg, Lookup* mac, int from, int to)
    : StateMachine(msg, mac), src_site_id_(from), dest_site_id_(to) {
  stringstream src_ss;
  src_ss << SITE_NAME << "(" << src_site_id_ << ")";
  src_name_ = src_ss.str();
  stringstream dest_ss;
  dest_ss << SITE_NAME << "(" << dest_site_id_ << ")";
  dest_name_ = dest_ss.str();
  stringstream channel_ss;
  channel_ss << CHANNEL_NAME << "(" << src_site_id_ << "," 
             << dest_site_id_ << ")";
  channel_name_ = channel_ss.str();
  setId(machineToInt(channel_name_));
  reset();
}

int Channel::transit(MessageTuple *inMsg, vector<MessageTuple*> &outMsgs,
                     bool &high_prob, int startIdx) {
  outMsgs.clear();

  if (startIdx != 1 && startIdx != 0)
    return -1;

  string msg = IntToMessage(inMsg->destMsgId());
  if (msg == SITEMSG) {
    // only allow one message in channel at a time
    // Site does not send more than one message in one period
    if (msg_in_transit_)
      assert(false);

    if (startIdx == 1) {
      high_prob = false;
      return 2; // loses the message
    } else {
      high_prob = true;
      assert(typeid(*inMsg) == typeid(SiteMessage));
      SiteMessage* sMsg = dynamic_cast<SiteMessage*>(inMsg);
      msg_in_transit_.reset(new SiteMessage(*sMsg));
      return 1;
    }
  } else if (msg == DEADLINE) {
    if (startIdx == 0) {
      if (msg_in_transit_) {
        high_prob = false;
        msg_in_transit_.reset(0);
      } else {
        high_prob = true;
      }
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
  outMsgs.push_back(
    new SiteMessage(0, machineToInt(dest_name_),
                    0, msg_in_transit_->destMsgId(),
                    macId(), msg_in_transit_->getSequenceNumber()));
  msg_in_transit_.reset(0);
  return 3;
} 

void Channel::restore(const StateSnapshot *snapshot) {
  assert(typeid(*snapshot) == typeid(ChannelSnapshot));
  const ChannelSnapshot *css = dynamic_cast<const ChannelSnapshot*>(snapshot);
  if (css->ss_msg_)
    msg_in_transit_.reset(new SiteMessage(*(css->ss_msg_)));
  else
    msg_in_transit_.reset(0);
}

StateSnapshot* Channel::curState() {
  if (msg_in_transit_)
    return new ChannelSnapshot(msg_in_transit_.get());
  else
    return new ChannelSnapshot();
}

void Channel::reset() {
  msg_in_transit_.reset(0);
}

int ChannelSnapshot::curStateId() const {
  if (ss_msg_)
    return 1;
  else 
    return 0;
}

string ChannelSnapshot::toString() {
  if (ss_msg_)
    return string("[") + ss_msg_->toString() + "]";
  else
    return "[]";
}

int ChannelSnapshot::toInt() {
  return ss_msg_->subjectId();
}

StateSnapshot* ChannelSnapshot::clone() const {
  if (ss_msg_)
    return new ChannelSnapshot(ss_msg_.get());
  else
    return new ChannelSnapshot();
}

bool ChannelSnapshot::match(StateSnapshot* other) {
  assert(typeid(*other) == typeid(ChannelSnapshot));
  ChannelSnapshot *other_ss = dynamic_cast<ChannelSnapshot*>(other);
  if (!ss_msg_)
    return !other_ss->ss_msg_;
  else
    return *ss_msg_ == *(other_ss->ss_msg_);
}
