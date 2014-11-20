//
//  commit-received-checker.h
//  fail-safe-bdcast 
//
//  Created by Shou-pon Lin on 11/20/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#ifndef FAILSAFEBDCAST_COMMITRECEIVEDCHECKER_H
#define FAILSAFEBDCAST_COMMITRECEIVEDCHECKER_H

#include <cassert>

#include <iostream>
#include <vector>
using namespace std;

#include "../prob_verify/statemachine.h"
#include "../prob_verify/checker.h"

#include "site.h"

// A stateless checker, only looks at the current state. Therefore no customized
// checker state is required for CommitReceivedChecker
class CommitReceivedChecker: public Checker {
  static vector<int> site_locations_;
public:
  CommitReceivedChecker() {}
  bool check(CheckerState* checker_state,
             const vector<StateSnapshot*>& machine_states);
  CheckerState* initState() { return new CheckerState(); }

  static void addSiteLocations(int pos) { site_locations_.push_back(pos); }
private:
};

#endif // FAILSAFEBDCAST_COMMITRECEIVEDCHECKER_H
