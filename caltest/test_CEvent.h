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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <cppunit/config/SourcePrefix.h>
#include "CEvent.h"
#include "COrganizer.h"
#include <iostream>


class CEvent_TS1 : public CppUnit::TestFixture
{
	
	CPPUNIT_TEST_SUITE( CEvent_TS1 );
	CPPUNIT_TEST(test_SetGeo_Valid);
	CPPUNIT_TEST(test_SetTransparency_Valid);
	CPPUNIT_TEST(test_SetTransparency_invalid);
	CPPUNIT_TEST(test_SetPriority_Negative);
	CPPUNIT_TEST(test_SetPriority_Valid);
	CPPUNIT_TEST(test_SetPriority_Inrange);
	CPPUNIT_TEST(test_setCreatedTimeZero);
	CPPUNIT_TEST(test_setCreatedTimeValid);
	CPPUNIT_TEST(test_setDateStampZero);
	CPPUNIT_TEST(test_setDateStampValid);
	CPPUNIT_TEST(test_setOrganizer_Null);
	CPPUNIT_TEST(test_setOrganizer_Valid);
	CPPUNIT_TEST(test_setAttachments_Null);
	CPPUNIT_TEST(test_setAttachments_Valid);
	CPPUNIT_TEST(test_setAttendees_Null);
	CPPUNIT_TEST(test_setAttendees_Valid);
	CPPUNIT_TEST(test_setSequence_Valid);
	CPPUNIT_TEST(test_setUid_Valid);
	CPPUNIT_TEST(test_setUrl_Valid);
	CPPUNIT_TEST(test_setCategories_Valid);
	CPPUNIT_TEST(test_setContact_Valid);
	CPPUNIT_TEST(test_setComments_Valid);
	CPPUNIT_TEST(test_setRelated_Valid);
	CPPUNIT_TEST(test_setResources_Valid);
	CPPUNIT_TEST(test_setXProperties_Valid);
	CPPUNIT_TEST(test_validLocation);
	CPPUNIT_TEST(test_validSummary);
	CPPUNIT_TEST(test_validModifiedTime);
	CPPUNIT_TEST(test_Status);
	CPPUNIT_TEST(test_Type);
	CPPUNIT_TEST(test_Id);
	CPPUNIT_TEST(test_Description);
	CPPUNIT_TEST(test_validDateStart);
	CPPUNIT_TEST(test_validDateEnd);
	CPPUNIT_TEST(test_emptySummary);
	CPPUNIT_TEST(test_setUrl_Empty);
	CPPUNIT_TEST(test_SetGeo_Empty);
	CPPUNIT_TEST(test_setCategories_Empty);
	CPPUNIT_TEST(test_setContact_Empty);
	CPPUNIT_TEST(test_setComments_Empty);
	CPPUNIT_TEST(test_setRelated_Empty);
	CPPUNIT_TEST(test_setResources_Empty);
	CPPUNIT_TEST (test_validCEventCopyConstructor);
	CPPUNIT_TEST (test_validCEventAssignementOperator);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		CEvent * pCe;
		void setUp();
		void tearDown();
		void test_SetGeo_Valid();
		void test_SetGeo_Empty();
		void test_SetTransparency_Valid();
		void test_SetTransparency_invalid();
		void test_SetPriority_Negative();
		void test_SetPriority_Valid();
		void test_SetPriority_Inrange();
		void test_setCreatedTimeZero();
		void test_setCreatedTimeValid();
		void test_setDateStampZero();
		void test_setDateStampValid();
		void test_setOrganizer_Null();
		void test_setOrganizer_Valid();
		void test_setAttachments_Null();
		void test_setAttachments_Valid();
		void test_setAttendees_Null();
		void test_setAttendees_Valid();
		void test_setSequence_Valid();
		void test_setUid_Valid();
		void test_setUrl_Valid();
		void test_setUrl_Empty();
		void test_setCategories_Valid();
		void test_setCategories_Empty();
		void test_setContact_Valid();
		void test_setContact_Empty();
		void test_setComments_Valid();
		void test_setComments_Empty();
		void test_setRelated_Empty();
		void test_setRelated_Valid();
		void test_setResources_Valid();
		void test_setResources_Empty();
		void test_setXProperties_Valid();
		void test_validLocation();
		void test_validSummary();
		void test_emptySummary();
		void test_validModifiedTime();
		void test_Status();
		void test_Type();
		void test_Id();
		void test_Description();
		void test_emptyDescription();
		void test_validDateStart();
		void test_validDateEnd();

		void test_validCEventCopyConstructor();
		void test_validCEventAssignementOperator();

};


class CEvent_TS2 : public CppUnit::TestFixture
{
	
	CPPUNIT_TEST_SUITE( CEvent_TS2 );
	CPPUNIT_TEST(test_toString);
	CPPUNIT_TEST(test_toString_NULL);
	CPPUNIT_TEST(test_toString_flags);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		CEvent * pCe;
		void setUp();
		void tearDown();
		void test_toString();
		void test_toString_NULL();
		void test_toString_flags();
};
		
