//
//  commit-received-checker.cpp
//  fail-safe-bdcast 
//
//  Created by Shou-pon Lin on 11/20/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "commit-received-checker.h"

vector<int> CommitReceivedChecker::site_locations_;

bool CommitReceivedChecker::check(CheckerState* checker_state,
                                  const vector<StateSnapshot*>& machine_states) {
  assert(site_locations_.size());
  assert(typeid(*(machine_states[site_locations_.front()])) ==
         typeid(SiteSnapshot));
  SiteSnapshot* site_snapshot = dynamic_cast<SiteSnapshot*>(
      machine_states[site_locations_.front()]);
  int buffer_size = site_snapshot->getBufferSize();
  for (int idx = 0; idx < buffer_size; ++idx) {
    int exists_zero = 0;
    int exists_one = 0;
    for (auto& p : site_locations_) {
      assert(typeid(*(machine_states[p])) == typeid(SiteSnapshot));
      SiteSnapshot* p_snapshot = dynamic_cast<SiteSnapshot*>(machine_states[p]);
      if (p_snapshot->getPhase(idx) == 0)
        exists_zero = 1;
      else if(p_snapshot->getPhase(idx) == 1)
        exists_one = 1;
      if (exists_zero && exists_one)
        return false;
    }
  }
  return true;
}
