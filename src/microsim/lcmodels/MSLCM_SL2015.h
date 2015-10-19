/****************************************************************************/
/// @file    MSLCM_SL2015.h
/// @author  Jakob Erdmann
/// @date    Tue, 06.10.2015
/// @version $Id: MSLCM_SL2015.h 18095 2015-03-17 09:39:00Z namdre $
///
// A lane change model for heterogeneous traffic (based on sub-lanes)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSLCM_SL2015_h
#define MSLCM_SL2015_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSAbstractLaneChangeModel.h"
#include <vector>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLCM_SL2015
 * @brief A lane change model developed by J. Erdmann
 */
class MSLCM_SL2015 : public MSAbstractLaneChangeModel {
public:

    enum MyLCAEnum {
        LCA_AMBLOCKINGLEADER = 1 << 16,
        LCA_AMBLOCKINGFOLLOWER = 1 << 17,
        LCA_MRIGHT = 1 << 18,
        LCA_MLEFT = 1 << 19,
        // !!! never set LCA_UNBLOCK = 1 << 20,
        LCA_AMBLOCKINGFOLLOWER_DONTBRAKE = 1 << 21,
        // !!! never used LCA_AMBLOCKINGSECONDFOLLOWER = 1 << 22,
        LCA_CHANGE_TO_HELP = 1 << 23,
        // !!! never read LCA_KEEP1 = 1 << 24,
        // !!! never used LCA_KEEP2 = 1 << 25,
        LCA_AMBACKBLOCKER = 1 << 26,
        LCA_AMBACKBLOCKER_STANDING = 1 << 27
    };


    MSLCM_SL2015(MSVehicle& v);

    virtual ~MSLCM_SL2015();

    /** @brief Called to examine whether the vehicle wants to change
     * using the given laneOffset.
     * This method gets the information about the surrounding vehicles
     * and whether another lane may be more preferable */
    int wantsChangeSublane(int laneOffset,
            const MSLeaderDistanceInfo& leaders,
            const MSLeaderDistanceInfo& followers,
            const MSLeaderDistanceInfo& blockers,
            const MSLeaderDistanceInfo& neighLeaders,
            const MSLeaderDistanceInfo& neighFollowers,
            const MSLeaderDistanceInfo& neighBlockers,
            const MSLane& neighLane,
            const std::vector<MSVehicle::LaneQ>& preb,
            MSVehicle** lastBlocked,
            MSVehicle** firstBlocked,
            SUMOReal& latDist, int& blocked);

    void* inform(void* info, MSVehicle* sender);

    /** @brief Called to adapt the speed in order to allow a lane change.
     *
     * @param min The minimum resulting speed
     * @param wanted The aspired speed of the car following model
     * @param max The maximum resulting speed
     * @param cfModel The model used
     * @return the new speed of the vehicle as proposed by the lane changer
     */
    SUMOReal patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max,
                        const MSCFModel& cfModel);
    /** helper function which contains the actual logic */
    SUMOReal _patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max,
                         const MSCFModel& cfModel);

    void changed(int dir);

    void prepareStep();


protected:

    /// @brief helper function for doing the actual work
    int _wantsChangeSublane(
        int laneOffset,
        const MSLeaderDistanceInfo& leaders,
        const MSLeaderDistanceInfo& followers,
        const MSLeaderDistanceInfo& blockers,
        const MSLeaderDistanceInfo& neighLeaders,
        const MSLeaderDistanceInfo& neighFollowers,
        const MSLeaderDistanceInfo& neighBlockers,
        const MSLane& neighLane,
        const std::vector<MSVehicle::LaneQ>& preb,
        MSVehicle** lastBlocked,
        MSVehicle** firstBlocked,
        SUMOReal& latDist, int& blocked);


    /* @brief decide whether we will overtake or follow blocking leaders
     * and inform them accordingly (see informLeader)
     * If we decide to follow, myVSafes will be extended
     * returns the planned speed if following or -1 if overtaking */
    SUMOReal informLeaders(int blocked, int dir,
                          const MSLeaderDistanceInfo& leaders,
                          const MSLeaderDistanceInfo& neighLeaders,
                          SUMOReal remainingSeconds);

    /// @brief decide how to deal with direct obstructions (could be leaders or followers)
    void informBlockers(int blocked, int dir,
                        const MSLeaderDistanceInfo& blockers,
                        const MSLeaderDistanceInfo& neighBlockers,
                        SUMOReal remainingSeconds,
                        SUMOReal plannedSpeed);

    /// @brief call informFollower for multiple followers
    void informFollowers(int blocked, int dir,
                        const MSLeaderDistanceInfo& followers,
                        const MSLeaderDistanceInfo& neighFollowers,
                        SUMOReal remainingSeconds,
                        SUMOReal plannedSpeed);

    /* @brief decide whether we will overtake or follow a blocking leader
     * and inform it accordingly
     * If we decide to follow, myVSafes will be extended
     * returns the planned speed if following or -1 if overtaking */
    SUMOReal informLeader(int blocked, int dir,
                          const CLeaderDist& neighLead,
                          SUMOReal remainingSeconds);

    /// @brief decide whether we will try cut in before the follower or allow to be overtaken
    void informFollower(int blocked, int dir,
                        const CLeaderDist& neighFollow,
                        SUMOReal remainingSeconds,
                        SUMOReal plannedSpeed);


    /// @brief compute useful slowdowns for blocked vehicles
    int slowDownForBlocked(MSVehicle** blocked, int state);

    /// @brief save space for vehicles which need to counter-lane-change
    void saveBlockerLength(const MSVehicle* blocker, int lcaCounter);

    /// @brief reserve space at the end of the lane to avoid dead locks
    inline void saveBlockerLength(SUMOReal length) {
        myLeadingBlockerLength = MAX2(length, myLeadingBlockerLength);
    };

    inline bool amBlockingLeader() {
        return (myOwnState & LCA_AMBLOCKINGLEADER) != 0;
    }
    inline bool amBlockingFollower() {
        return (myOwnState & LCA_AMBLOCKINGFOLLOWER) != 0;
    }
    inline bool amBlockingFollowerNB() {
        return (myOwnState & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0;
    }
    inline bool amBlockingFollowerPlusNB() {
        return (myOwnState & (LCA_AMBLOCKINGFOLLOWER | LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)) != 0;
    }
    inline bool currentDistDisallows(SUMOReal dist, int laneOffset, SUMOReal lookForwardDist) {
        return dist / (abs(laneOffset)) < lookForwardDist;
    }
    inline bool currentDistAllows(SUMOReal dist, int laneOffset, SUMOReal lookForwardDist) {
        return dist / abs(laneOffset) > lookForwardDist;
    }

    /// @brief information regarding save velocity (unused) and state flags of the ego vehicle
    typedef std::pair<SUMOReal, int> Info;


    /// @brief XXX
    void updateExpectedSublaneSpeeds(const std::vector<MSVehicle::LaneQ>& preb);

    /// @brief send a speed recommendation to the given vehicle
    void msg(const CLeaderDist& cld, SUMOReal speed, int state);

    /// @brief compute shift so that prevSublane + shift = newSublane
    int computeSublaneShift(const MSEdge* prevEdge, const MSEdge* curEdge);

    /// @brief get the longest vehicle in the given info
    static CLeaderDist getLongest(const MSLeaderDistanceInfo& ldi);

    /// @brief get the slowest vehicle in the given info
    static CLeaderDist getSlowest(const MSLeaderDistanceInfo& ldi);

    /// @brief determine blocking state depending on latDist
    int checkBlocking(SUMOReal latDist, int laneOffset,
                const MSLeaderDistanceInfo& leaders,
                const MSLeaderDistanceInfo& followers,
                const MSLeaderDistanceInfo& blockers,
                const MSLeaderDistanceInfo& neighLeaders,
                const MSLeaderDistanceInfo& neighFollowers,
                const MSLeaderDistanceInfo& neighBlockers) const; 

    static int checkBlockingLeaders(const MSVehicle* vehicle, const MSLeaderDistanceInfo& leaders, int laneOffset, SUMOReal latDist); 
    static int checkBlockingFollowers(const MSVehicle* vehicle, const MSLeaderDistanceInfo& followers, int laneOffset, SUMOReal latDist); 
    static bool overlap(SUMOReal right, SUMOReal left, SUMOReal right2, SUMOReal left2); 

protected:
    /// @brief a value for tracking the probability that a change to the right is beneficial
    SUMOReal mySpeedGainProbabilityRight;
    /// @brief a value for tracking the probability that a change to the left is beneficial
    SUMOReal mySpeedGainProbabilityLeft;

    /* @brief a value for tracking the probability of following the/"Rechtsfahrgebot"
     * A larger negative value indicates higher probability for moving to the
     * right (as in mySpeedGainProbability) */
    SUMOReal myKeepRightProbability;

    SUMOReal myLeadingBlockerLength;
    SUMOReal myLeftSpace;

    /*@brief the speed to use when computing the look-ahead distance for
     * determining urgency of strategic lane changes */
    SUMOReal myLookAheadSpeed;

    /// @brief speed adaptation requests by ego and surrounding vehicles 
    std::vector<SUMOReal> myVSafes;

    /// @brief expected travel speeds on all sublanes on the current edge(!)
    std::vector<SUMOReal> myExpectedSublaneSpeeds;

    SUMOReal myLastEdgeWidth;

    /// @brief flag to prevent speed adaptation by slowing down
    bool myDontBrake;

};


#endif

/****************************************************************************/

