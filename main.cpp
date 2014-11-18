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

#include "../prob_verify/parser.h"
#include "../prob_verify/pverify.h"
#include "../prob_verify/define.h"
#include "../prob_verify/sync.h"

#include "channel.h"
#include "site.h"

#include "abort-delay-checker.h"

#include "abort-delay-checker.h"

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

int kNumSites = 3;
int kNumChannels = kNumSites * (kNumSites - 1);
int kNumParties = kNumSites + kNumChannels;

int main( int argc, char* argv[] ) {
  try {
    // Declare the names of component machines so as to register these names as
    // id's in the parser
    Parser* psrPtr = new Parser() ;
    // Create StateMachine objects
    // Add the state machines into ProbVerifier
    // Register the machines that are triggered by deadline (sync)
    Lookup* msg_table = psrPtr->getMsgTable();
    Lookup* mac_table = psrPtr->getMacTable();
    StateMachine::setLookup(msg_table, mac_table);
    Sync::setRecurring(1);
    Sync* sync = new Sync(1, msg_table, mac_table);
    pvObj.addMachine(sync);

    // for each state machine:
    // 1. allocate the state machine and record the pointer
    // 2. register the state machine with ProbVerifier pvObj
    // 3. register the state machine with Sync controller
    // 4. group several machines that rely on the same timing stack as a single
    //    failure group, and register the group with Sync controller
    Site::setNumSites(3);
    vector<Site*> sites;
    vector<Channel*> channels;
    for (int i = 1; i <= kNumSites; ++i) {
      sites.push_back(new Site(msg_table, mac_table, i));
      pvObj.addMachine(sites.back());
      AbortDelayCheckerState::addSiteLocation(pvObj.getNumMachines() - 1);
      if (i == 1)
        sync->setMaster(sites.back());
      else
        sync->addMachine(sites.back());
      
      //vector<const StateMachine*> failure_group;
      //failure_group.push_back(sites.back());
      for (int j = 1; j <= kNumSites; ++j) {
        if (i != j) {
          channels.push_back(new Channel(msg_table, mac_table, i, j));
          pvObj.addMachine(channels.back());
          sync->addMachine(channels.back());
          //failure_group.push_back(channels.back());
        }
      }
      //sync->addFailureGroup(failure_group);
    }

    // Initialize AbortDelayChecker
    AbortDelayChecker ad_checker;
    AbortDelayCheckerState ad_checker_state;
    // Add checker into ProbVerifier
    pvObj.addChecker(&ad_checker);
    
    // Add a default service (stub)
    Service *srvc = new Service();
    srvc->reset();
    GlobalState::setService(srvc);

    // Specify the starting state
    GlobalState* startPoint = new GlobalState(pvObj.getMachinePtrs(),
                                              &ad_checker_state);
    startPoint->setParser(psrPtr);

    // Specify the global states in the set RS (stopping states)
    // initial state
    StoppingState stop1(startPoint);
    for (auto s_ptr : sites)
      stop1.addAllow(new SiteSnapshot(), s_ptr->macId());
    for (auto c_ptr : channels)
      stop1.addAllow(new ChannelSnapshot(), c_ptr->macId());

    /*
    StoppingState stop1a(startPoint);
    stop1a.addAllow(new StateSnapshot(1), 1) ;      // merge
    stop1a.addAllow(new LockSnapshot(1,0,REQUEST), 4) ;    // lock 0
    stop1a.addAllow(new StateSnapshot(0), 7) ;      // trbp
    stop1a.addAllow(new StateSnapshot(0), 8) ;      // icc merge
    stop1a.addAllow(new StateSnapshot(1), 11) ;     // driver
    pvObj.addSTOP(&stop1a);
    */

    pvObj.addPrintStop(printStop) ;
        
    // Start the procedure of probabilistic verification.
    // Specify the maximum probability depth to be explored
    pvObj.start(3);
        
    // When complete, deallocate all machines
    delete sync ;
    for (auto s_ptr : sites)
      delete s_ptr;
    for (auto c_ptr : channels)
      delete c_ptr;
    delete startPoint;
    delete psrPtr;
        
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

