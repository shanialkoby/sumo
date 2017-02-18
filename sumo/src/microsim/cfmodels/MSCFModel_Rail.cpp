/****************************************************************************/
/// @file    MSCFModel_Rail.cpp
/// @author  Gregor L\"ammel
/// @date    Tue, 08 Feb 2017
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <microsim/MSVehicle.h>
#include <utils/geom/GeomHelper.h>
#include "MSCFModel_Rail.h"
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>



#define G  9.80665


MSCFModel_Rail::MSCFModel_Rail(const MSVehicleType *vtype, std::string trainType) : MSCFModel(vtype, -1, -1, -1) {

    if (trainType.compare("RB425") == 0) {
        myTrainParams = initRB425Params();
    } else if (trainType.compare("RB628")) {
        myTrainParams = initRB628Params();
    } else if (trainType.compare("NGT400")) {
        myTrainParams = initNGT400Params();
    }else if (trainType.compare("NGT400_16")) {
        myTrainParams = initNGT400_16Params();
    } else if (trainType.compare("ICE1")) {
        myTrainParams = initICE1Params();
    } else if (trainType.compare("REDosto7")) {
        myTrainParams = initREDosto7Params();
    } else if (trainType.compare("Freight")) {
        myTrainParams = initFreightParams();
    } else {
        WRITE_ERROR("Unknown train type: " + trainType + ". Exiting!");
        throw ProcessError();
    }

    setMaxDecel(myTrainParams.decl);


}

MSCFModel_Rail::~MSCFModel_Rail() {

}



SUMOReal MSCFModel_Rail::followSpeed(const MSVehicle *const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed,
                                     SUMOReal predMaxDecel) const {
//TODO: impossible to answer unless the pred vehicle is known [Gregor Feb '17]
    return 0;
}




int MSCFModel_Rail::getModelID() const {
    return SUMO_TAG_CF_RAIL;
}

MSCFModel *MSCFModel_Rail::duplicate(const MSVehicleType *vtype) const {
    return nullptr;
}

SUMOReal MSCFModel_Rail::maxNextSpeed(SUMOReal speed, const MSVehicle *const veh) const {

//    MSCFModel_Rail::VehicleVariables *vars = (MSCFModel_Rail::VehicleVariables *) veh->getCarFollowVariables();
//    if (vars->isNotYetInitialized()) {
//        initVehicleVariables(veh, vars);
//    }


    if (speed >= myTrainParams.vmax) {
        return myTrainParams.vmax;
    }

    SUMOReal targetSpeed = myTrainParams.vmax;

    SUMOReal res = getInterpolatedValueFromLookUpMap(speed, &(myTrainParams.resistance)); // kN

    SUMOReal slope = veh->getSlope();
    SUMOReal gr = myTrainParams.weight * G * sin(DEG2RAD(slope)); //kN

    SUMOReal totalRes = res + gr; //kN

    SUMOReal trac = getInterpolatedValueFromLookUpMap(speed, &(myTrainParams.traction)); // kN

    SUMOReal a;
    if (speed < targetSpeed) {
        a = (trac - totalRes) / myTrainParams.rotWeight; //kN/t == N/kg
    } else {
        a = 0.;
        if (totalRes > trac) {
            a = (trac - totalRes) / myTrainParams.rotWeight;//kN/t == N/kg
        }
    }

    SUMOReal maxNextSpeed = speed + a * DELTA_T / 1000.;

    std::cout << veh->getID() << " speed: " << (speed*3.6) << std::endl;

    return maxNextSpeed;
}

SUMOReal MSCFModel_Rail::minNextSpeed(SUMOReal speed, const MSVehicle *const veh) const {

//    MSCFModel_Rail::VehicleVariables *vars = (MSCFModel_Rail::VehicleVariables *) veh->getCarFollowVariables();
//    if (vars->isNotYetInitialized()) {
//        initVehicleVariables(veh, vars);
//    }


    SUMOReal slope = veh->getSlope();
    SUMOReal gr = myTrainParams.weight * G * sin(DEG2RAD(slope)); //kN
    SUMOReal res = getInterpolatedValueFromLookUpMap(speed, &(myTrainParams.resistance)); // kN
    SUMOReal totalRes = res + gr; //kN

    SUMOReal a = (myTrainParams.decl + totalRes)/myTrainParams.rotWeight;

    return speed - a * DELTA_T / 1000.;

}

SUMOReal MSCFModel_Rail::getInterpolatedValueFromLookUpMap(SUMOReal speed, const LookUpMap * lookUpMap) const {
    std::map<SUMOReal, SUMOReal>::const_iterator low, prev;
    low = lookUpMap->lower_bound(speed);

    if (low == lookUpMap->end()) { //speed > max speed
        return (lookUpMap->rbegin())->second;
    }

    if (low == lookUpMap->begin()) {
        return low->second;
    }

    prev = low;
    --prev;

    SUMOReal range = low->first - prev->first;
    SUMOReal dist = speed - prev->first;
    assert(range > 0);
    assert(dist > 0);

    SUMOReal weight = dist / range;

    SUMOReal res = (1 - weight) * prev->second + weight * low->second;

    return res;

}



//void
//MSCFModel_Rail::initVehicleVariables(const MSVehicle *const veh, MSCFModel_Rail::VehicleVariables *pVariables) const {
//
//    pVariables->setInitialized();
//
//}

SUMOReal MSCFModel_Rail::getSpeedAfterMaxDecel(SUMOReal speed) const {

//    //TODO neither vehicle nor train is known here, so spd aftr mx decl cannot be calculated! [Gregor Feb '17]
//    SUMOReal gr = 0; //trainParams.weight * 9.81 * edge.grade
//
//    SUMOReal a = 0;//trainParams.decl - gr/trainParams.rotWeight;
//
//    return speed + a * DELTA_T / 1000.;
    WRITE_ERROR("function call not allowd for rail model. Exiting!");
    throw ProcessError();
}


MSCFModel::VehicleVariables *MSCFModel_Rail::createVehicleVariables() const {
    VehicleVariables *ret = new VehicleVariables();
    return ret;
}


//mostly c 'n p from MSCFModel
SUMOReal MSCFModel_Rail::moveHelper(MSVehicle *const veh, SUMOReal vPos) const {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    SUMOReal vMin, vNext;
    const SUMOReal vMax = MIN3(veh->getMaxSpeedOnLane(), maxNextSpeed(oldV, veh), vSafe);
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // we cannot rely on never braking harder than maxDecel because TraCI or strange cf models may decide to do so
        vMin = MIN2(minNextSpeed(oldV, veh), vMax);
        vNext = veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);
    } else {
        // for ballistic update, negative vnext must be allowed to
        // indicate a stop within the coming timestep (i.e., to attain negative values)
        vMin =  MIN2(minNextSpeed(oldV, veh), vMax);
        vNext = veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);
        // (Leo) moveHelper() is responsible for assuring that the next
        // velocity is chosen in accordance with maximal decelerations.
        // At this point vNext may also be negative indicating a stop within next step.
        // Moreover, because maximumSafeStopSpeed() does not consider deceleration bounds
        // vNext can be a large negative value at this point. We cap vNext here.
        vNext = MAX2(vNext, vMin);
    }

    return vNext;
}

double MSCFModel_Rail::freeSpeed(const MSVehicle *const veh, SUMOReal speed, SUMOReal dist, SUMOReal targetSpeed,
                                 const bool onInsertion) const {

//    MSCFModel_Rail::VehicleVariables *vars = (MSCFModel_Rail::VehicleVariables *) veh->getCarFollowVariables();
//    if (vars->isNotYetInitialized()) {
//        initVehicleVariables(veh, vars);
//    }

    //TODO: signals, coasting, ...

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // adapt speed to succeeding lane, no reaction time is involved
        // when breaking for y steps the following distance g is covered
        // (drive with v in the final step)
        // g = (y^2 + y) * 0.5 * b + y * v
        // y = ((((sqrt((b + 2.0*v)*(b + 2.0*v) + 8.0*b*g)) - b)*0.5 - v)/b)
        const SUMOReal v = SPEED2DIST(targetSpeed);
        if (dist < v) {
            return targetSpeed;
        }
        const SUMOReal b = ACCEL2DIST(myDecel);
        const SUMOReal y = MAX2(0.0, ((sqrt((b + 2.0 * v) * (b + 2.0 * v) + 8.0 * b * dist) - b) * 0.5 - v) / b);
        const SUMOReal yFull = floor(y);
        const SUMOReal exactGap = (yFull * yFull + yFull) * 0.5 * b + yFull * v + (y > yFull ? v : 0.0);
        const SUMOReal fullSpeedGain = (yFull + (onInsertion ? 1. : 0.)) * ACCEL2SPEED(myTrainParams.decl);
        return DIST2SPEED(MAX2((SUMOReal)0.0, dist - exactGap) / (yFull + 1)) + fullSpeedGain + targetSpeed;
    } else {
        WRITE_ERROR("Anything else then semi implicit euler update is not yet implemented. Exiting!");
        throw ProcessError();
    }
}

double MSCFModel_Rail::stopSpeed(const MSVehicle *const veh, const SUMOReal speed, SUMOReal gap) const {
    return 0;
}
