/* -*- mode: C++; c-file-style: "bsd"; tab-width: 4 -*- */
// memowrite.c - Pilot code for MemoWrite utility trace functions.
// JStroke 1.x - Japanese Kanji handwriting recognition technology demo.
// Copyright (C) 1997  Robert E. Wells
// http://wellscs.com/pilot
// mailto:robert@wellscs.com
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program (gpl.html); if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// See readme.txt, changelo, and gpl.html for more information.
//
// Commentary:
//
// This provides a mechanism for generating traces from a program as Pilot
// Memo entries, where they can be examined, HotSync'ed, and deleted using
// standard applications.
// ---------------------------------------------------------------------------

#pragma pack(2)
#include <Common.h>
#include <System/SysAll.h>
#include <UI/UIAll.h>
#include "memowrite.h"

#ifdef FOR_MEMOWRITE
// ----- Static data and defines ---------------------------------------------

#define diAllocSize 1024		// Must be a power of 2, due to code below...
#define diMaxSize   (0xfff0)

static DmOpenRef   s_pMemoDb = 0;
static UInt        s_iRecNum = 0;
static VoidHand    s_RecHandle = 0;
static UInt        s_iOffset = 0;
static UInt        s_iAllocSize = 0;

// ----- Memo_WriteOpen ------------------------------------------------------

void Memo_WriteOpen(void) {
	if (s_pMemoDb)
		return;
	s_pMemoDb = DmOpenDatabaseByTypeCreator('DATA', sysFileCMemo,
											dmModeReadWrite);
	if(!s_pMemoDb)
		return;
	s_iRecNum = DmNumRecords(s_pMemoDb);
	s_RecHandle = DmNewRecord(s_pMemoDb, &s_iRecNum, diAllocSize);
	s_iOffset = 0;
	s_iAllocSize = diAllocSize;
}

// ----- Memo_WriteClose -----------------------------------------------------

void Memo_WriteClose(void) {
	MemoWriteLen("", 0); // Make sure it is null terminated.

	// Release handle on record, resize to amount used...
	if (s_RecHandle) {
		(void) DmReleaseRecord(s_pMemoDb, s_iRecNum, true);
		if (s_iOffset+1 < s_iAllocSize &&
			(s_RecHandle = DmResizeRecord(s_pMemoDb, s_iRecNum, s_iOffset+1)))
			(void) DmReleaseRecord(s_pMemoDb, s_iRecNum, true);
		s_RecHandle = NULL;
	}

	if (s_pMemoDb)
		(void) DmCloseDatabase(s_pMemoDb);
	s_pMemoDb = NULL;
}

// ----- Memo_WriteLen -------------------------------------------------------

void Memo_WriteLen(char* cp, UInt len) {
	Long newsize, newalloc;
	Ptr  RecPointer;

	if (!s_pMemoDb || !cp)
		return;
	newsize = s_iOffset + len + 1;

	if (newsize > s_iAllocSize) {
		// If we need more room, round up to next multiple of diAllocSize.
		// This idiom only works if diAllocSize is a power of 2.
		newalloc = (newsize + diAllocSize-1) & ~(diAllocSize-1);
		if (newalloc > diMaxSize) {	// If we are nearly maxed out,
			newalloc = diMaxSize; // use what we've got.
			if (newsize > diMaxSize) // If we are maxed out, give it up.
				return;
		}
		if (s_RecHandle)
			(void) DmReleaseRecord(s_pMemoDb, s_iRecNum, true);
		s_RecHandle = DmResizeRecord(s_pMemoDb, s_iRecNum, newalloc);
		if (s_RecHandle)
			s_iAllocSize = newalloc;
	}
	if (s_RecHandle) {
		if ((RecPointer = MemHandleLock(s_RecHandle))) {
			// Write extra char at end, often it will be null termination...
			if (!DmWrite(RecPointer, s_iOffset, cp, len+1))	// if no error...
				s_iOffset += len;
			(void) MemPtrUnlock(RecPointer);
		}
	}
}

// ---------------------------------------------------------------------------
// ----- Memo_Write ----------------------------------------------------------

void Memo_Write(char* cp) {
	MemoWriteLen(cp, StrLen(cp));
}

// ----- Memo_Write2 ---------------------------------------------------------

void Memo_Write2(char* cp, char* cpVal) {
	MemoWriteLen(cp, StrLen(cp));
	MemoWriteLen(cpVal, StrLen(cpVal));
}

// ----- Memo_Writeln --------------------------------------------------------

void Memo_Writeln(char* cp) {
	MemoWriteLen(cp, StrLen(cp));
	MemoWriteLen("\n", 1);
}

// ----- Memo_Write2d --------------------------------------------------------

void Memo_Write2d(char* cp, long iVal) {
	char buf[10];
	MemoWriteLen(cp, StrLen(cp));

	if (iVal < 0) {
		MemoWriteLen("-", 1);
		iVal = -iVal;
	}
	StrIToA(buf, iVal);
	MemoWriteLen(buf, StrLen(buf));
}

// ----- Memo_Write2h --------------------------------------------------------

void Memo_Write2h(char* cp, long iVal) {
	char buf[10];
	StrIToH(buf, iVal);
	MemoWriteLen(cp, StrLen(cp));
	MemoWriteLen(buf, StrLen(buf));
}

#endif /* FOR_MEMOWRITE */
// ----- end of memowrite.c --------------------------------------------------
