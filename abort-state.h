//
//  abort-state.h
//  fail-safe-bdcast
//
//  Created by Shou-pon Lin on 11/19/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#ifndef FAILSAFEBDCAST_ABORTSTATE_H
#define FAILSAFEBDCAST_ABORTSTATE_H

#include "../prob_verify/stoppingstate.h"

class AbortState: public StoppingState {
public:
  AbortState(GlobalState* gs);
  virtual ~AbortState();
  virtual bool match(const GlobalState* gs);
  void addSiteLocation(int pos) { site_locations_.push_back(pos); }
  virtual string toString() { return string("All sites have state = 3 (aborted)"); }
private:
  vector<int> site_locations_;
};

#endif // FAILSAFEBDCAST_ABORTSTATE_H
