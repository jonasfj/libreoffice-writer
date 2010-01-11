/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: SwNumberTree.hxx,v $
 * $Revision: 1.11 $
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

#ifndef _SW_NUMBER_TREE_HXX
#define _SW_NUMBER_TREE_HXX

#include <set>
#include <vector>
#include <tools/string.hxx>
#include <swdllapi.h>
#include <SwNumberTreeTypes.hxx>

class SwNumberTreeNode;

bool SwNumberTreeNodeLessThan (const SwNumberTreeNode * pA,
                               const SwNumberTreeNode * pB);

struct compSwNumberTreeNodeLessThan
{
    bool operator()(const SwNumberTreeNode * pA,
                    const SwNumberTreeNode * pB) const
    { return SwNumberTreeNodeLessThan(pA, pB); }
};

/**
   A tree of numbered nodes.

   Simple example:

   <pre>
   1. kshdkjfs
     1.1. lskjf
   2. sdfjlksaf
   3. fka�slk
     3.1. lfjlaskf
     3.2. jaslkjflsf
       3.2.1. hkljhkjhk

   + R
     + 1 kshdkjfs
     | + 1 lskjf
     + 2 sdfjlksaf
     + 3 fka�slk
       + 1 lfjlaskf
       + 2 jaslkjflsf
         + 1 hkljhkjhk
   </pre>

   The root contains the nodes of the first level. Each node A of the
   first level contains those nodes of the second level that have the
   same first level number as A and so on for the subsidiary levels.

   The numbering label of a node A is resolved by concatenating the
   numbers of the nodes on the path from the root to A.

    ------------------------------------------

    Phantoms

    A phantom is an auxiliary node that is used to emulate numberings
    starting with nodes not at top level. The phantom contains the
    number for the level but is not considered part of the numbering.

     Constraint 1: A phantom is always the first child node.
     Constraint 2: At each node there is at most one child that is a phantom.
     Constraint 3: A phantom is the smallest of all numbering nodes.

    Uncounted Phantoms

      0.1. dljflskjlasf
    5. �ds�fka�s
      5.1.

    + R (nStart = 5)
      + 0 (phantom, not counted)
      | + 1 dljflskjlasf
      + 5 �ds�fka�s
        + 1

     The phantom gets numbered with 0. The first non-phantom node gets
     numbered with the start value.

     -----------------------------------------

     Counted Phantoms

       5.1. lgkjjgklg
     6. lkjfalskjflsaf
       6.1. ljdflaksjflkjasflkjsf

     + R (nStart = 5)
       + 5 (phantom, counted)
       | + 1 lgkjjgklg
       + 6 lkjfalskjflsaf
         + 1 ljdflaksjflkjasflkjsf

     The phantom gets numbered with the start value.
*/
class SW_DLLPUBLIC SwNumberTreeNode
{
protected:
    typedef std::set<SwNumberTreeNode *, compSwNumberTreeNodeLessThan>
    tSwNumberTreeChildren;

public:
    SwNumberTreeNode();

    virtual ~SwNumberTreeNode();

    /**
       Add a child.

       @param pChild   child to add
       @param nDepth   depth in which to add the child
     */
    void AddChild( SwNumberTreeNode* pChild,
                   const int nDepth = 0 );

    /**
       Remove a child.

       OD 2008-02-19 #refactorlists# - no longer virtual

       @param pChild     child to be removed
     */
    void RemoveChild( SwNumberTreeNode* pChild );

    /**
       Remove this child from the tree.
     */
    void RemoveMe();

    /**
       Returns the parent of this node.

       @return the parent
    */
    inline SwNumberTreeNode* GetParent() const
    {
        return mpParent;
    }

    /**
       Returns number of this node.

       @param bValidate     validate the number?

       @return number of this node
     */
    SwNumberTree::tSwNumTreeNumber GetNumber( bool bValidate = true ) const;

    // --> OD 2008-11-26 #158694#
    bool IsContinueingPreviousSubTree() const;
    // <--

    /**
       Returns level numbers of this node.

       @return level numbers of this node
     */
    SwNumberTree::tNumberVector GetNumberVector() const;

    /**
       Return if numbering is restartet at this node.
     */
    virtual bool IsRestart() const = 0;

    /**
       Return start value.

       @return start value
     */
    virtual SwNumberTree::tSwNumTreeNumber GetStartValue() const = 0;

    /**
       Return if this node is counted.

       @retval true this node is counted
       @retval false this node is NOT counted
     */
    virtual bool IsCounted() const;

    /**
       Return if this node is counted continuous.

       @retval true    This node is counted continuous.
       @retval false   else
     */
    virtual bool IsContinuous() const = 0;

    /**
       Return if a node is first non-phantom child of this node.

       @param pNode     the node to check

       @retval true     pNode is first child of this node
       @retval false    else
     */
    virtual bool IsFirst(const SwNumberTreeNode * pNode) const;

    /**
       Return if this node if the first non-phantom node in the tree.

       @retval true     this node is the first non-phantom node in the tree
       @retval false    else
     */
    virtual bool IsFirst() const;

    /**
       Return if this node is a phantom.

       @retval true this node is a phantom
       @retval false this node is NOT a phantom
     */
    bool IsPhantom() const;

    /** set level of this node

        OD 2008-03-13 #refactorlists#
        precondition: node is already member of a list tree

        @author OD
    */
    void SetLevelInListTree( const int nLevel );

    /**
       Return level of this node.

       The level of this node is the length of the path from the root
       to this node.

       @return the level of this node
     */
    int GetLevelInListTree() const;

    /**
       Returns if this node is less than another node.

       @param rTreeNode   node to compare with

       @attention A phantom node is considered the least element with
       respect to lessThan.

       @retval true this node is less than rTreeNode
       @retval false else
    */
    virtual bool LessThan(const SwNumberTreeNode & rTreeNode) const;

    /**
       Invalidate this node and all its descendants.

       All iterators holding the last valid node in the according list
       of childs are set to the end of this list, thereby stating all
       children in the list are invalid.
       OD 2007-10-26 #i83479# - made public
     */
    void InvalidateTree() const;

    /**
       Notifies all invalid children of this node.
       OD 2007-10-26 #i83479# - made public
     */
    void NotifyInvalidChildren();

    /**
       Notifies the node.

       Calls Invalidate(this) on parent.
     */
    void InvalidateMe();

    /**
       Validate the tree.

       Validates all nodes in this subtree.
    */
    void ValidateTree();

    /**
       Validates this node.

       Calls Validate(this) on parent.
    */
    void ValidateMe();

    /**
       Notifies all invalid siblings of this node.
    */
    void NotifyInvalidSiblings();

    /** notification of all nodes in the list tree on certain list level

        OD 2008-04-17 #refactorlists#
    */
    void NotifyNodesOnListLevel( const int nListLevel );

    /** Invalidation and notification of complete numbering tree

        OD 2006-04-26 #i64010#
        Usage: on <IsCounted()> state change its needed to invalidate the
               complete numbering tree due to wide influence of this change.
    */
    inline void InvalidateAndNotifyTree()
    {
        if ( GetRoot() )
        {
            GetRoot()->InvalidateTree();
            GetRoot()->Notify();
        }
    }

    /**
       Returns the greatest descendant of the root that is smaller than
       this node, aka the predecessor of this node.

       @return the predecessor
     */
    SwNumberTreeNode* GetPred( bool bSibling = false ) const;

    /** determines the node, which is preceding the node

        OD 2007-09-06 #i81002#
        The search for the preceding node is performed for the tree below the
        <this> node. To search the complete tree, the method has been called for
        the root of the tree.

        @author OD
    */
    const SwNumberTreeNode* GetPrecedingNodeOf( const SwNumberTreeNode& rNode ) const;

//    /**
//       Returns a string representation of this node.

//       @return the string representation of this node
//     */
//    virtual String ToString() const = 0;

//    /**
//       Print this subtree.

//       @param o              output stream to direct output to
//       @param rIndent        additional indent for the children of this node
//       @param rMyIndent      indent to use for this node
//       @param nDepth         number of levels to print (-1 means all levels)

//       @return output stream after output of this subtree
//     */
//    String print(const String & rIndent = String("  ",
//                                                 RTL_TEXTENCODING_ASCII_US),
//                 const String & rMyIndent = String("  ",
//                                                   RTL_TEXTENCODING_ASCII_US),
//                 int nDepth = -1) const;

#ifdef DBG_UTIL
    static unsigned long GetInstances();
    unsigned long GetSerial();
#endif

#ifdef __SW_NUMBER_TREE_SANITY_CHECK
    /**
       Sanity check.

       @param bRecursive    descend to children

       @retval true   the structure of this node is sane
       @retval false  else
     */
    bool IsSane(bool bRecursive) const;
#endif // __SW_NUMBER_TREE_SANITY_CHECK

protected:
    /**
       the children
    */
    tSwNumberTreeChildren mChildren;

    /**
       Returns the root node of the tree this node is part of.

       Important note: method call <GetRoot()->GetRoot()> returns NULL.

       @return the root
     */
    SwNumberTreeNode* GetRoot() const;

     /**
       Return if the notification is not disabled on global conditions

       @retval true   Notification enabled in general.
       @retval false  else
     */
    virtual bool IsNotificationEnabled() const = 0;

    /**
       Returns how many children this node has got.

       @return number of children
     */
    tSwNumberTreeChildren::size_type GetChildCount() const;

    // --> OD 2006-04-26 #i64010# - made pure virtual
    virtual bool HasCountedChildren() const = 0;
    // <--

    // --> OD 2006-04-26 #i64010#
    virtual bool IsCountedForNumbering() const = 0;
    // <--

    // --> OD 2008-02-19 #refactorlists#
    // method called before this tree node has been added to the list tree
    virtual void PreAdd() = 0;
    // method called after this tree node has been removed from the list tree
    virtual void PostRemove() = 0;
    // <--

#ifdef __SW_NUMBER_TREE_SANITY_CHECK
    /**
       Sanity check with loop detection.

       @param bRecursive   descend to children
       @param rParents     vector for recording path

       @retval true     this node is sane
       @retval false    else     */
    virtual bool IsSane
    (bool bRecursive, std::vector<const SwNumberTreeNode *> rParents) const;
#endif // __SW_NUMBER_TREE_SANITY_CHECK

    /**
       the parent node
    */
    SwNumberTreeNode * mpParent;

    /**
       the number of the node
    */
    mutable SwNumberTree::tSwNumTreeNumber mnNumber;

    // --> OD 2008-11-26 #158694#
    // boolean indicating, that a node of a not counted parent node is continueing
    // the numbering of parent's previous node sub tree.
    // Example:
    //   1. kshdkjfs
    //     1.1. lskjf
    //      sdfjlksaf <-- not counted parent node
    //     1.2. lfjlaskf <-- <mbContinueingPreviousSubTree = true>
    mutable bool mbContinueingPreviousSubTree;
    // <--

    /**
       true     this node is a phantom
       false    this node is NOT a phantom
     */
    bool mbPhantom;

    /**
       Iterator to the last valid element. All children that are less
       than or equal to the referenced child are valid. All children
       greater than the referenced child are invalid.
     */
    mutable tSwNumberTreeChildren::iterator mItLastValid;

#ifdef DBG_UTIL
    /**
       Counter for the number of created instances.
     */
    static unsigned long nInstances;

    /**
       Serial number.
    */
    unsigned long mnSerial;
#endif

    SwNumberTreeNode(const SwNumberTreeNode& );
    SwNumberTreeNode& operator=( const SwNumberTreeNode& );

    /**
       Calls _GetNumberVector on parent and adds number of this node
       at the end.

       @param rVector     return value
       @param bValidate   validate the number?
     */
    void _GetNumberVector( SwNumberTree::tNumberVector& rVector,
                           bool bValidate = true ) const;

    /**
       Invalidates a child.

       Calls SetLastValid for the preceeding sibling of the child and
       notifies all invalid children.

       @param pChild      the child to invalidate
     */
    void Invalidate( SwNumberTreeNode * pChild );

    /** Invalidation of all children

        OD 2005-10-19 #126009#
        Usage: on <IsCounted()> state change the children have to be invalidated
    */
    inline void InvalidateChildren()
    {
        SetLastValid( mChildren.end() );
    }

    /** Invalidation of parent node, if its not counted.

        OD 2005-10-19 #126009#
        Usage: on <IsCounted()> state change the parent have to be invalidated
    */
    inline void InvalidateNotCountedParent()
    {
        if ( GetParent() && !GetParent()->IsCountedForNumbering() )
        {
            GetParent()->InvalidateMe();
        }
    }

    /**
       Set the last valid child of this node.

       @param aItLastValid    iterator pointing to the new last valid child
       @param bValidating     - true    always set the last valid node to
                                        aItLastValid
                              - false   only set if aItLastValid is preceeding
                                        the current last valid node
     */
    void SetLastValid(tSwNumberTreeChildren::iterator aItLastValid,
                      bool bValidating = false) const;

    /**
       Set this node as last valid child of its parent.

       @param bValidation    see aboce
     */
    void SetLastValid(bool bValidating) const;

    /**
       Return if this node is notifiable.

       @attention If a not is not notifiable a notify request is *not*
       forwarded to its descendants.

       @retval true   This node is notifiable.
       @retval false  else
     */
    virtual bool IsNotifiable() const = 0;

    /**
       Notifies the node.

       Called when the number of the node got invalid.
     */
    virtual void NotifyNode() = 0;

    /**
       Notifies this node (NotifyNode) and all descendants.
     */
    void Notify();

    /** Notification of parent node siblings, if its not counted.

        OD 2005-10-19 #126009#
        Usage: on <IsCounted()> state change the parent node and its siblings
               have to be notified.
    */
    inline void NotifyNotCountedParentSiblings()
    {
        if ( GetParent() && !GetParent()->IsCountedForNumbering() )
        {
            GetParent()->NotifyInvalidSiblings();
        }
    }

    /** notification of children nodes on certain depth

        OD 2008-04-17 #refactorlists#

        @author OD
    */
    void NotifyChildrenOnDepth( const int nDepth );

    /**
       Returns if a child A this node is valid.

       A is valid if aItLastValid in parent refers to a node
       greater than of equal to A.

       @param pChild    child to be tested

       @retval true this node is valid
       @retval false this node is NOT valid
     */
    bool IsValid(const SwNumberTreeNode * pChild) const;

    /**
       Returns if this node is valid.

       @retval true    this node is valid
       @retval false   else
     */
    bool IsValid() const;

    /**
       Validates a child.

       @param pNode     child to be validated

       @attention All invalid children preceding pNode are validated, too.
     */
    void Validate(const SwNumberTreeNode * pNode) const;

    /**
       Validates a child using hierarchical numbering.

       @param pNode      child to be validated

       @attention All invalid children preceding pNode are validated, too.
     */
    void ValidateHierarchical(const SwNumberTreeNode * pNode) const;

    /**
       Validates a child using continuous numbering.

       @param pNode      child to be validated

       @attention All invalid children preceding pNode are validated, too.
     */
    void ValidateContinuous(const SwNumberTreeNode * pNode) const;

    /**
       Creates a new node of the same class.

       @return the new node
    */
    virtual SwNumberTreeNode * Create() const = 0;

    /**
       Creates a phantom.

       @return the created phantom
     */
    SwNumberTreeNode * CreatePhantom();

    /**
       Set if this node is a phantom.

       @param bPhantom   - true this node is a phantom
                         - false this node is a phantom
     */
    void SetPhantom(bool bPhantom = true);

    /**
       Return if phantoms are counted.

       OD 2008-02-19 #refactorlists# - pure virtual now

       @retval true phantoms are counted
       @retval false else
     */
    virtual bool IsCountPhantoms() const = 0;

    /**
       Return if all descendants of this node are phantoms.

       @retval true   all descendants are phantoms
       @retval false  else
     */
    bool HasOnlyPhantoms() const;

    // --> OD 2005-10-27 #126009#
    bool HasPhantomCountedParent() const;
    // <--

    /**
        HB, OD : return node, if it isn't a phantom, otherwise return first
                 non-phantom descendant.
        Returns the first child of this node that is NOT a phantom.

        @return  the first non phantom child
    */
    SwNumberTreeNode* GetFirstNonPhantomChild();

    /**
       Removes recursively phantoms that have no children.

       The resulting tree has no phantoms that either have no children or
       whose descendancy consist entirely of phantoms.
     */
    void ClearObsoletePhantoms();

    tSwNumberTreeChildren::iterator GetIterator(const SwNumberTreeNode * pChild) const;

    /**
       Moves all children to a given destination node.

       @param pDest    the destination node
     */
    void MoveChildren(SwNumberTreeNode * pDest);

    /** Moves all children of this node that are greater than a given node
        to the destination node.

        OD 2005-10-14 #125991#
        distinguish between node for comparing, whose children are greater,
        and the destination node.

        @param _rCompareNode
        input parameter - reference to the node, which is used to determine
        the greater children

        @param _rDestNode
        input parameter - reference to the node, which is the destination for
        the greater children
    */
    void MoveGreaterChildren( SwNumberTreeNode& _rCompareNode,
                              SwNumberTreeNode& _rDestNode );

    /**
       Returns the last descendant of a node, if it has children.

       @return last descendant of the node
    */
    SwNumberTreeNode* GetLastDescendant() const;

};

/**
   Functor. Checks if a certain node is less than the functor's member.
 */
struct SwNumberTreeNodeIsLessThan
{
    const SwNumberTreeNode * pNode;

    SwNumberTreeNodeIsLessThan(const SwNumberTreeNode * _pNode)
        : pNode(_pNode) {}

    bool operator()(const SwNumberTreeNode * _pNode) const
    { return SwNumberTreeNodeLessThan(_pNode, pNode); }
};
#endif // _SW_NUMBER_TREE_HXX
