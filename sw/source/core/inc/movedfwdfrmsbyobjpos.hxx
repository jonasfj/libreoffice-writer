/*************************************************************************
 *
 *  $RCSfile: movedfwdfrmsbyobjpos.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 15:42:17 $
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
#ifndef _MOVEDFWDFRMSBYOBJPOS_HXX
#define _MOVEDFWDFRMSBYOBJPOS_HXX

#include <vector>

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

class SwTxtNode;
class SwTxtFrm;
// --> OD 2004-10-05 #i26945#
class SwRowFrm;
// <--

class SwMovedFwdFrmsByObjPos
{
    private:
        struct Entry
        {
            const SwTxtNode* mpTxtNode;
            sal_uInt32 mnToPageNum;
        };

        std::vector< Entry* > maMovedFwdFrms;

    public:
        SwMovedFwdFrmsByObjPos();
        ~SwMovedFwdFrmsByObjPos();

        void Insert( const SwTxtFrm& _rMovedFwdFrmByObjPos,
                     const sal_uInt32 _nToPageNum );

        bool FrmMovedFwdByObjPos( const SwTxtFrm& _rTxtFrm,
                                  sal_uInt32& _ornToPageNum ) const;

        // --> OD 2004-10-05 #i26945#
        bool DoesRowContainMovedFwdFrm( const SwRowFrm& _rRowFrm ) const;
        // <--
        void Clear();
};

#endif
