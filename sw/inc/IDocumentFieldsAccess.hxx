/*************************************************************************
 *
 *  $RCSfile: IDocumentFieldsAccess.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2006-09-27 10:51:22 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef IDOCUMENTFIELDSACCESS_HXX_INCLUDED
#define IDOCUMENTFIELDSACCESS_HXX_INCLUDED

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

class SwFldTypes;
class SwFieldType;
class SfxPoolItem;
struct SwPosition;
class SwDocUpdtFld;
class SwCalc;
class SwTxtFld;
class SwField;
class SwMsgPoolItem;
class DateTime;
class _SetGetExpFld;
struct SwHash;
class String;
class SwNode;

namespace com { namespace sun { namespace star { namespace uno { class Any; } } } }

 /** Document fields related interfaces
 */
 class IDocumentFieldsAccess
 {
 public:
    /**
    */
    virtual const SwFldTypes *GetFldTypes() const = 0;

    /**
    */
    virtual SwFieldType *InsertFldType(const SwFieldType &) = 0;

    /**
    */
    virtual SwFieldType *GetSysFldType( const sal_uInt16 eWhich ) const = 0;

    /**
    */
    virtual SwFieldType* GetFldType(sal_uInt16 nResId, const String& rName, bool bDbFieldMatching) const = 0;

    /**
    */
    virtual void RemoveFldType(sal_uInt16 nFld) = 0;

    /**
    */
    virtual void UpdateFlds( SfxPoolItem* pNewHt, bool bCloseDB) = 0;

    /**
    */
    virtual void InsDeletedFldType(SwFieldType &) = 0;

    // #111840#
    /**
       Puts a value into a field at a certain position.

       A missing field at the given position leads to a failure.

       @param rPosition        position of the field
       @param rVal             the value
       @param nMId

       @retval TRUE            putting of value was successful
       @retval FALSE           else
    */
    virtual bool PutValueToField(const SwPosition & rPos, const com::sun::star::uno::Any& rVal, BYTE nMId) = 0;

    // rufe das Update der Expression Felder auf; alle Ausdruecke werden
    // neu berechnet.
    // #111840#

    /** Updates a field.

        @param rDstFmtFld field to update
        @param rSrcFld field containing the new values
        @param pMsgHnt
        @param bUpdateTblFlds TRUE: update table fields, too.

        @retval TRUE             update was successful
        @retval FALSE            else
    */
    virtual bool UpdateFld(SwTxtFld * rDstFmtFld, SwField & rSrcFld, SwMsgPoolItem * pMsgHnt, bool bUpdateTblFlds) = 0;

    /**
    */
    virtual void UpdateRefFlds(SfxPoolItem* pHt) = 0;

    /**
    */
    virtual void UpdateTblFlds(SfxPoolItem* pHt) = 0;

    /**
    */
    virtual void UpdateExpFlds(SwTxtFld* pFld, bool bUpdateRefFlds) = 0;

    /**
    */
    virtual void UpdateUsrFlds() = 0;

    /**
    */
    virtual void UpdatePageFlds(SfxPoolItem*) = 0;

    /**
    */
    virtual void LockExpFlds() = 0;

    /**
    */
    virtual void UnlockExpFlds() = 0;

    /**
    */
    virtual bool IsExpFldsLocked() const = 0;


    virtual SwDocUpdtFld& GetUpdtFlds() const = 0;

    /*	@@@MAINTAINABILITY-HORROR@@@
        SwNode (see parameter pChk) is (?) part of the private
        data structure of SwDoc and should not be exposed
    */
    virtual bool SetFieldsDirty(bool b, const SwNode* pChk, ULONG nLen) = 0;

    /**
    */
    virtual void SetFixFields(bool bOnlyTimeDate, const DateTime* pNewDateTime) = 0;

    // Setze im Calculator alle SetExpresion Felder, die bis zur
    // angegebenen Position (Node [ + ::com::sun::star::ucb::Content]) gueltig sind. Es kann
    // eine erzeugte Liste aller Felder mit uebergegeben werden.
    // (ist die Adresse != 0, und der Pointer == 0 wird eine neue
    // Liste returnt.)
    virtual void FldsToCalc(SwCalc& rCalc, ULONG nLastNd, sal_uInt16 nLastCnt) = 0;

    /**
    */
    virtual void FldsToCalc(SwCalc& rCalc, const _SetGetExpFld& rToThisFld) = 0;

    /**
    */
    virtual void FldsToExpand(SwHash**& ppTbl, sal_uInt16& rTblSize, const _SetGetExpFld& rToThisFld) = 0;

    /**
    */
    virtual bool IsNewFldLst() const = 0;

    /**
    */
    virtual void SetNewFldLst( bool bFlag) = 0;

    /**
    */
    virtual void InsDelFldInFldLst(bool bIns, const SwTxtFld& rFld) = 0;

protected:
    virtual ~IDocumentFieldsAccess() {};
 };

 #endif // IDOCUMENTLINKSADMINISTRATION_HXX_INCLUDED
