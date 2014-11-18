//
//  site.cpp
//  fail-safe-bdcast
//
//  Created by Shou-pon Lin on 11/17/14
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "site.h"

Site::Site(Lookup* msg, Lookup* mac, int id)
    : StateMachine(msg, mac), site_id_(id),
      commit_phases_(WRAP_MULTIPLIER * num_sites_, 0), counter_(0) {
  stringstream ss;
  ss << SITE_NAME << "(" << id << ")";
  setId(machineToInt(ss.str()));
  reset() ;
}

int Site::transit(MessageTuple *inMsg, vector<MessageTuple*> &outMsgs,
                  bool &high_prob, int startIdx) {
  outMsgs.clear();
  high_prob = true;
  
  // all transitions are deterministic
  if (startIdx)
    return -1;
    
  string msg = IntToMessage(inMsg->destMsgId()) ;
  string src = IntToMachine(inMsg->subjectId()) ;
  switch (_state) {
    case 0:
      if (msg == SITEMSG) {
        return 3;
      } else if (msg == DEADLINE) {
        ++counter_;
        // phase reset to 0
        phaseReset(counter_);
        // phase 1 commit
        phase1(counter_ - num_sites_);
        // phase 2 commit
        for (int k = counter_ - 3 * num_sites_;
             k <= counter_ - 2 * num_sites_ - 1; ++k) {
          phase2(k);
        }

        if (counter_ % num_sites_ == site_id_) {
          broadcast(inMsg, outMsgs, counter_);
        } else {
          _state = 2;
        }
        return 3;
      }
      break;

    case 1:
      if (msg == SITEMSG) {
        return 3;
      } else if (msg == DEADLINE) {
        ++counter_;
        phaseReset(counter_);
        // phase 2 commit
        for (int k = counter_ - 3 * num_sites_;
             k <= counter_ - 2 * num_sites_ - 1; ++k) {
          phase2(k);
        }

        if (counter_ % num_sites_ == site_id_)
          _state = 3;
        else
          _state = 1;
        return 3;
      }
      break;

    case 2:
      if (msg == SITEMSG) {
        assert(typeid(SiteMessage) == typeid(*inMsg));
        SiteMessage* site_msg = dynamic_cast<SiteMessage*>(inMsg);
        assert(site_msg->getSequenceNumber() == counter_);
        _state = 0;
        return 3;
      } else if (msg == DEADLINE) {
        ++counter_;
        phaseReset(counter_);
        for (int k = counter_ - 3 * num_sites_;
             k <= counter_ - 2 * num_sites_ - 1; ++k)
          phase2(k);
        if (counter_ % num_sites_ == site_id_)
          _state = 3;
        else
          _state = 1;
        return 3;
      }
      break;

    case 3:
      return 3;
      break;

    default:
      return -1;
      break;
    }

    return -1;
}

int Site::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob,
                         int startIdx) {
  return -1;
}

void Site::restore(const StateSnapshot* snapshot) {
  // TODO(shoupon): implement this function
}

StateSnapshot* Site::curState() {
  // TODO(shoupon): implement this function
  return new SiteSnapshot();
}

void Site::reset() {
  commit_phases_.clear();
  commit_phases_.resize(WRAP_MULTIPLIER * num_sites_, 0);
  counter_ = 0;
}

void Site::phase1(int seq_num) {
  phaseCommit(1, seq_num);
}

void Site::phase2(int seq_num) {
  phaseCommit(2, seq_num);
}

void Site::phaseReset(int seq_num) {
  phaseCommit(0, seq_num);
}

void Site::phaseReceived(int seq_num) {
  phaseCommit(3, seq_num);
}

void Site::phaseCommit(int phase_num, int seq_num) {
  commit_phases_[seq_num % (WRAP_MULTIPLIER * num_sites_)] = phase_num;
}

void Site::broadcast(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs,
                     int seq_num) {
  for (int k = 1; k <= num_sites_; ++k) {
    if (k != site_id_) {
      outMsgs.push_back(createSiteMsg(inMsg, k, seq_num));
    }
  }
}

SiteMessage* Site::createSiteMsg(MessageTuple *inMsg, int dest_site_id,
                                  int seq_num) {
    stringstream ss;
    ss << CHANNEL_NAME << "(" << site_id_ << "," << dest_site_id << ")";
    return new SiteMessage(inMsg->srcID(), machineToInt(ss.str()),
                           inMsg->srcMsgId(), messageToInt(SITEMSG),
                           macId(), seq_num) ;
}

SiteMessage::SiteMessage(int src, int dest, int srcMsg, int destMsg,
                         int subject, int seq_num)
    : MessageTuple(src, dest, srcMsg, destMsg, subject),
      sequence_num_(seq_num) {
  ;
}

