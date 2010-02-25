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
#ifndef _DRAWDOC_HXX
#define _DRAWDOC_HXX

#ifndef _FM_FMMODEL_HXX
#include <svx/fmmodel.hxx>
#endif

class SwDoc;
class SwDocShell;

//==================================================================

class SwDrawDocument : public FmFormModel
{
    SwDoc* pDoc;
public:
    SwDrawDocument( SwDoc* pDoc );
    ~SwDrawDocument();

    const SwDoc& GetDoc() const	{ return *pDoc; }
          SwDoc& GetDoc()      	{ return *pDoc; }

    virtual SdrPage* AllocPage(FASTBOOL bMasterPage);

    // fuers "load on demand" von Grafiken im DrawingLayer
    virtual SvStream* GetDocumentStream( SdrDocumentStreamInfo& rInfo ) const;

    // fuers Speicher von Rechtecken als Control-Ersatz fuker Versionen < 5.0
    virtual SdrLayerID GetControlExportLayerId( const SdrObject & ) const;

protected:
    // --> OD 2006-03-01 #b6382898#
    // overload of <SdrModel::createUnoModel()> is needed to provide corresponding uno model.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > createUnoModel();
    // <--
};


#endif
