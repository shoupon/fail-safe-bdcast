//
//  site.h
//  fail-safe-bdcast
//
//  Created by Shou-pon Lin on 11/17/14
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#ifndef FAILSAFEBDCAST_SITE_H
#define FAILSAFEBDCAST_SITE_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

#include "../prob_verify/statemachine.h"
#include "../prob_verify/sync.h"
#include "identifiers.h"

#define WRAP_MULTIPLIER 2

class SiteSnapshot;
class SiteMessage;

class Site: public StateMachine {
  static int num_sites_;
public:
  Site(Lookup* msg, Lookup* mac, int id);
  ~Site() {}
  int transit(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
              bool& high_prob, int startIdx = 0);
  int nullInputTrans(vector<MessageTuple*>& outMsgs,
                     bool& high_prob, int startIdx = 0);        
  void restore(const StateSnapshot *snapshot);
  StateSnapshot* curState();
  void reset();
  static void setNumSites(int n) { num_sites_ = n; }
  static int getNumSites() { return num_sites_; }

private:
  void phase1(int seq_num);
  void phase2(int seq_num);
  void phaseReset(int seq_num);
  void phaseReceived(int seq_num);
  void phaseCommit(int phase_num, int seq_num);

  int incrementCounter();
  void broadcast(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs,
                 int seq_num);
  SiteMessage* createSiteMsg(MessageTuple* inMsg, int dest_site_id,
                             int seq_num);

  int site_id_;
  int counter_;
  vector<int> commit_phases_;
};

class SiteMessage: public MessageTuple {
public:
  SiteMessage(int src, int dest, int srcMsg, int destMsg, int subject,
              int seq_num);
  int getSequenceNumber() { return sequence_num_; }

private:
  int sequence_num_;
};

class SiteSnapshot: public StateSnapshot {
  friend class Site;
public:
  SiteSnapshot()
      : ss_state_(0),
        ss_counter_(0),
        ss_commit_phases_(Site::getNumSites() * WRAP_MULTIPLIER, 0) {}
  SiteSnapshot(int state, int counter, const vector<int>& phases)
      : ss_state_(state), ss_counter_(counter), ss_commit_phases_(phases) {}
  ~SiteSnapshot() {}
  int curStateId() const { return ss_state_; }
  string toString();
  int toInt();
  StateSnapshot* clone() const;
  bool match(StateSnapshot* other);

  int getCounter() const { return ss_counter_; }
private:
  bool sameArray(const vector<int>& lhs, const vector<int>& rhs);

  const int ss_state_;
  const int ss_counter_;
  const vector<int> ss_commit_phases_;
};

#endif // FAILSAFEBDCAST_SITE_H
