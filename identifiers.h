//
//  identifiers.h
//  verification-merge
//
//  Created by Shou-pon Lin on 4/14/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#ifndef IDENTIFIERS_H
#define IDENTIFIERS_H

// List of machine names
#define SITE_NAME "site"
#define SITE_0_NAME "site(0)"
#define SITE_1_NAME "site(1)"
#define SITE_2_NAME "site(2)"
#define CHANNEL_NAME "channel"
#define CHANNEL_0_1_NAME "channel(0,1)"
#define CHANNEL_0_2_NAME "channel(0,2)"
#define CHANNEL_1_0_NAME "channel(1,0)"
#define CHANNEL_1_2_NAME "channel(1,2)"
#define CHANNEL_2_0_NAME "channel(2,0)"
#define CHANNEL_2_1_NAME "channel(2,1)"
#define LOCK_NAME "lock"
#define LOCK_0_NAME "lock(0)"
#define LOCK_1_NAME "lock(1)"
#define LOCK_2_NAME "lock(2)"
#define CRUISE_FRONT_NAME "cruise(f)"
#define CRUISE_BACK_NAME "cruise(b)"
#define CRUISE_MERGE_NAME "cruise(m)"
#define TRBP_NAME "trbp"
#define FRONT_NAME "front"
#define BACK_NAME "back"
#define MERGE_NAME "merge"
#define DRIVER_NAME "driver"
#define SENSOR_NAME "sensor"
#define SYNC_NAME "sync"

// List of message names
// Site
#define SITEMSG "SITEMSG"
#define REQUIRE "REQUIRE"
// Lock
#define ATTEMPT "ATTEMPT"
#define SUCCESS "SUCCESS"
#define COOPERATE "COOPERATE"
// Cruise control
#define RESET "RESET"
#define MAINTAIN "MAINTAIN"
#define MAKEGAP "MAKE-GAP"
#define ALIGN "ALIGN"
// TRBP
#define COMMLOSS "COMM-LOSS"
#define TRBPON "TRBPon"
#define TRBPOFF "TRBPoff"
#define TRBPFAIL "TRBPfail"
// Sensor
#define EMERGENCY "EMERGENCY"
#define GAPTAKEN "GAP-TAKEN"
#define GAPREADY "GAP-READY"
#define INCONSISTENT "INCONSISTENT-READINGS"
#define SENSORON "SENSORon"
#define SENSOROFF "SENSORoff"
#define SENSORFAIL "SENSORfail"
// Driver
#define SIGNAL "SIGNAL"
#define CANCEL "CANCEL"
#define ABORT "ABORT"
#define FINISH "FINISH"
#define GREENLIGHT "GREENLIGHT"
// Merge
#define REQUEST "REQUEST"
#define CREATE "CREATE"
#define MOVE "MOVE"


#endif // endif(IDENTIFIERS_H)
