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
#ifndef _UNDO_FLY_STR_ATTR_HXX
#define _UNDO_FLY_STR_ATTR_HXX

#include <undobj.hxx>
#include <swundo.hxx>

class SwFlyFrmFmt;
class String;

class SwUndoFlyStrAttr : public SwUndo
{
    public:
        SwUndoFlyStrAttr( SwFlyFrmFmt& rFlyFrmFmt,
                          const SwUndoId eUndoId,
                          const String& sOldStr,
                          const String& sNewStr );
        virtual ~SwUndoFlyStrAttr();

        virtual void Undo( SwUndoIter & rIt );
        virtual void Redo( SwUndoIter & rIt );
        virtual void Repeat( SwUndoIter & rIt );

        virtual SwRewriter GetRewriter() const;

    private:
        SwFlyFrmFmt& mrFlyFrmFmt;
        const String msOldStr;
        const String msNewStr;
};

#endif // _UNDO_FLY_STR_ATTR_HXX
