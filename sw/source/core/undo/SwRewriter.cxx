/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SwRewriter.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 05:15:35 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#include <algorithm>
#include <SwRewriter.hxx>

using namespace std;

bool operator == (const SwRewriteRule & a, const SwRewriteRule & b)
{
    return a.first == b.first;
}

SwRewriter::SwRewriter()
{
}

SwRewriter::SwRewriter(const SwRewriter & rSrc)
    : mRules(rSrc.mRules)
{
}

SwRewriter::~SwRewriter()
{
}

void SwRewriter::AddRule(const String & rWhat, const String & rWith)
{
    SwRewriteRule aRule(rWhat, rWith);

    vector<SwRewriteRule>::iterator aIt;

    aIt = find(mRules.begin(), mRules.end(), aRule);

    if (aIt != mRules.end())
        *aIt = aRule;
    else
        mRules.push_back(aRule);
}

String SwRewriter::Apply(const String & rStr) const
{
    String aResult = rStr;
    vector<SwRewriteRule>::const_iterator aIt;

    for (aIt = mRules.begin(); aIt != mRules.end(); aIt++)
        aResult.SearchAndReplaceAll(aIt->first, aIt->second);

    return aResult;
}

vector<String> SwRewriter::Apply(const vector<String> & rStrs) const
{
    vector<String> aResult;
    vector<String>::const_iterator aIt;

    for (aIt = rStrs.begin(); aIt != rStrs.end(); aIt++)
        aResult.push_back(Apply(*aIt));
        
    return aResult;
}

String SwRewriter::ToString() const
{
    String aResult("[ \n", RTL_TEXTENCODING_ASCII_US);

    vector<SwRewriteRule>::const_iterator aIt;
    
    for (aIt = mRules.begin(); aIt != mRules.end(); aIt++)
    {
        aResult += String("  \"", RTL_TEXTENCODING_ASCII_US);
        aResult += aIt->first;
        aResult += String("\" -> \"", RTL_TEXTENCODING_ASCII_US);
        aResult += aIt->second;
        aResult += String("\"\n", RTL_TEXTENCODING_ASCII_US);
    }

    aResult += String("]", RTL_TEXTENCODING_ASCII_US);

    return aResult;
}

