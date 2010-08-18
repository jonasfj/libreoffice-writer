/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef _WW8GRAF_HXX
#define _WW8GRAF_HXX

#include <vector>
#include <stack>
#include "writerhelper.hxx"

struct EscherShape
{
    ULONG mnEscherShapeOrder;
    ULONG mnNoInlines;
    // --> OD 2004-12-13 #117915# - new member <mbInHeaderFooter>
    bool mbInHeaderFooter;
    EscherShape( ULONG nEscherShapeOrder,
                 bool _bInHeaderFooter )
        : mnEscherShapeOrder(nEscherShapeOrder),
          mnNoInlines(0),
          mbInHeaderFooter( _bInHeaderFooter )
    {}
    // <--
};

class wwZOrderer
{
private:
    // --> OD 2004-12-13 #117915# - consider that objects in page header/footer
    // are always behind objects in page body. Thus, assure, that in vector
    // <maEscherLayer> objects in page header|footer are inserted before
    // objects in page body - see method <GetEscherObjectPos(..)>.
    //No of objects in doc before starting (always 0 unless using file->insert
    //and probably 0 then as well
    std::vector<EscherShape> maEscherLayer;
    // <--
    typedef std::vector<EscherShape>::iterator myeiter;

    std::vector<short> maDrawHeight;
    typedef std::vector<short>::iterator myditer;

    std::stack<USHORT> maIndexes;

    sw::util::SetLayer maSetLayer;

    ULONG mnNoInitialObjects;
    ULONG mnInlines;
    SdrPage* mpDrawPg;
    const SvxMSDffShapeOrders *mpShapeOrders;

    USHORT GetEscherObjectIdx(ULONG nSpId);
    myeiter MapEscherIdxToIter(ULONG nIdx);
    // --> OD 2004-12-13 #117915# - new parameter <_bInHeaderFooter>, indicating
    // that object is in header or footer
    ULONG GetEscherObjectPos( ULONG nSpId,
                              const bool _bInHeaderFooter );
    // <--
    ULONG GetDrawingObjectPos(short nWwHeight);
    bool InsertObject(SdrObject *pObject, ULONG nPos);
public:
    wwZOrderer(const sw::util::SetLayer &rSetLayer, SdrPage* pDrawPg,
        const SvxMSDffShapeOrders *pShapeOrders);
    void InsertTextLayerObject(SdrObject* pObject);
    /*
     cmc: We should have have seperate ZOrder classes for 95- and 97+ and
     instantiate the appropiate one at run time.
     */
    void InsertDrawingObject(SdrObject* pObj, short nWwHeight);
    // --> OD 2004-12-13 #117915# - new parameter <_bInHeaderFooter>, indicating
    // that object is in header or footer
    void InsertEscherObject( SdrObject* pObject,
                             ULONG nSpId,
                             const bool _bInHeaderFooter );
    // <--
    void InsideEscher(ULONG nIndex);
    void OutsideEscher();
};

void WW8FSPAShadowToReal( WW8_FSPA_SHADOW* pFSPAS, WW8_FSPA* pPic );
#endif

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
