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
public:
  AbortDelayCheckerState(): exists_aborted_(0), counter_(0) {}
  AbortDelayCheckerState(int aborted, int counter)
      : exists_aborted_(aborted), counter_(counter) {}
  AbortDelayCheckerState* clone() {
    return new AbortDelayCheckerState(exists_aborted_, counter_);
  }

  static void addSiteLocation(int pos) { site_locations_.push_back(pos); }
private:
  int exists_aborted_;
  int counter_;
};

class AbortDelayChecker: public Checker {
public:
  AbortDelayChecker() {}
  bool check(CheckerState* checker_state, 
             const vector<StateSnapshot*>& machine_states);
  AbortDelayCheckerState* initState() { return new AbortDelayCheckerState(); }
};

#endif // FAILSAFEBDCAST_ABORTDELAYCHECKER_H
