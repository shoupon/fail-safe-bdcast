//
//  abort-delay-checker.h
//  verification-merge
//
//  Created by Shou-pon Lin on 11/18/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "abort-delay-checker.h"

vector<int> AbortDelayCheckerState::site_locations_;
int AbortDelayCheckerState::num_sites_;

bool AbortDelayChecker::check(CheckerState* checker_state,
                              const vector<StateSnapshot*>& machine_states) {
  assert(typeid(*checker_state) == typeid(AbortDelayCheckerState));
  AbortDelayCheckerState *ad_checker_state =
      dynamic_cast<AbortDelayCheckerState*>(checker_state);

  if (ad_checker_state->exists_aborted_) {
    if (++ad_checker_state->num_ticks_after_abort_ <
        ad_checker_state->num_sites_)
      return true;
    for (auto& p : ad_checker_state->site_locations_) {
      if (((SiteSnapshot*)(machine_states[p]))->getCounter() ==
          ad_checker_state->counter_)
        return true;
      if (p != ad_checker_state->exists_aborted_ && 
          machine_states[p]->curStateId() != 3)
        return false;
    }
  } else {
    for (auto& p : ad_checker_state->site_locations_) {
      if (machine_states[p]->curStateId() == 3) {
        ad_checker_state->exists_aborted_ = p;
        ad_checker_state->counter_ =
            ((SiteSnapshot*)(machine_states[p]))->getCounter();
        break;
      }
    }
  }
  return true;
}
