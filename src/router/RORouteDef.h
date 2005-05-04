#ifndef RORouteDef_h
#define RORouteDef_h
//---------------------------------------------------------------------------//
//                        RORouteDef.h -
//  Basic class for route definitions (not the computed routes)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.11  2005/05/04 08:51:41  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.10  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.9  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.8  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.7  2003/11/11 08:04:47  dkrajzew
// avoiding emissions of vehicles on too short edges
//
// Revision 1.6  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.5  2003/06/18 11:36:50  dkrajzew
// a new interface which allows to choose whether to stop after a route could
//  not be computed or not; not very sphisticated, in fact
//
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:22:35  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <utils/common/Named.h>
#include <utils/gfx/RGBColor.h>
#include "ReferencedItem.h"
#include "ROAbstractRouter.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;
class ROAbstractRouter;
class RORoute;
class OptionsCont;
class ROVehicle;
class ROEdgeVector;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORouteDef
 * A RORouteDef is the upper class for all route definitions.
 * Whether it is just the origin and the destination, the whole route through
 * the network or even a route with alternatives depends on the derived class.
 */
class RORouteDef : public ReferencedItem,
                   public Named {
public:
    /// Constructor
    RORouteDef(const std::string &id,
        const RGBColor &color);

    /// Destructor
    virtual ~RORouteDef();

    /// Returns the route's origin edge
    virtual ROEdge *getFrom() const = 0;

    /// Returns the route's destination edge
    virtual ROEdge *getTo() const = 0;

    /** @brief Changes the id to a next, hopefully valid
        This is done if the vehicle(s) using this route are emitted periodically */
    void patchID();

    /** @brief Builds the complete route
        (or chooses her from the list of alternatives, when existing) */
    virtual RORoute *buildCurrentRoute(ROAbstractRouter &router, SUMOTime begin,
        bool continueOnUnbuild, ROVehicle &veh,
		ROAbstractRouter::ROAbstractEdgeEffortRetriever * const retriever) = 0;

    /** @brief Adds an alternative to the list of routes
        (This may be the new route) */
    virtual void addAlternative(RORoute *current, SUMOTime begin) = 0;

    /** @brief Writes the current route */
    virtual void xmlOutCurrent(std::ostream &res,
        bool isPeriodical) const = 0;

    /** @brief Writes the list of known alternatives */
    virtual void xmlOutAlternatives(std::ostream &altres) const = 0;

    /** @brief Returns a copy of the route definition */
    virtual RORouteDef *copy(const std::string &id) const = 0;

    virtual const ROEdgeVector &getCurrentEdgeVector() const = 0;

protected:
    /// The color the route shall have
    RGBColor myColor;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

