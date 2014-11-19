//
//  abort-state.cpp
//  fail-safe-bdcast
//
//  Created by Shou-pon Lin on 11/19/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "abort-state.h"

AbortState::AbortState(GlobalState* gs)
    : StoppingState(gs) {
  ;
}

AbortState::~AbortState() {
  ;
}

bool AbortState::match(const GlobalState* gs) {
  const vector<StateSnapshot*>& snapshots = gs->getStateVec();
  for (auto& p : site_locations_) {
    if (snapshots[p]->curStateId() != 3)
      return false;
  }
  return true;
}
