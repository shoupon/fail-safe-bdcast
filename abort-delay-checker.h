//
//  abort-delay-checker.h
//  verification-merge
//
//  Created by Shou-pon Lin on 11/18/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#ifndef FAILSAFEBDCAST_ABORTDELAYCHECKER_H
#define FAILSAFEBDCAST_ABORTDELAYCHECKER_H

#include <cassert>

#include <iostream>
#include <vector>
using namespace std;

#include "../prob_verify/statemachine.h"
#include "../prob_verify/checker.h"

#include "site.h"

class AbortDelayChecker;

class AbortDelayCheckerState: public CheckerState {
  friend class AbortDelayChecker;
  static vector<int> site_locations_;
  static int num_sites_;
public:
  AbortDelayCheckerState()
      : exists_aborted_(0), counter_(0), num_ticks_after_abort_(0) {}
  AbortDelayCheckerState(int aborted, int counter, int num_ticks)
      : exists_aborted_(aborted), counter_(counter),
        num_ticks_after_abort_(num_ticks) {}
  AbortDelayCheckerState* clone() {
    return new AbortDelayCheckerState(exists_aborted_, counter_,
                                      num_ticks_after_abort_);
  }

  static void addSiteLocation(int pos) { site_locations_.push_back(pos); }
  static void setNumSites(int n) { num_sites_ = n; }
private:
  int exists_aborted_;
  int counter_;
  int num_ticks_after_abort_;
};

class AbortDelayChecker: public Checker {
public:
  AbortDelayChecker() {}
  bool check(CheckerState* checker_state, 
             const vector<StateSnapshot*>& machine_states);
  AbortDelayCheckerState* initState() { return new AbortDelayCheckerState(); }
};

#endif // FAILSAFEBDCAST_ABORTDELAYCHECKER_H
