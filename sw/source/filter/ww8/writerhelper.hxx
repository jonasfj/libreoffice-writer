/*************************************************************************
 *
 *  $RCSfile: writerhelper.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2003-12-09 11:52:58 $
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
 *  Contributor(s): cmc@openoffice.org
 *
 *
 ************************************************************************/

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
/// @HTML

#ifndef SW_WRITERHELPER
#define SW_WRITERHELPER

#include <typeinfo>
#include <vector>

#ifndef WW_TYPES
#   include "types.hxx"
#endif

#ifndef _SFXITEMPOOL_HXX
#   include <svtools/itempool.hxx>     //SfxItemPool
#endif
#ifndef _SFXITEMSET_HXX
#   include <svtools/itemset.hxx>      //SfxItemSet
#endif
#ifndef _FORMAT_HXX
#   include <format.hxx>               //SwFmt
#endif
#ifndef _NODE_HXX
#   include <node.hxx>                 //SwCntntNode
#endif
#ifndef _DOC_HXX
#   include <doc.hxx>                  //SwDoc
#endif
#ifndef _PAM_HXX
#   include <pam.hxx>                  //SwPaM
#endif
#ifndef _IPOBJ_HXX
#   include <so3/ipobj.hxx>            //SvInPlaceObjectRef
#endif
#ifndef _TL_POLY_HXX
#   include <tools/poly.hxx>           //Polygon, PolyPolygon
#endif

//Uncomment to dump debugging streams of graphics
#define DEBUGDUMP

class SwDoc;
class SwTxtFmtColl;
class SwCharFmt;
class SdrObject;
class SdrOle2Obj;
class SvPersist;
class OutlinerParaObject;
class SdrTextObj;
class SwNumFmt;
class SwTxtNode;
class SwNoTxtNode;

namespace sw
{
    /// STL container of Paragraph Styles (SwTxtFmtColl)
    typedef std::vector<SwTxtFmtColl *> ParaStyles;
    /// STL iterator for ParaStyles
    typedef std::vector<SwTxtFmtColl *>::iterator ParaStyleIter;


    /** Make exporting a Writer Frame easy

        In word all frames are effectively anchored to character or as
        character. This is nice and simple, writer is massively complex in this
        area, so this sw::Frame simplies matters by providing a single unified
        view of the multitute of elements in writer and their differing quirks. 
        
        A sw::Frame wraps a writer frame and is guaranted to have a suitable
        anchor position available from it. It hides much of the needless
        complexity of the multitude of floating/inline elements in writer, it...

        Guarantees an anchor position for a frame.
        Provides a readable way to see if we are anchored inline. (as character)
        Provides a simple way to flag what type of entity this frame describes.
        Provides the size of the element as drawn by writer.

        @author 
        <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
    */
    class Frame
    {
    public:
        enum WriterSource {eTxtBox, eGraphic, eOle, eDrawing, eFormControl};
    private:
        const SwFrmFmt* mpFlyFrm;
        SwPosition maPos;
        Size maSize;
        WriterSource meWriterType;
        const SwNode *mpStartFrameContent;
        bool mbIsInline;
    public:
        Frame(const SwFrmFmt &rFlyFrm, const SwPosition &rPos);

        /** Get the writer SwFrmFmt that this object describes

            @return 
            The wrapped SwFrmFmt
        */
        const SwFrmFmt &GetFrmFmt() const { return *mpFlyFrm; }

        /** Get the position this frame is anchored at

            @return 
            The anchor position of this frame
        */
        const SwPosition &GetPosition() const { return maPos; }

        /** Get the node this frame is anchored into

            @return 
            The SwTxtNode this frame is anchored inside
        */
        const SwCntntNode *GetCntntNode() const
            { return maPos.nNode.GetNode().GetCntntNode(); }

        /** Get the type of frame that this wraps

            @return 
            a WriterSource which describes the source type of this wrapper
        */
        WriterSource GetWriterType() const { return meWriterType; }

        /** Is this frame inline (as character)

            @return 
            whether this is inline or not
        */
        bool IsInline() const;


        /** Even if the frame isn't an inline frame, force it to behave as one
       
            There are a variety of circumstances where word cannot have
            anything except inline elements, e.g. inside frames. So its easier
            to force this sw::Frame into behaving as one, instead of special
            casing export code all over the place.

        */
        void ForceTreatAsInline();

        /** Get the first node of content in the frame
         
         @return
         the first node of content in the frame, might not be any at all.
        */
        const SwNode *GetContent() const { return mpStartFrameContent; }


        /** Does this sw::Frame refer to the same writer content as another
         
         @return
         if the two sw::Frames are handling the same writer frame
        */
        bool RefersToSameFrameAs(const Frame &rOther) const
        {
            return (mpFlyFrm == rOther.mpFlyFrm);
        }

        /** The Size of the contained element
         
         @return
         the best size to use to export to word
        */
        const Size GetSize() const { return maSize; }
    };
    
    /// STL container of Frames
    typedef std::vector<Frame> Frames;
    /// STL iterator for Frames
    typedef std::vector<Frame>::iterator FrameIter;
}

namespace sw
{
    namespace util
    {
        /** Provide a dynamic_cast style cast for SfxPoolItems
         
            A SfxPoolItem generally need to be cast back to its original type
            to be useful, which is both tedious and errorprone. So item_cast is
            a helper template to aid the process and test if the cast is
            correct.

            @param rItem 
            The SfxPoolItem which is to be casted

            @tplparam T 
            A SfxPoolItem derived class to cast rItem to

            @return A rItem upcasted back to a T

            @exception std::bad_cast Thrown if the rItem was not a T

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & item_cast(const SfxPoolItem &rItem)
            throw(std::bad_cast)
        {
            if (!rItem.IsA(STATICTYPE(T)))
                throw std::bad_cast();
            return static_cast<const T &>(rItem);
        }

        /** Provide a dynamic_cast style cast for SfxPoolItems
         
            A SfxPoolItem generally need to be cast back to its original type
            to be useful, which is both tedious and errorprone. So item_cast is
            a helper template to aid the process and test if the cast is
            correct.

            @param pItem 
            The SfxPoolItem which is to be casted

            @tplparam T 
            A SfxPoolItem derived class to cast pItem to

            @return A pItem upcasted back to a T or 0 if pItem was not a T

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T * item_cast(const SfxPoolItem *pItem)
        {
            if (pItem && !pItem->IsA(STATICTYPE(T)))
                pItem = 0;
            return static_cast<const T *>(pItem);
        }

        /** Extract a SfxPoolItem derived property from a SwCntntNode

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            ItemGet uses item_cast () on the retrived reference to test that the
            retrived property is of the type that the developer thinks it is.

            @param rNode 
            The SwCntntNode to retrieve the property from

            @param eType 
            The numeric identifier of the property to be retrieved

            @tplparam T 
            A SfxPoolItem derived class of the retrieved property

            @exception std::bad_cast Thrown if the property was not a T

            @return The T requested

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & ItemGet(const SwCntntNode &rNode, 
            sal_uInt16 eType) throw(std::bad_cast)
        {
            return item_cast<T>(rNode.GetAttr(eType));
        }

        /** Extract a SfxPoolItem derived property from a SwFmt

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            ItemGet uses item_cast () on the retrived reference to test that the
            retrived property is of the type that the developer thinks it is.

            @param rFmt 
            The SwFmt to retrieve the property from

            @param eType 
            The numeric identifier of the property to be retrieved

            @tplparam T 
            A SfxPoolItem derived class of the retrieved property

            @exception std::bad_cast Thrown if the property was not a T

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & ItemGet(const SwFmt &rFmt, 
            sal_uInt16 eType) throw(std::bad_cast)
        {
            return item_cast<T>(rFmt.GetAttr(eType));
        }

        /** Extract a SfxPoolItem derived property from a SfxItemSet

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            ItemGet uses item_cast () on the retrived reference to test that the
            retrived property is of the type that the developer thinks it is.

            @param rSet 
            The SfxItemSet to retrieve the property from

            @param eType 
            The numeric identifier of the property to be retrieved

            @tplparam T 
            A SfxPoolItem derived class of the retrieved property

            @exception std::bad_cast Thrown if the property was not a T

            @return The T requested

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & ItemGet(const SfxItemSet &rSet, 
            sal_uInt16 eType) throw(std::bad_cast)
        {
            return item_cast<T>(rSet.Get(eType));
        }

        /** Extract a default SfxPoolItem derived property from a SfxItemPool

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            DefaultItemGet returns a reference to the default property of a
            given SfxItemPool for a given property id, e.g. default fontsize

            DefaultItemGet uses item_cast () on the retrived reference to test
            that the retrived property is of the type that the developer thinks
            it is.

            @param rPool 
            The SfxItemPool whose default property we want

            @param eType 
            The numeric identifier of the default property to be retrieved

            @tplparam T 
            A SfxPoolItem derived class of the retrieved property

            @exception std::bad_cast Thrown if the property was not a T

            @return The T requested

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & DefaultItemGet(const SfxItemPool &rPool,
            sal_uInt16 eType) throw(std::bad_cast)
        {
            return item_cast<T>(rPool.GetDefaultItem(eType));
        }

        /** Extract a default SfxPoolItem derived property from a SwDoc

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            DefaultItemGet returns a reference to the default property of a
            given SwDoc (Writer Document) for a given property id, e.g default
            fontsize

            DefaultItemGet uses item_cast () on the retrived reference to test
            that the retrived property is of the type that the developer thinks
            it is.

            @param rPool 
            The SfxItemPool whose default property we want

            @param eType 
            The numeric identifier of the default property to be retrieved

            @tplparam T 
            A SfxPoolItem derived class of the retrieved property

            @exception std::bad_cast Thrown if the property was not a T

            @return The T requested

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & DefaultItemGet(const SwDoc &rDoc, 
            sal_uInt16 eType) throw(std::bad_cast)
        {
            return DefaultItemGet<T>(rDoc.GetAttrPool(), eType);
        }

        /** Return a pointer to a SfxPoolItem derived class if it exists in an
            SfxItemSet

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            HasItem returns a pointer to the requested SfxPoolItem for a given
            property id if it exists in the SfxItemSet or its chain of parents,
            e.g. fontsize

            HasItem uses item_cast () on the retrived pointer to test that the
            retrived property is of the type that the developer thinks it is.

            @param rSet
            The SfxItemSet whose property we want

            @param eType 
            The numeric identifier of the default property to be retrieved

            @tplparam T 
            A SfxPoolItem derived class of the retrieved property

            @return The T requested or 0 if no T found with id eType

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T* HasItem(const SfxItemSet &rSet,
            sal_uInt16 eType)
        {
            return item_cast<T>(rSet.GetItem(eType));
        }

        /** Return a pointer to a SfxPoolItem derived class if it exists in an
            SwFmt

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            HasItem returns a pointer to the requested SfxPoolItem for a given
            property id if it exists in the SwFmt e.g. fontsize

            HasItem uses item_cast () on the retrived pointer to test that the
            retrived property is of the type that the developer thinks it is.

            @param rSet
            The SwFmt whose property we want

            @param eType 
            The numeric identifier of the default property to be retrieved

            @tplparam T 
            A SfxPoolItem derived class of the retrieved property

            @return The T requested or 0 if no T found with id eType

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T* HasItem(const SwFmt &rFmt,
            sal_uInt16 eType)
        {
            return HasItem<T>(rFmt.GetAttrSet(), eType);
        }

        /** Get the Paragraph Styles of a SwDoc
        
            Writer's styles are in one of those dreaded macro based pre-STL
            containers. Give me an STL container of the paragraph styles
            instead.

            @param rDoc 
            The SwDoc document to get the styles from

            @return A ParaStyles containing the SwDoc's Paragraph Styles

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        ParaStyles GetParaStyles(const SwDoc &rDoc);


        /** Get a Paragraph Style which fits a given name

            Its surprisingly tricky to get a style when all you have is a name,
            but that's what this does

            @param rDoc 
            The SwDoc document to search in

            @param rName
            The name of the style to search for

            @return A Paragraph Style if one exists which matches the name

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        SwTxtFmtColl* GetParaStyle(SwDoc &rDoc, const String& rName);

        /** Get a Character Style which fits a given name

            Its surprisingly tricky to get a style when all you have is a name,
            but that's what this does

            @param rDoc 
            The SwDoc document to search in

            @param rName
            The name of the style to search for

            @return A Character Style if one exists which matches the name

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        SwCharFmt* GetCharStyle(SwDoc &rDoc, const String& rName);

        /** Sort sequence of Paragraph Styles by outline numbering level

            Sort ParaStyles in ascending order of outline level, e.g.  given
            Normal/Heading2/Heading1 at their default outline levels of body
            level/level 2/level 1 sorts them to Heading1/Heading2/Normal

            @param rStyles 
            The ParaStyles to sort

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        void SortByOutline(ParaStyles &rStyles);

        /** Get the Floating elements in a SwDoc
        
            Writer's FrmFmts may or may not be anchored to some text content,
            e.g. Page Anchored elements will not be. For the winword export we
            need them to have something to be anchored to. So this method
            returns all the floating elements in a document as a STL container
            of sw::Frames which are guaranteed to have an appropiate anchor.
            This will include drawing objects, use GetNonDrawingFrames if
            you are not interested in the drawing objects.

            @param rDoc 
            The SwDoc document to get the styles from

            @param pPaM
            The SwPam to describe the selection in the document to get the
            elements from. 0 means the entire document.

            @return A Frames containing the selections Floating elements

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        Frames GetAllFrames(const SwDoc &rDoc, SwPaM *pPaM = 0);

        /** Get the Floating elements in a SwDoc
        
            Writer's FrmFmts may or may not be anchored to some text content,
            e.g. Page Anchored elements will not be. For the winword export we
            need them to have something to be anchored to. So this method
            returns all the floating elements in a document as a STL container
            of sw::Frames which are guaranteed to have an appropiate anchor.
            This will not include drawing objects, use GetAllFrames if you
            are interested in the drawing objects.

            @param rDoc 
            The SwDoc document to get the styles from

            @param pPaM
            The SwPam to describe the selection in the document to get the
            elements from. 0 means the entire document.

            @return A Frames containing the selections Floating elements

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        Frames GetNonDrawingFrames(const SwDoc &rDoc, SwPaM *pPaM = 0);

        /** Get the Frames anchored to a given node

            Given a container of frames, find the ones anchored to a given node

            @param rFrames
            The container of frames to search in

            @param rNode
            The SwNode to check for anchors to

            @return the Frames in rFrames anchored to rNode

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        Frames GetFramesInNode(const Frames &rFrames, const SwNode &rNode);

        /** Get the Frames anchored for all nodes between two points

            Given a container of frames, find the ones anchored to the nodes
            from start to end. Half open sequence, i.e. those anchored to
            start, but not those anchored to end

            @param rFrames
            The container of frames to search in

            @param rStart
            The SwNode to start check for anchors from

            @param rEnd
            The SwNode to end check for anchors from

            @return the Frames in rFrames anchored to rNode

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        Frames GetFramesBetweenNodes(const Frames &rFrames, 
                const SwNode &rStart, const SwNode &rEnd);

        /** Get the Numbering Format used on a paragraph

            There are two differing types of numbering formats that may be on a
            paragraph, normal and outline. The outline is that numbering you
            see in tools->outline numbering. Theres no difference in the
            numbering itself, just how you get it from the SwTxtNode. Needless
            to say the filter generally couldn't care less what type of
            numbering is in use.
          
            @param rTxtNode
            The SwTxtNode that is the paragraph

            @return A SwNumFmt pointer that describes the numbering level
            on this paragraph, or 0 if there is none.

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        const SwNumFmt* GetNumFmtFromTxtNode(const SwTxtNode &rTxtNode);

        /** Get the SwNoTxtNode associated with a SwFrmFmt if here is one

            There are two differing types of numbering formats that may be on a
            paragraph, normal and outline. The outline is that numbering you
            see in tools->outline numbering. Theres no difference in the
            numbering itself, just how you get it from the SwTxtNode. Needless
            to say the filter generally couldn't care less what type of
            numbering is in use.
          
            @param rFmt
            The SwFrmFmt that may describe a graphic

            @return A SwNoTxtNode pointer that describes the graphic of this
            frame if there is one, or 0 if there is none.

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        SwNoTxtNode *GetNoTxtNodeFromSwFrmFmt(const SwFrmFmt &rFmt);

        /** Does a node have a "page break before" applied
        
            Both text nodes and tables in writer can have "page break before"
            This function gives a unified view to both entities

            @param rNode
            The SwNode to query the page break of

            @return true if there is a page break, false otherwise

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        bool HasPageBreak(const SwNode &rNode);

        /** Shift tabstops from a given start position to a new start position

            For no good reason except to complicate my life writer tabs are
            relative to the left of the paragraph text body indent. More
            reasonably word's are absolute.
            
            AdjustTabs converts the tabs in rTabs originally relative from
            nScrLeft to be relative to nDestLeft. For example nSrcLeft would be
            0 when converting from word to writer, and vice versa when
            converting to word, and both values are set when moving writer tabs
            after an indent change

            @param rTabs
            The SvxTabStopItem whose tabs we want to change

            @param nSrcLeft
            The original offset that rTabs are relative to

            @param nDestLeft
            The new offset to change rTabs to be relative to

            @return true if there was any tabs changed, false otherwise

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>

       */
        bool AdjustTabs(SvxTabStopItem &rTabs, long nSrcLeft, long nDestLeft);


        /** Make a best fit Polygon from a PolyPolygon
        
            For custom contours in writer we use a PolyPolygon, while word uses
            a simple polygon, so we need to try and make the best polygon from
            a PolyPolygon

            @param rPolyPoly
            The PolyPolygon to try and turn into a Polygon

            @return best fit Polygon from rPolyPoly

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        Polygon PolygonFromPolyPolygon(const PolyPolygon &rPolyPoly);


        /** Make setting a drawing object's layer in a Writer document easy


            Word has the simple concept of a drawing object either in the
            foreground and in the background. We have an additional complexity
            that form components live in a seperate layer, which seems
            unnecessarily complicated. So in the winword filter we set the
            object's layer through this class with either SendObjectToHell for
            the bottom layer and SendObjectToHeaven for the top and we don't
            worry about the odd form layer design wrinkle.

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        class SetLayer
        {
        private:
            sal_uInt8 mnHeavenLayer, mnHellLayer, mnFormLayer;
            enum Layer {eHeaven, eHell};
            void SetObjectLayer(SdrObject &rObject, Layer eLayer) const;
            void Swap(SetLayer &rOther) throw();
        public:

            /** Make Object live in the bottom drawing layer

                @param rObject 
                The object to be set to the bottom layer
            */
            void SendObjectToHell(SdrObject &rObject) const;

            /** Make Object lives in the top top layer

                @param rObject 
                The object to be set to the bottom layer
            */
            void SendObjectToHeaven(SdrObject &rObject) const;

            /** Normal constructor

                @param rDoc 
                The Writer document whose drawing layers we will be inserting
                objects into
            */
            SetLayer(const SwDoc &rDoc);

            SetLayer(const SetLayer &rOther) throw();
            SetLayer& operator=(const SetLayer &rOther) throw();
        };
    }

    namespace hack
    {
        /** Make inserting an OLE object into a Writer document easy

            The rest of Office uses SdrOle2Obj for their OLE objects, Writer
            doesn't, which makes things a bit difficult as this is the type of
            object that the escher import code shared by the MSOffice filters
            produces when it imports an OLE object. 
            
            This utility class takes ownership of the OLE object away from a
            SdrOle2Obj and can massage it into the condition best suited to
            insertion into Writer.

            If the object was not transferred into Writer then it is deleted
            during destruction.

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        class DrawingOLEAdaptor
        {
        private:
            String msOrigPersistName;
            SvInPlaceObjectRef mxIPRef;
            SvPersist &mrPers;
        public:
            /** Take ownership of a SdrOle2Objs OLE object

                @param rObj 
                The SdrOle2Obj whose OLE object we want to take control of

                @param rPers 
                The SvPersist of a SwDoc (SwDoc::GetPersist()) into which we
                may want to move the object, or remove it from if unwanted.
            */
            DrawingOLEAdaptor(SdrOle2Obj &rObj, SvPersist &rPers);

            /// Destructor will destroy the owned OLE object if not transferred
            ~DrawingOLEAdaptor();

            /** Transfer ownership of the OLE object to a document's SvPersist
           
                TransferToDoc moves the object into the persist under the name
                passed in. This name is then suitable to be used as an argument
                to SwDoc::InsertOLE. 

                The object is no longer owned by the adaptor after this call,
                subsequent calls are an error and return false.

                @param rName 
                The name to store the object under in the document.  

                @return On success true is returned, otherwise false. On
                success rName is then suitable for user with SwDoc::InsertOLE
            */
            bool TransferToDoc(const String &rName);
        private:
            /// No assigning allowed
            DrawingOLEAdaptor& operator=(const DrawingOLEAdaptor&);
            /// No copying allowed
            DrawingOLEAdaptor(const DrawingOLEAdaptor &rDoc);
        };

        /** Get the Outliner Object from a SdrTextObj that contains the visible
            text

            A SdrTextObj contains an object which describes its text, but
            frustratingly if the object is being actively edited this new text
            outliner object is stored seperately from the previous preedit
            outliner. So objects being edited have to be handled differently
            from those that are not to return the currently being edited
            outliner if its exists, and the normal outliner if not. This method
            just gives me the text outliner that contains the visible text,
            which is all anyone could really care about. 

            See OpenOffice.org issue 13885
            (http://www.openoffice.org/issues/show_bug.cgi?id=13885)

            @param SdrTextObj
            The SdrTextObj from which we want to get the text content.

            @return the OutlinerParaObject that describes the user visible text

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        const OutlinerParaObject* GetOutlinerParaObject(const SdrTextObj &rObj);


#ifdef DEBUGDUMP
        /** Create a SvStream to dump data to during debugging

            This creates a file in the program dir of OOo, delete the SvStream
            after you are done with it

            @param rSuffix
            The suffix that will be appened to this debugging file

            @return a SvStream to dump data to

            @author 
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        SvStream *CreateDebuggingStream(const String &rSuffix);

        /** Dump one SvStream to another

            @param rSrc
            The source stream

            @param rDest
            The destination stream

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        void DumpStream(const SvStream &rSrc, SvStream &rDest);
#endif
    }
}

#endif
/* vi:set tabstop=4 shiftwidth=4 expandtab: */
