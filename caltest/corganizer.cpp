/*
 * This file is part of calendar-backend
 *
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ilias Biris <ilias.biris@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <cppunit/config/SourcePrefix.h>

#include <iostream>
#include <string>

#include "test_COrganizer.h"

using namespace std;

int main(int ac,char **av)
{
   // Create the event manager and test controller
   CPPUNIT_NS::TestResult controller;

   // Add a listener that colllects test result
   CPPUNIT_NS::TestResultCollector result;
   controller.addListener( &result );

   // Add a listener that print dots as test run.
   CPPUNIT_NS::BriefTestProgressListener progress;
   controller.addListener( &progress );

  
   CppUnit::TextUi::TestRunner runner;
   // Add the top runner.to the test runner
   runner.addTest( COrganizer_TS1::suite() );
   runner.run( controller );
 
   // Print test in a compiler compatible format.
   std::ofstream file("corganizer.xml");
   CPPUNIT_NS::XmlOutputter xml( &result, file );
   xml.setStyleSheet( "report.xsl" );
   xml.write();
   file.close();
   return result.wasSuccessful() ? 0 : 1;
                               
}

