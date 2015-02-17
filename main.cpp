//
//  main.cpp
//  secure
//
//  Created by Shou-pon Lin on 8/22/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include <iostream>
#include <vector>
#include <exception>
#include <stdexcept>
using namespace std;

#include "../prob_verify/pverify.h"
#include "../prob_verify/define.h"
#include "../prob_verify/sync.h"

#include "channel.h"
#include "site.h"

#include "abort-delay-checker.h"
#include "commit-received-checker.h"
#include "abort-state.h"

// the flag CHECK_GUARANTEE_1 has precedence over CHECK_GUARANTEE_2, i.e., when
// CHECK_GUARANTEE_1 is defined, then all statements under CHECK_GUARANTEE_2
// will not be compiled
#define CHECK_GUARANTEE_1
#define CHECK_GUARANTEE_2

#define NUM_SITES 3

ProbVerifier pvObj ;
GlobalState* startPoint;

bool printStop(GlobalState* left, GlobalState* right)
{
    /*
     StoppingState stopZero(left);
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 1); // lock 0
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 2); // lock 1
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 3); // lock 2
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 4); // lock 3
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 5); // lock 4
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 6); // lock 5
     //stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 7); // lock 6
     */
    /*
     if( stopZero.match(left) && stopZero.match(right))
     return true ;
     //return false;
     }
     else
     return false;*/
    return true;
}

vector<Site*> sites;
vector<Channel*> channels;

void setupCommitState(StoppingState& stop, int state, int counter,
                      const vector<int>& phases) {
  for (auto s_ptr : sites)
    stop.addAllow(new SiteSnapshot(state, counter, phases),
                  s_ptr->macId() - 1);
  for (auto c_ptr : channels)
    stop.addAllow(new ChannelSnapshot(vector<bool>(NUM_SITES - 1, true)),
                  c_ptr->macId() - 1);
}

void mutateInitState(GlobalState* init, int state, int counter,
                     const vector<int>& phases) {
  SiteSnapshot* snapshot;
  for (auto s_ptr : sites) {
    snapshot = new SiteSnapshot(state, counter, phases);
    init->mutateState(snapshot, s_ptr->macId() - 1);
    delete snapshot;
  }
}

int kNumSites = NUM_SITES;
int kNumChannels = kNumSites * (kNumSites - 1);
int kNumParties = kNumSites + kNumChannels;

int main( int argc, char* argv[] ) {
  try {
    // Create StateMachine objects
    // Add the state machines into ProbVerifier
    // Register the machines that are triggered by deadline (sync)
    unique_ptr<Lookup> message_lookup(new Lookup());
    unique_ptr<Lookup> machine_lookup(new Lookup());
    StateMachine::setLookup(message_lookup.get(), machine_lookup.get());
    StateMachine::setLookup(message_lookup.get(), machine_lookup.get());
    Sync::setRecurring(1);
    Sync* sync = new Sync(1, message_lookup.get(), machine_lookup.get());
    pvObj.addMachine(sync);

    // for each state machine:
    // 1. allocate the state machine and record the pointer
    // 2. register the state machine with ProbVerifier pvObj
    // 3. register the state machine with Sync controller
    // 4. group several machines that rely on the same timing stack as a single
    //    failure group, and register the group with Sync controller
    Site::setNumSites(kNumSites);
    vector<int> site_locations;
    for (int i = 1; i <= kNumSites; ++i) {
      sites.push_back(new Site(message_lookup.get(), machine_lookup.get(), i));
      pvObj.addMachine(sites.back());
      site_locations.push_back(pvObj.getNumMachines() - 1);
      if (i == 1)
        sync->setMaster(sites.back());
      else
        sync->addMachine(sites.back());
      
      //vector<const StateMachine*> failure_group;
      //failure_group.push_back(sites.back());
      vector<int> destinations;
      for (int j = 1; j <= kNumSites; ++j) {
        if (i != j)
          destinations.push_back(j);
      }
      channels.push_back(new Channel(message_lookup.get(),
                                     machine_lookup.get(),
                                     i, destinations));
      pvObj.addMachine(channels.back());
      sync->addMachine(channels.back());
      //failure_group.push_back(channels.back());
      //sync->addFailureGroup(failure_group);
    }

#if defined(CHECK_GUARANTEE_1)
    // Initialize AbortDelayChecker
    AbortDelayCheckerState::setNumSites(kNumSites);
    for (auto& p : site_locations)
      AbortDelayCheckerState::addSiteLocation(p);
    AbortDelayChecker ad_checker;
    AbortDelayCheckerState ad_checker_state;
    // Add checker into ProbVerifier
    pvObj.addChecker(&ad_checker);
#elif defined(CHECK_GUARANTEE_2)
    // Initialize CommitReceivedChecker
    CommitReceivedChecker cr_checker;
    for (auto& p : site_locations)
      cr_checker.addSiteLocations(p);
    CheckerState dummy_checker_state;
    pvObj.addChecker(&cr_checker);
#else
    Checker dummy_checker;
    CheckerState dummy_checker_state;
    pvObj.addChecker(&dummy_checker);
#endif
    
    // Add a default service (stub)
    Service *srvc = new Service();
    srvc->reset();
    GlobalState::setService(srvc);

    // Specify the starting state
#if defined(CHECK_GUARANTEE_1)
    GlobalState* startPoint = new GlobalState(pvObj.getMachinePtrs(),
                                              &ad_checker_state);
#else
    GlobalState* startPoint = new GlobalState(pvObj.getMachinePtrs(),
                                              &dummy_checker_state);
#endif

    // Specify the global states in the set RS (stopping states)
    // initial state
    /*
    StoppingState stop1(startPoint);
    for (auto s_ptr : sites)
      stop1.addAllow(new SiteSnapshot(), s_ptr->macId() - 1);
    for (auto c_ptr : channels)
      stop1.addAllow(new ChannelSnapshot(), c_ptr->macId() - 1);
    pvObj.addSTOP(&stop1);
    */
    // the 0-th machine in the lookup table of the machine names if "nul"
    // the first machine is "sync", then the second machine is "site(1)" and so
    // on, while the 0-th element in pointer array pvObj._machines is the
    // pointer to machine Sync, therefore the -1 offset.
    // TODO(shoupon): find a good way to fix this

    // another potential stopping state
    // (0,5,[1,1,1,3,3,3]),[],[],
    // (0,5,[1,1,1,3,3,3]),[],[],
    // (0,5,[1,1,1,3,3,3]),[],[]
    
#if (NUM_SITES == 3)
    vector<int> commit_0_5 {1, 1, 1, 3, 3, 3};
    mutateInitState(startPoint, 0, 5, commit_0_5);

    StoppingState stop_0_5(startPoint);
    setupCommitState(stop_0_5, 0, 5, commit_0_5);
    pvObj.addSTOP(&stop_0_5);

    StoppingState stop_0_4(startPoint);
    vector<int> commit_0_4 {1, 1, 3, 3, 3, 1};
    setupCommitState(stop_0_4, 0, 4, commit_0_4);
    pvObj.addSTOP(&stop_0_4);

    StoppingState stop_0_3(startPoint);
    vector<int> commit_0_3 {1, 3, 3, 3, 1, 1};
    setupCommitState(stop_0_3, 0, 3, commit_0_3);
    pvObj.addSTOP(&stop_0_3);

    StoppingState stop_0_2(startPoint);
    vector<int> commit_0_2 {3, 3, 3, 1, 1, 1};
    setupCommitState(stop_0_2, 0, 2, commit_0_2);
    pvObj.addSTOP(&stop_0_2);

    StoppingState stop_0_1(startPoint);
    vector<int> commit_0_1 {3, 3, 1, 1, 1, 3};
    setupCommitState(stop_0_1, 0, 1, commit_0_1);
    pvObj.addSTOP(&stop_0_1);

    StoppingState stop_0_0(startPoint);
    vector<int> commit_0_0 {3, 1, 1, 1, 3, 3};
    setupCommitState(stop_0_0, 0, 0, commit_0_0);
    pvObj.addSTOP(&stop_0_0);

    StoppingState error_0_2(startPoint);
    vector<int> no_commit {3, 3, 3, 3, 3, 3};
    for (auto s_ptr : sites)
      error_0_2.addAllow(new SiteSnapshot(0, 2, no_commit),
                         s_ptr->macId() - 1);
    for (auto c_ptr : channels) {
      error_0_2.addAllow(
        new ChannelSnapshot(vector<bool>(NUM_SITES - 1, true)),
        c_ptr->macId() - 1);
    }
    pvObj.addError(&error_0_2);
#elif (NUM_SITES == 4)
    vector<int> commit_0_7 {1, 1, 1, 1, 3, 3, 3, 3};
    mutateInitState(startPoint, 0, 7, commit_0_7);

    StoppingState stop_0_7(startPoint);
    setupCommitState(stop_0_7, 0, 7, commit_0_7);
    pvObj.addSTOP(&stop_0_7);

    StoppingState stop_0_6(startPoint);
    vector<int> commit_0_6 {1, 1, 1, 3, 3, 3, 3, 1};
    setupCommitState(stop_0_6, 0, 6, commit_0_6);
    pvObj.addSTOP(&stop_0_6);

    StoppingState stop_0_5(startPoint);
    vector<int> commit_0_5 {1, 1, 3, 3, 3, 3, 1, 1};
    setupCommitState(stop_0_5, 0, 5, commit_0_5);
    pvObj.addSTOP(&stop_0_5);

    StoppingState stop_0_4(startPoint);
    vector<int> commit_0_4 {1, 3, 3, 3, 3, 1, 1, 1};
    setupCommitState(stop_0_4, 0, 4, commit_0_4);
    pvObj.addSTOP(&stop_0_4);

    StoppingState stop_0_3(startPoint);
    vector<int> commit_0_3 {3, 3, 3, 3, 1, 1, 1, 1};
    setupCommitState(stop_0_3, 0, 3, commit_0_3);
    pvObj.addSTOP(&stop_0_3);

    StoppingState stop_0_2(startPoint);
    vector<int> commit_0_2 {3, 3, 3, 1, 1, 1, 1, 3};
    setupCommitState(stop_0_2, 0, 2, commit_0_2);
    pvObj.addSTOP(&stop_0_2);

    StoppingState stop_0_1(startPoint);
    vector<int> commit_0_1 {3, 3, 1, 1, 1, 1, 3, 3};
    setupCommitState(stop_0_1, 0, 1, commit_0_1);
    pvObj.addSTOP(&stop_0_1);

    StoppingState stop_0_0(startPoint);
    vector<int> commit_0_0 {3, 1, 1, 1, 1, 3, 3, 3};
    setupCommitState(stop_0_0, 0, 0, commit_0_0);
    pvObj.addSTOP(&stop_0_0);

    StoppingState error_0_2(startPoint);
    vector<int> no_commit {3, 3, 3, 3, 3, 3, 3, 3};
    for (auto s_ptr : sites)
      error_0_2.addAllow(new SiteSnapshot(0, 2, no_commit),
                         s_ptr->macId() - 1);
    for (auto c_ptr : channels) {
      error_0_2.addAllow(
        new ChannelSnapshot(vector<bool>(NUM_SITES - 1, true)),
        c_ptr->macId() - 1);
    }
    pvObj.addError(&error_0_2);
#elif (NUM_SITES == 5)
    vector<int> commit_0_9 {1, 1, 1, 1, 1, 3, 3, 3, 3, 3};
    mutateInitState(startPoint, 0, 9, commit_0_9);
  
    StoppingState stop_0_9(startPoint);
    setupCommitState(stop_0_9, 0, 9, commit_0_9);
    pvObj.addSTOP(&stop_0_9);

    StoppingState stop_0_8(startPoint);
    vector<int> commit_0_8 {1, 1, 1, 1, 3, 3, 3, 3, 3, 1};
    setupCommitState(stop_0_8, 0, 8, commit_0_8);
    pvObj.addSTOP(&stop_0_8);

    StoppingState stop_0_7(startPoint);
    vector<int> commit_0_7 {1, 1, 1, 3, 3, 3, 3, 3, 1, 1};
    setupCommitState(stop_0_7, 0, 7, commit_0_7);
    pvObj.addSTOP(&stop_0_7);

    StoppingState stop_0_6(startPoint);
    vector<int> commit_0_6 {1, 1, 3, 3, 3, 3, 3, 1, 1, 1};
    setupCommitState(stop_0_6, 0, 6, commit_0_6);
    pvObj.addSTOP(&stop_0_6);

    StoppingState stop_0_5(startPoint);
    vector<int> commit_0_5 {1, 3, 3, 3, 3, 3, 1, 1, 1, 1};
    setupCommitState(stop_0_5, 0, 5, commit_0_5);
    pvObj.addSTOP(&stop_0_5);

    StoppingState stop_0_4(startPoint);
    vector<int> commit_0_4 {3, 3, 3, 3, 3, 1, 1, 1, 1, 1};
    setupCommitState(stop_0_4, 0, 4, commit_0_4);
    pvObj.addSTOP(&stop_0_4);

    StoppingState stop_0_3(startPoint);
    vector<int> commit_0_3 {3, 3, 3, 3, 1, 1, 1, 1, 1, 3};
    setupCommitState(stop_0_3, 0, 3, commit_0_3);
    pvObj.addSTOP(&stop_0_3);

    StoppingState stop_0_2(startPoint);
    vector<int> commit_0_2 {3, 3, 3, 1, 1, 1, 1, 1, 3, 3};
    setupCommitState(stop_0_2, 0, 2, commit_0_2);
    pvObj.addSTOP(&stop_0_2);

    StoppingState stop_0_1(startPoint);
    vector<int> commit_0_1 {3, 3, 1, 1, 1, 1, 1, 3, 3, 3};
    setupCommitState(stop_0_1, 0, 1, commit_0_1);
    pvObj.addSTOP(&stop_0_1);

    StoppingState stop_0_0(startPoint);
    vector<int> commit_0_0 {3, 1, 1, 1, 1, 1, 3, 3, 3, 3};
    setupCommitState(stop_0_0, 0, 0, commit_0_0);
    pvObj.addSTOP(&stop_0_0);

    StoppingState error_0_2(startPoint);
    vector<int> no_commit {3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    for (auto s_ptr : sites)
      error_0_2.addAllow(new SiteSnapshot(0, 2, no_commit),
                         s_ptr->macId() - 1);
    for (auto c_ptr : channels) {
      error_0_2.addAllow(
        new ChannelSnapshot(vector<bool>(NUM_SITES - 1, true)),
        c_ptr->macId() - 1);
    }
    pvObj.addError(&error_0_2);
#endif

    AbortState abort_state(startPoint);
    for (auto& p : site_locations)
      abort_state.addSiteLocation(p);
    pvObj.addEND(&abort_state);

    pvObj.addPrintStop(printStop) ;
        
    ProbVerifierConfig config;
    config.setLowProbBound(0.01);
    config.setBoundMethod(DFS_BOUND);
    pvObj.configure(config);
    // Start the procedure of probabilistic verification.
    // Specify the maximum probability depth to be explored
    //pvObj.start(2);
    pvObj.start(10, startPoint);

    // When complete, deallocate all machines
    delete sync ;
    for (auto s_ptr : sites)
      delete s_ptr;
    for (auto c_ptr : channels)
      delete c_ptr;
    delete startPoint;
        
  } catch (runtime_error& re) {
    cerr << "Runtime error:" << endl
    << re.what() << endl ;
  } catch (exception e) {
    cerr << e.what() << endl;
  } catch (...) {
    cerr << "Something wrong." << endl;
  }
    
  return 0;
}

