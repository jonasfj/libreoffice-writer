/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ww8par2.hxx,v $
 * $Revision: 1.47.214.1 $
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

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <tools/solar.h>
#include <rtl/ustring.hxx>
#include <tools/stream.hxx>
#include <IDocumentExternalData.hxx>

namespace ww8
{
    typedef boost::shared_array<BYTE> DataArray_t;
    
class WW8Struct : public ::sw::ExternalData
    {
        DataArray_t mp_data;
        sal_uInt32 mn_offset;
        sal_uInt32 mn_size;
        
    public:
        WW8Struct(SvStream& rSt, sal_uInt32 nPos, sal_uInt32 nSize);
        WW8Struct(WW8Struct * pStruct, sal_uInt32 nPos, sal_uInt32 nSize);
        virtual ~WW8Struct();
        
        sal_uInt8 getU8(sal_uInt32 nOffset);

        sal_uInt16 getU16(sal_uInt32 nOffset) 
        { return getU8(nOffset) + (getU8(nOffset + 1) << 8); }

        sal_uInt32 getU32(sal_uInt32 nOffset)
        { return  getU16(nOffset) + (getU16(nOffset + 1) << 16); }
        
        ::rtl::OUString getUString(sal_uInt32 nOffset, sal_uInt32 nCount);
        
        ::rtl::OUString getString(sal_uInt32 nOffset, sal_uInt32 nCount);
    };
    
typedef ::std::vector<rtl::OUString> StringVector_t;
    template <class T>
    class WW8Sttb : public WW8Struct
    {
        typedef ::boost::shared_ptr< void > ExtraPointer_t;
        typedef ::std::vector< ExtraPointer_t > ExtrasVector_t;
        bool bDoubleByteCharacters;
        StringVector_t m_Strings;
        ExtrasVector_t m_Extras;
        
    public:
        WW8Sttb(SvStream& rSt, INT32 nPos, sal_uInt32 nSize);
        virtual ~WW8Sttb();
        
        sal_uInt32 getCount() const;
        ::rtl::OUString getEntry(sal_uInt32 nEntry) const
        {
            return m_Strings[nEntry];
        }

        StringVector_t & getStrings()
        {
            return m_Strings;
        }
        
        const T * getExtra(sal_uInt32 nEntry) const
        {
            return dynamic_cast<const T *> (m_Extras[nEntry].get());
        }
    };
    
    template <class T>
    WW8Sttb<T>::WW8Sttb(SvStream& rSt, INT32 nPos, sal_uInt32 nSize)
    : WW8Struct(rSt, nPos, nSize), bDoubleByteCharacters(false)
    {
        sal_uInt32 nOffset = 0;
        
        if (getU16(nOffset) == 0xffff)
        {
            bDoubleByteCharacters = true;
            nOffset += 2;
        }
        
        sal_uInt16 nCount = getU16(nOffset);
        sal_uInt16 ncbExtra = getU16(nOffset + 2);
        
        nOffset += 4;
        for (sal_uInt16 i = 0; i < nCount; i++)
        {
            if (bDoubleByteCharacters)
            {
                sal_uInt16 nStrLen = getU16(nOffset);
                
                m_Strings.push_back(getUString(nOffset +2, nStrLen));
                
                nOffset += 2 + 2 * nStrLen;
            }
            else
            {
                sal_uInt8 nStrLen = getU8(nOffset);
                
                m_Strings.push_back(getUString(nOffset, nStrLen));
                
                nOffset += 1 + nStrLen;
            }
            
            if (ncbExtra > 0)
            {
                ExtraPointer_t pExtra(new T(this, nOffset, ncbExtra));
                m_Extras.push_back(pExtra);

                nOffset += ncbExtra;
            }
        }
    }
    
    template <class T>
    WW8Sttb<T>::~WW8Sttb()
    {
    }
}
