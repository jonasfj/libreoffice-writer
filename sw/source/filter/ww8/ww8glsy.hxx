/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ww8glsy.hxx,v $
 * $Revision: 1.15 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */

#ifndef _WW8GLSY_HXX
#define _WW8GLSY_HXX

#include <sot/storage.hxx>
#include "ww8scan.hxx"

class SwTextBlocks;
class SwNodeIndex;

/*
 * GlossaryFib takes the document fib and finds the glossary fib which may
 * not exist. The glossary fib has the offsets into the autotext subdocument
 * which is at the end of template .dot's
 */
class WW8GlossaryFib : public WW8Fib
{
public:
    WW8GlossaryFib( SvStream& rStrm, BYTE nWantedVersion ,
        SvStream& rTableStrm, const WW8Fib &rFib) : WW8Fib(rStrm,
        nWantedVersion,FindGlossaryFibOffset(rTableStrm,rStrm,rFib)) {}
    bool IsGlossaryFib();
private:
    UINT32 FindGlossaryFibOffset(SvStream &rTableStrm,SvStream &rStrm,
        const WW8Fib &rFib);
};

/*
 * Imports glossaries from word, given the document it gets the usual word
 * doc information, then the glossary fib and uses the usual reader class to
 * wrap the autotext into a star doc. Afterwards taking each section entry and
 * making it a single star autotext entry.
 *
 * ToDo currently all autotext entries become resource hungry star autotext
 * formatted text, need to use a flag in the ww8reader class to determine if
 * an entry is formatted or not.
 */
class WW8Glossary
{
public:
    WW8Glossary( SvStorageStreamRef &refStrm, BYTE nVersion, SvStorage *pStg=0);
    bool Load( SwTextBlocks &rBlocks, bool bSaveRelFile );
    ~WW8Glossary()                  { delete pGlossary; }
    WW8GlossaryFib *GetFib()        { return pGlossary; }
    USHORT GetNoStrings() const     { return nStrings; }

private:
    WW8GlossaryFib *pGlossary;
    SvStorageStreamRef xTableStream;
    SvStorageStreamRef &rStrm;
    SvStorageRef xStg;
    USHORT nStrings;

    bool MakeEntries(SwDoc *pD, SwTextBlocks &rBlocks, bool bSaveRelFile,
        const ::std::vector<String>& rStrings,
        const ::std::vector<ww::bytes>& rExtra);
    bool HasBareGraphicEnd(SwDoc *pD,SwNodeIndex &rIdx);

    //No copying
    WW8Glossary(const WW8Glossary&);
    WW8Glossary& operator=(const WW8Glossary&);
};
#endif

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
