/***************************************************************************
                          OptionsIO.cpp
			  Loads the configuration file using "OptionsLoader"
			  and parses the given command line arguments using
			  "OptionsParser"
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.6  2002/06/11 13:43:35  dkrajzew
// Windows eol removed
//
// Revision 1.5  2002/06/10 08:33:23  dkrajzew
// Parsing of strings into other data formats generelized; Options now recognize false numeric values; documentation added
//
// Revision 1.7  2002/06/10 06:54:30  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized; options now recognize false numerical input
//
// Revision 1.6  2002/05/14 04:45:49  dkrajzew
// Bresenham added; some minor changes; windows eol removed
//
// Revision 1.5  2002/04/29 05:38:18  dkrajzew
// Better error handling on missing configuration implemented
//
// Revision 1.4  2002/04/26 10:08:39  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/04/17 11:21:52  dkrajzew
// Windows-carriage returns removed
//
// Revision 1.2  2002/04/16 12:28:26  dkrajzew
// Usage of SUMO_DATA removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:01  dkrajzew
// new version-free project name
//
// Revision 1.5  2002/04/09 12:20:37  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.4  2002/03/22 10:59:37  dkrajzew
// Memory leak tracing added; ostrstreams replaces by ostringstreams
//
// Revision 1.3  2002/03/20 08:50:37  dkrajzew
// Revisions patched
//
// Revision 1.2  2002/03/20 08:41:22  dkrajzew
// New configuration search schema
//
// Revision 1.1  2002/02/13 15:48:19  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include <parsers/SAXParser.hpp>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <util/PlatformUtils.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include <cstdlib>
#include "OptionsIO.h"
#include "OptionsCont.h"
#include "OptionsLoader.h"
#include "OptionsParser.h"
#include "FileHelpers.h"
#include "TplConvert.h"

#ifdef EXTERNAL_TEMPLATE_DEFINITION
#include "../utils/TplConvert.cpp"
#endif

/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
bool OptionsIO::getOptions(OptionsCont *oc, int argc, char **argv) {
    bool ret = true;
    ret = OptionsParser::parse(oc, argc, argv);
    if(ret) oc->resetDefaults();
    if(ret)
        ret = loadConfiguration(oc);
    if(ret) oc->resetDefaults();
    if(ret) ret = OptionsParser::parse(oc, argc, argv);
    return ret;
}

string OptionsIO::getConfigurationPath(OptionsCont *oc, bool &ok) {
    string path = oc->getString("c");
    // check the user supplied path
    if(FileHelpers::exists(path))
        return path;
    cout << "The configurations file '" << path << "' could not be found." << endl;
    ok = false;
    return "";
}

bool OptionsIO::loadConfiguration(OptionsCont *oc) {
    if(oc->exists("no-config") && oc->isSet("no-config") && oc->getBool("no-config"))
        return true;
    if(!oc->exists("c") || !oc->isSet("c"))
        return true;
    bool ok = true;
    string path = getConfigurationPath(oc, ok);
    if(path.length()==0||!ok)
        return false;
    try {
      XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
      cerr << "Error during XML-initialization: " << TplConvert<XMLCh>::_2str(toCatch.getMessage()) << endl;
      return false;
    }
    // build parser
    SAXParser parser;
    if(oc->getBool("v"))
      cout << "Loading configuration..." << endl;
    parser.setValidationScheme(SAXParser::Val_Auto);
    parser.setDoNamespaces(false);
    parser.setDoSchema(false);
    // start the parsing
    OptionsLoader *handler = new OptionsLoader(oc, path.c_str(), oc->getBool("w"), oc->getBool("v"));
    try {
      parser.setDocumentHandler(handler);
      parser.setErrorHandler(handler);
      parser.parse(path.c_str());
      if(handler->errorOccured())
        ok = false;
    } catch (const XMLException& toCatch) {
      cerr << "Error: " << TplConvert<XMLCh>::_2str(toCatch.getMessage()) << endl;
      ok = false;
    }
    //XMLPlatformUtils::Terminate();
    delete handler;
    return ok;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "OptionsIO.icc"
//#endif

// Local Variables:
// mode:C++
// End:



