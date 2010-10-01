#include "cursor.hxx"
#include "parse.hxx"
#include "visitors.hxx"
#include "document.hxx"
#include "view.hxx"

void SmCursor::Move(OutputDevice* pDev, SmMovementDirection direction, bool bMoveAnchor){
    SmCaretPosGraphEntry* NewPos = NULL;
    switch(direction){
        case MoveLeft:
        {
            //If position->Left is NULL, we want NewPos = NULL anyway...
            NewPos = position->Left;
        }break;
        case MoveRight:
        {
            //If position->Right is NULL, we want NewPos = NULL anyway...
            NewPos = position->Right;
        }break;
        case MoveUp:
            //Implementation is practically identical to MoveDown, except for a single if statement
            //so I've implemented them together and added a direction == MoveDown to the if statements.
        case MoveDown:
        {
            SmCaretLine from_line = SmCaretPos2LineVisitor(pDev, position->CaretPos).GetResult(),
                        best_line,  //Best approximated line found so far
                        curr_line;  //Current line
            long dbp_sq = 0;        //Distance squared to best line
            SmCaretPosGraphIterator it = pGraph->GetIterator();
            while(it.Next()){
                //Reject it if it's the current position
                if(it->CaretPos == position->CaretPos) continue;
                //Compute caret line
                curr_line = SmCaretPos2LineVisitor(pDev, it->CaretPos).GetResult();
                //Reject anything above if we're moving down
                if(curr_line.GetTop() <= from_line.GetTop() && direction == MoveDown) continue;
                //Reject anything below if we're moving up
                if(curr_line.GetTop() + curr_line.GetHeight() >= from_line.GetTop() + from_line.GetHeight()
                        && direction == MoveUp) continue;
                //Compare if it to what we have, if we have anything yet
                if(NewPos){
                    //Compute distance to current line squared, multiplied with a horizontial factor
                    long dp_sq = curr_line.SquaredDistanceX(from_line) * HORIZONTICAL_DISTANCE_FACTOR +
                                 curr_line.SquaredDistanceY(from_line);
                    //Discard current line if best line is closer
                    if(dbp_sq <= dp_sq) continue;
                }
                //Take current line as the best
                best_line = curr_line;
                NewPos = it.Current();
                //Update distance to best line
                dbp_sq = best_line.SquaredDistanceX(from_line) * HORIZONTICAL_DISTANCE_FACTOR +
                         best_line.SquaredDistanceY(from_line);
            }
        }break;
        default:
            j_assert(false, "Movement direction not supported!");
    }
    if(NewPos){
        position = NewPos;
        if(bMoveAnchor)
            anchor = NewPos;
        RequestRepaint();
    }
}

void SmCursor::MoveTo(OutputDevice* pDev, Point pos, bool bMoveAnchor){
    SmCaretLine best_line,  //Best line found so far, when iterating
                curr_line;  //Current line, when iterating
    SmCaretPosGraphEntry* NewPos = NULL;
    long dp_sq = 0,     //Distance to current line squared
         dbp_sq = 1;    //Distance to best line squared
    SmCaretPosGraphIterator it = pGraph->GetIterator();
    while(it.Next()){
        j_assert(it->CaretPos.IsValid(), "The caret position graph may not have invalid positions!");
        //Compute current line
        curr_line = SmCaretPos2LineVisitor(pDev, it->CaretPos).GetResult();
        //If we have a position compare to it
        if(NewPos){
            //Compute squared distance to current line
            dp_sq = curr_line.SquaredDistanceX(pos) + curr_line.SquaredDistanceY(pos);
            //If best line is closer, reject current line
            if(dbp_sq <= dp_sq) continue;
        }
        //Accept current position as the best
        best_line = curr_line;
        NewPos = it.Current();
        //Update distance to best line
        dbp_sq = best_line.SquaredDistanceX(pos) + best_line.SquaredDistanceY(pos);
    }
    if(NewPos){
        position = NewPos;
        if(bMoveAnchor)
            anchor = NewPos;
        RequestRepaint();
    }
}

void SmCursor::BuildGraph(){
    //Save the current anchor and position
    SmCaretPos _anchor, _position;
    //Release pGraph if allocated
    if(pGraph){
        if(anchor)
            _anchor = anchor->CaretPos;
        if(position)
            _position = position->CaretPos;
        delete pGraph;
        //Reset anchor and position as they point into an old graph
        anchor = NULL;
        position = NULL;
    }
    pGraph = NULL;

    //Build the new graph
    SmCaretPosGraphBuildingVisitor builder;
    pTree->Accept(&builder);
    pGraph = builder.Graph();

    //Restore anchor and position pointers
    if(_anchor.IsValid() || _position.IsValid()){
        SmCaretPosGraphIterator it = pGraph->GetIterator();
        while(it.Next()){
            if(_anchor == it->CaretPos)
                anchor = it.Current();
            if(_position == it->CaretPos)
                position = it.Current();
        }
    }
    //Set position and anchor to first caret position
    SmCaretPosGraphIterator it = pGraph->GetIterator();
    if(!position)
        position = it.Next();
    if(!anchor)
        anchor = position;

    j_assert(position->CaretPos.IsValid(), "Position must be valid");
    j_assert(anchor->CaretPos.IsValid(), "Anchor must be valid");
}

bool SmCursor::SetCaretPosition(SmCaretPos pos, bool moveAnchor){
    SmCaretPosGraphIterator it = pGraph->GetIterator();
    while(it.Next()){
        if(it->CaretPos == pos){
            position = it.Current();
            if(moveAnchor)
                anchor = it.Current();
            return true;
        }
    }
    return false;
}

void SmCursor::AnnotateSelection(){
    //TODO: Manage a state, reset it upon modification and optimize this call
    SmSetSelectionVisitor SSV(anchor->CaretPos, position->CaretPos);
    pTree->Accept(&SSV);
}

void SmCursor::Draw(OutputDevice& pDev, Point Offset){
    SmCaretDrawingVisitor(pDev, GetPosition(), Offset);
}

void SmCursor::Delete(){
    //Return if we don't have a selection to delete
    if(!HasSelection())
        return;

    //Enter edit setion
    BeginEdit();

    //Set selected on nodes
    AnnotateSelection();

    //Find an arbitrary selected node
    SmNode* pSNode = FindSelectedNode(pTree);
    j_assert(pSNode != NULL, "There must be a selection when HasSelection is true!");

    //Find the topmost node of the line that holds the selection
    SmNode* pLine = FindTopMostNodeInLine(pSNode, true);

    //Get the parent of the line
    SmStructureNode* pLineParent = pLine->GetParent();
    //Find line offset in parent
    int nLineOffset = pLineParent->IndexOfSubNode(pLine);
    j_assert(nLineOffset != -1, "pLine must be a child of it's parent!");

    //Position after delete
    SmCaretPos PosAfterDelete;

    SmNodeList* pLineList;
    if(IsLineCompositionNode(pLine))
        pLineList = LineToList((SmStructureNode*)pLine);
    else {
        pLineList = new SmNodeList();
        pLineList->push_back(pLine);
    }

    //Take the selected nodes and delete them...
    SmNodeList::iterator patchIt = TakeSelectedNodesFromList(pLineList);

    //Get teh position to set after delete
    PosAfterDelete = PatchLineList(pLineList, patchIt);

    //Finish editing
    FinishEdit(pLineList, pLineParent, nLineOffset, PosAfterDelete);
}

void SmCursor::InsertNodes(SmNodeList* pNewNodes){
    if(pNewNodes->size() == 0){
        delete pNewNodes;
        return;
    }

    //Begin edit section
    BeginEdit();

    //Position after insert should be after pNewNode
    SmCaretPos PosAfterInsert = SmCaretPos(pNewNodes->back(), 1);

    //Get the current position
    const SmCaretPos pos = position->CaretPos;

    //Find top most of line that holds position
    SmNode* pLine = FindTopMostNodeInLine(pos.pSelectedNode, false);

    //Find line parent and line index in parent
    SmStructureNode* pLineParent = pLine->GetParent();
    int nParentIndex = pLineParent->IndexOfSubNode(pLine);
    j_assert(nParentIndex != -1, "pLine must be a subnode of pLineParent!");

    //Convert line to list
    SmNodeList* pLineList;
    if(IsLineCompositionNode(pLine))
        pLineList = LineToList((SmStructureNode*)pLine);
    else {
        pLineList = new SmNodeList();
        pLineList->push_front(pLine);
    }

    //Find iterator for place to insert nodes
    SmNodeList::iterator it = FindPositionInLineList(pLineList, pos);

    //Insert all new nodes
    SmNodeList::iterator newIt,
                         patchIt = it, // (pointless default value, fixes compiler warnings)
                         insIt;
    for(newIt = pNewNodes->begin(); newIt != pNewNodes->end(); newIt++){
        insIt = pLineList->insert(it, *newIt);
        if(newIt == pNewNodes->begin())
            patchIt = insIt;
        if((*newIt)->GetType() == NTEXT)
            PosAfterInsert = SmCaretPos(*newIt, ((SmTextNode*)*newIt)->GetText().Len());
        else
            PosAfterInsert = SmCaretPos(*newIt, 1);
    }
    //Patch the places we've changed stuff
                        PatchLineList(pLineList, patchIt);
    PosAfterInsert =    PatchLineList(pLineList, it);
    //Release list, we've taken the nodes
    delete pNewNodes;
    pNewNodes = NULL;

    //Finish editing
    FinishEdit(pLineList, pLineParent, nParentIndex, PosAfterInsert);
}

SmNodeList::iterator SmCursor::FindPositionInLineList(SmNodeList* pLineList, SmCaretPos aCaretPos) {
    //Find iterator for position
    SmNodeList::iterator it;
    for(it = pLineList->begin(); it != pLineList->end(); it++){
        if(*it == aCaretPos.pSelectedNode){
            if((*it)->GetType() == NTEXT){
                //Split textnode if needed
                if(aCaretPos.Index > 0){
                    SmTextNode* pText = (SmTextNode*)aCaretPos.pSelectedNode;
                    XubString str1 = pText->GetText().Copy(0, aCaretPos.Index);
                    XubString str2 = pText->GetText().Copy(aCaretPos.Index);
                    pText->ChangeText(str1);
                    ++it;
                    //Insert str2 as new text node
                    if(str2.Len() > 0){
                        SmTextNode* pNewText = new SmTextNode(pText->GetToken(), pText->GetFontDesc());
                        pNewText->ChangeText(str2);
                        it = pLineList->insert(it, pNewText);
                    }
                }
            }else
                ++it;
            //it now pointer to the node following pos, so pLineList->insert(it, ...) will insert correctly
            return it;

        }
    }
    //If we didn't find pSelectedNode, it must be because the caret is infront of the line
    return pLineList->begin();
}

SmCaretPos SmCursor::PatchLineList(SmNodeList* pLineList, SmNodeList::iterator aIter) {
    //The nodes we should consider merging
    SmNode *prev = NULL,
           *next = NULL;
    if(aIter != pLineList->end())
        next = *aIter;
    if(aIter != pLineList->begin()) {
        aIter--;
        prev = *aIter;
        aIter++;
    }

    //Check if there's textnodes to merge
    if( prev &&
        next &&
        prev->GetType() == NTEXT &&
        next->GetType() == NTEXT &&
        ( prev->GetToken().eType != TNUMBER ||
          next->GetToken().eType == TNUMBER) ){
        SmTextNode *pText = (SmTextNode*)prev,
                   *pOldN = (SmTextNode*)next;
        SmCaretPos retval(pText, pText->GetText().Len());
        String newText;
        newText += pText->GetText();
        newText += pOldN->GetText();
        pText->ChangeText(newText);
        delete pOldN;
        pLineList->erase(aIter);
        return retval;
    }

    //Check if there's a SmPlaceNode to remove:
    if(prev && next && prev->GetType() == NPLACE && !SmNodeListParser::IsOperator(next->GetToken())){
        aIter--;
        aIter = pLineList->erase(aIter);
        delete prev;
        //Return caret pos infront of aIter
        if(aIter != pLineList->begin())
            aIter--; //Thus find node before aIter
        if(aIter == pLineList->begin())
            return SmCaretPos();
        if((*aIter)->GetType() == NTEXT)
            return SmCaretPos(*aIter, ((SmTextNode*)*aIter)->GetText().Len());
        return SmCaretPos(*aIter, 1);
    }
    if(prev && next && next->GetType() == NPLACE && !SmNodeListParser::IsOperator(prev->GetToken())){
        aIter = pLineList->erase(aIter);
        delete next;
        if(prev->GetType() == NTEXT)
            return SmCaretPos(prev, ((SmTextNode*)prev)->GetText().Len());
        return SmCaretPos(prev, 1);
    }

    //If we didn't do anything return
    if(!prev) //return an invalid to indicate we're infront of line
        return SmCaretPos();
    if(prev->GetType() == NTEXT)
        return SmCaretPos(prev, ((SmTextNode*)prev)->GetText().Len());
    return SmCaretPos(prev, 1);
}

SmNodeList::iterator SmCursor::TakeSelectedNodesFromList(SmNodeList *pLineList,
                                                         SmNodeList *pSelectedNodes) {
    SmNodeList::iterator retval;
    SmNodeList::iterator it = pLineList->begin();
    while(it != pLineList->end()){
        if((*it)->IsSelected()){
            //Split text nodes
            if((*it)->GetType() == NTEXT) {
                SmTextNode* pText = (SmTextNode*)*it;
                String aText = pText->GetText();
                //Start and lengths of the segments, 2 is the selected segment
                int start1 = 0,
                    start2 = pText->GetSelectionStart(),
                    start3 = pText->GetSelectionEnd(),
                    len1 = start2 - 0,
                    len2 = start3 - start2,
                    len3 = aText.Len() - start3;
                SmToken aToken = pText->GetToken();
                USHORT eFontDesc = pText->GetFontDesc();
                //If we need make segment 1
                if(len1 > 0) {
                    String str = aText.Copy(start1, len1);
                    pText->ChangeText(str);
                    it++;
                } else {//Remove it if not needed
                    it = pLineList->erase(it);
                    delete pText;
                }
                //Set retval to be right after the selection
                retval = it;
                //if we need make segment 3
                if(len3 > 0) {
                    String str = aText.Copy(start3, len3);
                    SmTextNode* pSeg3 = new SmTextNode(aToken, eFontDesc);
                    pSeg3->ChangeText(str);
                    retval = pLineList->insert(it, pSeg3);
                }
                //If we need to save the selected text
                if(pSelectedNodes && len2 > 0) {
                    String str = aText.Copy(start2, len2);
                    SmTextNode* pSeg2 = new SmTextNode(aToken, eFontDesc);
                    pSeg2->ChangeText(str);
                    pSelectedNodes->push_back(pSeg2);
                }
            } else { //if it's not textnode
                SmNode* pNode = *it;
                retval = it = pLineList->erase(it);
                if(pSelectedNodes)
                    pSelectedNodes->push_back(pNode);
                else
                    delete pNode;
            }
        } else
            it++;
    }
    return retval;
}

void SmCursor::InsertSubSup(SmSubSup eSubSup) {
    AnnotateSelection();

    //Find line
    SmNode *pLine;
    if(HasSelection()) {
        SmNode *pSNode = FindSelectedNode(pTree);
        j_assert(pSNode != NULL, "There must be a selected node when HasSelection is true!");
        pLine = FindTopMostNodeInLine(pSNode, TRUE);
    } else
        pLine = FindTopMostNodeInLine(position->CaretPos.pSelectedNode, FALSE);

    //Find Parent and offset in parent
    SmStructureNode *pLineParent = pLine->GetParent();
    int nParentIndex = pLineParent->IndexOfSubNode(pLine);
    j_assert(nParentIndex != -1, "pLine must be a subnode of pLineParent!");

    //TODO: Consider handling special cases where parent is an SmOperNode,
    //      Maybe this method should be able to add limits to an SmOperNode...

    //We begin modifying the tree here
    BeginEdit();

    //Convert line to list
    SmNodeList* pLineList;
    if(IsLineCompositionNode(pLine))
        pLineList = LineToList((SmStructureNode*)pLine);
    else {
        pLineList = new SmNodeList();
        pLineList->push_front(pLine);
    }

    //Take the selection, and/or find iterator for current position
    SmNodeList* pSelectedNodesList = new SmNodeList();
    SmNodeList::iterator it;
    if(HasSelection())
        it = TakeSelectedNodesFromList(pLineList, pSelectedNodesList);
    else
        it = FindPositionInLineList(pLineList, position->CaretPos);

    //Find node that this should be applied to
    SmNode* pSubject;
    BOOL bPatchLine = pSelectedNodesList->size() > 0; //If the line should be patched later
    if(it != pLineList->begin()) {
        it--;
        pSubject = *it;
        it++;
    } else {
        //Create a new place node
        pSubject = new SmPlaceNode();
        pSubject->Prepare(pDocShell->GetFormat(), *pDocShell);
        it = pLineList->insert(it, pSubject);
        it++;
        bPatchLine = TRUE;  //We've modified the line it should be patched later.
    }

    //Wrap the subject in a SmSubSupNode
    SmSubSupNode* pSubSup;
    if(pSubject->GetType() != NSUBSUP){
        SmToken token;
        token.nGroup = TGPOWER;
        pSubSup = new SmSubSupNode(token);
        pSubSup->SetBody(pSubject);
        *(--it) = pSubSup;
        it++;
    }else
        pSubSup = (SmSubSupNode*)pSubject;
    //pSubject shouldn't be referenced anymore, pSubSup is the SmSubSupNode in pLineList we wish to edit.
    //and it pointer to the element following pSubSup in pLineList.
    pSubject = NULL;

    //Patch the line if we noted that was needed previously
    if(bPatchLine)
        PatchLineList(pLineList, it);

    //Convert existing, if any, sub-/superscript line to list
    SmNode *pScriptLine = pSubSup->GetSubSup(eSubSup);
    SmNodeList* pScriptLineList;
    if(pScriptLine && IsLineCompositionNode(pScriptLine))
        pScriptLineList = LineToList((SmStructureNode*)pScriptLine);
    else{
        pScriptLineList = new SmNodeList();
        if(pScriptLine)
            pScriptLineList->push_front(pScriptLine);
    }

    //Add selection to pScriptLineList
    unsigned int nOldSize = pScriptLineList->size();
    pScriptLineList->insert(pScriptLineList->end(), pSelectedNodesList->begin(), pSelectedNodesList->end());
    delete pSelectedNodesList;
    pSelectedNodesList = NULL;

    //Patch pScriptLineList if needed
    if(0 < nOldSize && nOldSize < pScriptLineList->size()) {
        SmNodeList::iterator iPatchPoint = pScriptLineList->begin();
        std::advance(iPatchPoint, nOldSize);
        PatchLineList(pScriptLineList, iPatchPoint);
    }

    //Find caret pos, that should be used after sub-/superscription.
    SmCaretPos PosAfterScript; //Leave invalid for first position
    if(pScriptLineList->size() > 0)
        PosAfterScript = SmCaretPos::GetPosAfter(pScriptLineList->back());

    //Parse pScriptLineList
    pScriptLine = SmNodeListParser().Parse(pScriptLineList);
    delete pScriptLineList;
    pScriptLineList = NULL;

    //Insert pScriptLine back into the tree
    pSubSup->SetSubSup(eSubSup, pScriptLine);

    //Finish editing
    FinishEdit(pLineList, pLineParent, nParentIndex, PosAfterScript, pScriptLine);
}

BOOL SmCursor::InsertLimit(SmSubSup eSubSup, BOOL bMoveCaret) {
    //Find a subject to set limits on
    SmOperNode *pSubject = NULL;
    //Check if pSelectedNode might be a subject
    if(position->CaretPos.pSelectedNode->GetType() == NOPER)
        pSubject = (SmOperNode*)position->CaretPos.pSelectedNode;
    else {
        //If not, check if parent of the current line is a SmOperNode
        SmNode *pLineNode = FindTopMostNodeInLine(position->CaretPos.pSelectedNode, FALSE);
        if(pLineNode->GetParent() && pLineNode->GetParent()->GetType() == NOPER)
            pSubject = (SmOperNode*)pLineNode->GetParent();
    }

    //Abort operation if we're not in the appropriate context
    if(!pSubject)
        return FALSE;

    BeginEdit();

    //Find the sub sup node
    SmSubSupNode *pSubSup = NULL;
    //Check if there's already one there...
    if(pSubject->GetSubNode(0)->GetType() == NSUBSUP)
        pSubSup = (SmSubSupNode*)pSubject->GetSubNode(0);
    else { //if not create a new SmSubSupNode
        SmToken token;
        token.nGroup = TGLIMIT;
        pSubSup = new SmSubSupNode(token);
        //Set it's body
        pSubSup->SetBody(pSubject->GetSubNode(0));
        //Replace the operation of the SmOperNode
        pSubject->SetSubNode(0, pSubSup);
    }

    //Create the limit, if needed
    SmCaretPos PosAfterLimit;
    SmNode *pLine;
    if(!pSubSup->GetSubSup(eSubSup)){
        pLine = new SmPlaceNode();
        pSubSup->SetSubSup(eSubSup, pLine);
        PosAfterLimit = SmCaretPos(pLine, 1);
    //If it's already there... let's move the caret
    } else if(bMoveCaret){
        pLine = pSubSup->GetSubSup(eSubSup);
        SmNodeList* pLineList;
        if(IsLineCompositionNode(pLine))
            pLineList = LineToList((SmStructureNode*)pLine);
        else {
            pLineList = new SmNodeList();
            pLineList->push_front(pLine);
        }
        if(pLineList->size() > 0)
            PosAfterLimit = SmCaretPos::GetPosAfter(pLineList->back());
        pLine = SmNodeListParser().Parse(pLineList);
        delete pLineList;
        pSubSup->SetSubSup(eSubSup, pLine);
    }

    //Rebuild graph of caret positions
    BuildGraph();
    AnnotateSelection();

    //Set caret position
    if(bMoveCaret)
        if(!SetCaretPosition(PosAfterLimit, true))
            SetCaretPosition(SmCaretPos(pLine, 0), true);

    EndEdit();

    return TRUE;
}

void SmCursor::InsertBrackets(SmBracketType eBracketType) {
    BeginEdit();

    AnnotateSelection();

    //Find line
    SmNode *pLine;
    if(HasSelection()) {
        SmNode *pSNode = FindSelectedNode(pTree);
        j_assert(pSNode != NULL, "There must be a selected node if HasSelection()");
        pLine = FindTopMostNodeInLine(pSNode, TRUE);
    } else
        pLine = FindTopMostNodeInLine(position->CaretPos.pSelectedNode, FALSE);

    //Find parent and offset in parent
    SmStructureNode *pLineParent = pLine->GetParent();
    int nParentIndex = pLineParent->IndexOfSubNode(pLine);
    j_assert( nParentIndex != -1, "pLine must be a subnode of pLineParent!");

    //Convert line to list
    SmNodeList *pLineList;
    if(IsLineCompositionNode(pLine))
        pLineList = LineToList((SmStructureNode*)pLine);
    else {
        pLineList = new SmNodeList();
        pLineList->push_front(pLine);
    }

    //Take the selection, and/or find iterator for current position
    SmNodeList *pSelectedNodesList = new SmNodeList();
    SmNodeList::iterator it;
    if(HasSelection())
        it = TakeSelectedNodesFromList(pLineList, pSelectedNodesList);
    else
        it = FindPositionInLineList(pLineList, position->CaretPos);

    //If there's no selected nodes, create a place node
    SmCaretPos PosAfterInsert;
    if(pSelectedNodesList->size() == 0) {
        SmNode* pPlace = new SmPlaceNode();
        PosAfterInsert = SmCaretPos(pPlace, 1);
        pSelectedNodesList->push_front(pPlace);
    }

    //Parse body nodes
    SmNode *pBodyNode = SmNodeListParser().Parse(pSelectedNodesList);
    delete pSelectedNodesList;

    //Create SmBraceNode
    SmToken aTok(TLEFT, '\0', "left", 0, 5);
    SmBraceNode *pBrace = new SmBraceNode(aTok);
    pBrace->SetScaleMode(SCALE_HEIGHT);
    SmNode *pLeft = CreateBracket(eBracketType, true),
           *pRight = CreateBracket(eBracketType, false);
    SmBracebodyNode *pBody = new SmBracebodyNode(SmToken());
    pBody->SetSubNodes(pBodyNode, NULL);
    pBrace->SetSubNodes(pLeft, pBody, pRight);
    pBrace->Prepare(pDocShell->GetFormat(), *pDocShell);

    //Insert into line
    pLineList->insert(it, pBrace);
    //Patch line (I think this is good enough)
    SmCaretPos pAfter = PatchLineList(pLineList, it);
    if( !PosAfterInsert.IsValid() )
        PosAfterInsert = pAfter;

    //Finish editing
    FinishEdit(pLineList, pLineParent, nParentIndex, PosAfterInsert);
}

SmNode *SmCursor::CreateBracket(SmBracketType eBracketType, BOOL bIsLeft) {
    SmToken aTok;
    if(bIsLeft){
        switch(eBracketType){
            case NoneBrackets:
                aTok = SmToken(TNONE, '\0', "none", TGLBRACES | TGRBRACES, 0);
                break;
            case RoundBrackets:
                aTok = SmToken(TLPARENT, MS_LPARENT, "(", TGLBRACES, 5);
                break;
            case SquareBrackets:
                aTok = SmToken(TLBRACKET, MS_LBRACKET, "[", TGLBRACES, 5);
                break;
            case DoubleSquareBrackets:
                aTok = SmToken(TLDBRACKET, MS_LDBRACKET, "ldbracket", TGLBRACES, 5);
                break;
            case LineBrackets:
                aTok = SmToken(TLLINE, MS_LINE, "lline", TGLBRACES, 5);
                break;
            case DoubleLineBrackets:
                aTok = SmToken(TLDLINE, MS_DLINE, "ldline", TGLBRACES, 5);
                break;
            case CurlyBrackets:
                aTok = SmToken(TLBRACE, MS_LBRACE, "lbrace", TGLBRACES, 5);
                break;
            case AngleBrackets:
                aTok = SmToken(TLANGLE, MS_LANGLE, "langle", TGLBRACES, 5);
                break;
            case CeilBrackets:
                aTok = SmToken(TLCEIL, MS_LCEIL, "lceil", TGLBRACES, 5);
                break;
            case FloorBrackets:
                aTok = SmToken(TLFLOOR, MS_LFLOOR, "lfloor", TGLBRACES, 5);
                break;
        }
    } else {
        switch(eBracketType) {
            case NoneBrackets:
                aTok = SmToken(TNONE, '\0', "none", TGLBRACES | TGRBRACES, 0);
                break;
            case RoundBrackets:
                aTok = SmToken(TRPARENT, MS_RPARENT, ")", TGRBRACES, 5);
                break;
            case SquareBrackets:
                aTok = SmToken(TRBRACKET, MS_RBRACKET, "]", TGRBRACES, 5);
                break;
            case DoubleSquareBrackets:
                aTok = SmToken(TRDBRACKET, MS_RDBRACKET, "rdbracket", TGRBRACES, 5);
                break;
            case LineBrackets:
                aTok = SmToken(TRLINE, MS_LINE, "rline", TGRBRACES, 5);
                break;
            case DoubleLineBrackets:
                aTok = SmToken(TRDLINE, MS_DLINE, "rdline", TGRBRACES, 5);
                break;
            case CurlyBrackets:
                aTok = SmToken(TRBRACE, MS_RBRACE, "rbrace", TGRBRACES, 5);
                break;
            case AngleBrackets:
                aTok = SmToken(TRANGLE, MS_RANGLE, "rangle", TGRBRACES, 5);
                break;
            case CeilBrackets:
                aTok = SmToken(TRCEIL, MS_RCEIL, "rceil", TGRBRACES, 5);
                break;
            case FloorBrackets:
                aTok = SmToken(TRFLOOR, MS_RFLOOR, "rfloor", TGRBRACES, 5);
                break;
        }
    }
    SmNode* pRetVal = new SmMathSymbolNode(aTok);
    pRetVal->SetScaleMode(SCALE_HEIGHT);
    return pRetVal;
}

BOOL SmCursor::InsertRow() {
    AnnotateSelection();

    //Find line
    SmNode *pLine;
    if(HasSelection()) {
        SmNode *pSNode = FindSelectedNode(pTree);
        j_assert(pSNode != NULL, "There must be a selected node if HasSelection()");
        pLine = FindTopMostNodeInLine(pSNode, TRUE);
    } else
        pLine = FindTopMostNodeInLine(position->CaretPos.pSelectedNode, FALSE);

    //Find parent and offset in parent
    SmStructureNode *pLineParent = pLine->GetParent();
    int nParentIndex = pLineParent->IndexOfSubNode(pLine);
    j_assert( nParentIndex != -1, "pLine must be a subnode of pLineParent!");

    //Discover the context of this command
    SmTableNode  *pTable  = NULL;
    SmMatrixNode *pMatrix = NULL;
    int nTableIndex = nParentIndex;
    if(pLineParent->GetType() == NTABLE)
        pTable = (SmTableNode*)pLineParent;
    //If it's warped in a SmLineNode, we can still insert a newline
    else if(pLineParent->GetType() == NLINE &&
            pLineParent->GetParent() &&
            pLineParent->GetParent()->GetType() == NTABLE) {
        //NOTE: This hack might give problems if we stop ignoring SmAlignNode
        pTable = (SmTableNode*)pLineParent->GetParent();
        nTableIndex = pTable->IndexOfSubNode(pLineParent);
        j_assert(nTableIndex != -1, "pLineParent must be a child of its parent!");
    }
    if(pLineParent->GetType() == NMATRIX)
        pMatrix = (SmMatrixNode*)pLineParent;

    //If we're not in a context that supports InsertRow, return FALSE
    if(!pTable && !pMatrix)
        return FALSE;

    //Now we start editing
    BeginEdit();

    //Convert line to list
    SmNodeList *pLineList;
    if(IsLineCompositionNode(pLine))
        pLineList = LineToList((SmStructureNode*)pLine);
    else {
        pLineList = new SmNodeList();
        pLineList->push_front(pLine);
    }

    //Find position in line
    SmNodeList::iterator it;
    if(HasSelection()) {
        //Take the selected nodes and delete them...
        it = TakeSelectedNodesFromList(pLineList);
    } else
        it = FindPositionInLineList(pLineList, position->CaretPos);

    //New caret position after inserting the newline/row in whatever context
    SmCaretPos PosAfterInsert;

    //If we're in the context of a table
    if(pTable) {
        SmNodeList *pNewLineList = new SmNodeList();
        //Move elements from pLineList to pNewLineList
        pNewLineList->splice(pNewLineList->begin(), *pLineList, it, pLineList->end());
        //Make sure it is valid again
        it = pLineList->end();
        if(it != pLineList->begin())
            it--;
        if(pNewLineList->size() == 0)
            pNewLineList->push_front(new SmPlaceNode());
        //Parse new line
        SmNode *pNewLine = SmNodeListParser().Parse(pNewLineList);
        delete pNewLineList;
        //Get position before we wrap in SmLineNode
        //NOTE: This should be done after, if SmLineNode ever becomes a line composition node
        PosAfterInsert = SmCaretPos(pNewLine, 0);
        //Wrap pNewLine in SmLineNode if needed
        if(pLineParent->GetType() == NLINE) {
            SmLineNode *pNewLineNode = new SmLineNode(SmToken(TNEWLINE, '\0', "newline"));
            pNewLineNode->SetSubNodes(pNewLine, NULL);
            pNewLine = pNewLineNode;
        }
        //Move other nodes if needed
        for( int i = pTable->GetNumSubNodes(); i > nTableIndex + 1; i--)
            pTable->SetSubNode(i, pTable->GetSubNode(i-1));
        //Insert new line
        pTable->SetSubNode(nTableIndex + 1, pNewLine);
        //Check if we need to change token type:
        if(pTable->GetNumSubNodes() > 2 && pTable->GetToken().eType == TBINOM) {
            SmToken tok = pTable->GetToken();
            tok.eType = TSTACK;
            pTable->SetToken(tok);
        }
    }
    //If we're in the context of a matrix
    else if(pMatrix) {
        //Find position after insert and patch the list
        PosAfterInsert = PatchLineList(pLineList, it);
        //Move other children
        USHORT rows = pMatrix->GetNumRows();
        USHORT cols = pMatrix->GetNumCols();
        int nRowStart = (nParentIndex - nParentIndex % cols) + cols;
        for( int i = pMatrix->GetNumSubNodes() + cols - 1; i >= nRowStart + cols; i--)
            pMatrix->SetSubNode(i, pMatrix->GetSubNode(i - cols));
        for( int i = nRowStart; i < nRowStart + cols; i++) {
            SmPlaceNode *pNewLine = new SmPlaceNode();
            if(i == nParentIndex + cols)
                PosAfterInsert = SmCaretPos(pNewLine, 0);
            pMatrix->SetSubNode(i, pNewLine);
        }
        pMatrix->SetRowCol(rows + 1, cols);
    } else
        j_assert(FALSE, "We must be either the context of a table or matrix!");

    //Finish editing
    FinishEdit(pLineList, pLineParent, nParentIndex, PosAfterInsert);
    //FinishEdit is actually used to handle siturations where parent is an instance of
    //SmSubSupNode. In this case parent should always be a table or matrix, however, for
    //code reuse we just use FinishEdit() here too.
    return TRUE;
}

void SmCursor::InsertFraction() {
    AnnotateSelection();

    //Find line
    SmNode *pLine;
    if(HasSelection()) {
        SmNode *pSNode = FindSelectedNode(pTree);
        j_assert(pSNode != NULL, "There must be a selected node when HasSelection is true!");
        pLine = FindTopMostNodeInLine(pSNode, TRUE);
    } else
        pLine = FindTopMostNodeInLine(position->CaretPos.pSelectedNode, FALSE);

    //Find Parent and offset in parent
    SmStructureNode *pLineParent = pLine->GetParent();
    int nParentIndex = pLineParent->IndexOfSubNode(pLine);
    j_assert(nParentIndex != -1, "pLine must be a subnode of pLineParent!");

    //We begin modifying the tree here
    BeginEdit();

    //Convert line to list
    SmNodeList* pLineList;
    if(IsLineCompositionNode(pLine))
        pLineList = LineToList((SmStructureNode*)pLine);
    else {
        pLineList = new SmNodeList();
        pLineList->push_front(pLine);
    }

    //Take the selection, and/or find iterator for current position
    SmNodeList* pSelectedNodesList = new SmNodeList();
    SmNodeList::iterator it;
    if(HasSelection())
        it = TakeSelectedNodesFromList(pLineList, pSelectedNodesList);
    else
        it = FindPositionInLineList(pLineList, position->CaretPos);

    //Create pNum, and pDenom
    if(pSelectedNodesList->size() == 0)
        pSelectedNodesList->push_front(new SmPlaceNode());
    SmNode *pNum   = SmNodeListParser().Parse(pSelectedNodesList),
           *pDenom = new SmPlaceNode();
    delete pSelectedNodesList;
    pSelectedNodesList = NULL;

    //Create new fraction
    SmBinVerNode *pFrac = new SmBinVerNode(SmToken(TOVER, '\0', "over", TGPRODUCT, 0));
    SmNode *pRect = new SmRectangleNode(SmToken());
    pFrac->SetSubNodes(pNum, pRect, pDenom);

    //Insert in pLineList
    SmNodeList::iterator patchIt = pLineList->insert(it, pFrac);
    PatchLineList(pLineList, patchIt);
    PatchLineList(pLineList, it);

    //Finish editing
    FinishEdit(pLineList, pLineParent, nParentIndex, SmCaretPos(pDenom, 1));
}


void SmCursor::InsertText(XubString aString){
    BeginEdit();

    Delete();

    SmToken token;
    token.eType = TIDENT;
    token.cMathChar = '\0';
    token.nGroup = 0;
    token.nLevel = 5;
    token.aText = aString;

    SmTextNode* pText = new SmTextNode(token, FNT_VARIABLE);

    //Prepare the new node
    pText->Prepare(pDocShell->GetFormat(), *pDocShell);
    pText->AdjustFontDesc();

    SmNodeList* pList = new SmNodeList();
    pList->push_front(pText);
    InsertNodes(pList);

    EndEdit();
}

void SmCursor::InsertElement(SmFormulaElement element){
    BeginEdit();

    Delete();

    //Create new node
    SmNode* pNewNode = NULL;
    switch(element){
        case BlankElement:
        {
            SmToken token;
            token.nGroup = TGBLANK;
            token.aText.AssignAscii("~");
            pNewNode = new SmBlankNode(token);
        }break;
        case FactorialElement:
        {
            SmToken token(TFACT, MS_FACT, "fact", TGUNOPER, 5);
            pNewNode = new SmMathSymbolNode(token);
        }break;
        case PlusElement:
        {
            SmToken token;
            token.eType = TPLUS;
            token.cMathChar = MS_PLUS;
            token.nGroup = TGUNOPER | TGSUM;
            token.nLevel = 5;
            token.aText.AssignAscii("+");
            pNewNode = new SmMathSymbolNode(token);
        }break;
        case MinusElement:
        {
            SmToken token;
            token.eType = TMINUS;
            token.cMathChar = MS_MINUS;
            token.nGroup = MS_PLUS;
            token.nLevel = 5;
            token.aText.AssignAscii("-");
            pNewNode = new SmMathSymbolNode(token);
        }break;
        case CDotElement:
        {
            SmToken token;
            token.eType = TCDOT;
            token.cMathChar = MS_CDOT;
            token.nGroup = TGPRODUCT;
            token.aText.AssignAscii("cdot");
            pNewNode = new SmMathSymbolNode(token);
        }break;
        case EqualElement:
        {
            SmToken token;
            token.eType = TASSIGN;
            token.cMathChar = MS_ASSIGN;
            token.nGroup = TGRELATION;
            token.aText.AssignAscii("=");
            pNewNode = new SmMathSymbolNode(token);
        }break;
        case LessThanElement:
        {
            SmToken token;
            token.eType = TLT;
            token.cMathChar = MS_LT;
            token.nGroup = TGRELATION;
            token.aText.AssignAscii("<");
            pNewNode = new SmMathSymbolNode(token);
        }break;
        case GreaterThanElement:
        {
            SmToken token;
            token.eType = TGT;
            token.cMathChar = MS_GT;
            token.nGroup = TGRELATION;
            token.aText.AssignAscii(">");
            pNewNode = new SmMathSymbolNode(token);
        }break;
        default:
            j_assert(false, "Element unknown!");
    }
    j_assert(pNewNode != NULL, "No new node was created!");
    if(!pNewNode)
        return;

    //Prepare the new node
    pNewNode->Prepare(pDocShell->GetFormat(), *pDocShell);

    //Insert new node
    SmNodeList* pList = new SmNodeList();
    pList->push_front(pNewNode);
    InsertNodes(pList);

    EndEdit();
}

void SmCursor::InsertSpecial(XubString aString) {
    BeginEdit();
    Delete();

    aString.EraseLeadingAndTrailingChars();
    aString.EraseLeadingChars('%');

    //Create instance of special node
    SmToken token;
    token.eType = TSPECIAL;
    token.cMathChar = '\0';
    token.nGroup = 0;
    token.nLevel = 5;
    token.aText = aString; //Don't know if leading "%" should  be removed
    SmSpecialNode* pSpecial = new SmSpecialNode(token);

    //Prepare the special node
    pSpecial->Prepare(pDocShell->GetFormat(), *pDocShell);

    //Insert the node
    SmNodeList* pList = new SmNodeList();
    pList->push_front(pSpecial);
    InsertNodes(pList);

    EndEdit();
}

void SmCursor::InsertCommand(USHORT nCommand) {
    switch(nCommand){
        case RID_NEWLINE:
            InsertRow();
            break;
        case RID_FROMX:
            InsertLimit(CSUB, TRUE);
            break;
        case RID_TOX:
            InsertLimit(CSUP, TRUE);
            break;
        case RID_FROMXTOY:
            if(InsertLimit(CSUB, FALSE))
                InsertLimit(CSUP, TRUE);
            break;
        default:
            InsertCommandText(SmResId(nCommand));
            break;
    }
}

void SmCursor::InsertCommandText(XubString aCommandText) {
    //Parse the the sub expression
    SmNode* pSubExpr = SmParser().ParseExpression(aCommandText);

    //Prepare the subtree
    pSubExpr->Prepare(pDocShell->GetFormat(), *pDocShell);

    //Convert subtree to list
    SmNodeList* pLineList;
    if(IsLineCompositionNode(pSubExpr))
        pLineList = LineToList((SmStructureNode*)pSubExpr);
    else {
        pLineList = new SmNodeList();
        pLineList->push_front(pSubExpr);
    }

    BeginEdit();

    //Delete any selection
    Delete();

    //Insert it
    InsertNodes(pLineList);

    EndEdit();
}

void SmCursor::Copy(){
    if(!HasSelection())
        return;

    //Find selected node
    SmNode* pSNode = FindSelectedNode(pTree);
    //Find visual line
    SmNode* pLine = FindTopMostNodeInLine(pSNode, true);

    //Clone selected nodes
    SmNodeList* pList;
    if(IsLineCompositionNode(pLine))
        pList = CloneLineToList((SmStructureNode*)pLine, true);
    else{
        pList = new SmNodeList();
        //Special care to only clone selected text
        if(pLine->GetType() == NTEXT) {
            SmTextNode *pText = (SmTextNode*)pLine;
            SmTextNode *pClone = new SmTextNode( pText->GetToken(), pText->GetFontDesc() );
            int start  = pText->GetSelectionStart(),
                length = pText->GetSelectionEnd() - pText->GetSelectionStart();
            pClone->ChangeText(pText->GetText().Copy(start, length));
            pClone->SetScaleMode(pText->GetScaleMode());
            pList->push_front(pClone);
        } else {
            SmCloningVisitor aCloneFactory;
            pList->push_front(aCloneFactory.Clone(pLine));
        }
    }

    //Set clipboard
    if(pList->size() > 0)
        SetClipboard(pList);
}

void SmCursor::Paste() {
    BeginEdit();
    Delete();

    if(pClipboard && pClipboard->size() > 0)
        InsertNodes(CloneList(pClipboard));

    EndEdit();
}

SmNodeList* SmCursor::CloneList(SmNodeList* pList){
    SmCloningVisitor aCloneFactory;
    SmNodeList* pClones = new SmNodeList();

    SmNodeList::iterator it;
    for(it = pList->begin(); it != pList->end(); it++){
        SmNode *pClone = aCloneFactory.Clone(*it);
        pClones->push_back(pClone);
    }

    return pClones;
}


void SmCursor::SetClipboard(SmNodeList* pList){
    if(pClipboard){
        //Delete all nodes on the clipboard
        SmNodeList::iterator it;
        for(it = pClipboard->begin(); it != pClipboard->end(); it++)
            delete (*it);
        delete pClipboard;
    }
    pClipboard = pList;
}

SmNode* SmCursor::FindTopMostNodeInLine(SmNode* pSNode, bool MoveUpIfSelected){
    //If we haven't got a subnode
    if(!pSNode)
        return NULL;

    //Move up parent untill we find a node who's
    //parent isn't selected and not a type of:
    //      SmExpressionNode
    //      SmBinHorNode
    //      SmUnHorNode
    //      SmAlignNode
    //      SmFontNode
    while((MoveUpIfSelected && pSNode->GetParent()->IsSelected()) ||
          IsLineCompositionNode(pSNode->GetParent())){
        pSNode = pSNode->GetParent();
        j_assert(pSNode, "pSNode shouldn't be NULL, have we hit root node if so, this is bad!");
        if(!pSNode) //I've got to do something, nothing is probably the best solution :)
            return NULL;
    }
    //Now we have the selection line node
    return pSNode;
}

SmNode* SmCursor::FindSelectedNode(SmNode* pNode){
    SmNodeIterator it(pNode);
    while(it.Next()){
        if(it->IsSelected())
            return it.Current();
        SmNode* pRetVal = FindSelectedNode(it.Current());
        if(pRetVal)
            return pRetVal;
    }
    return NULL;
}

SmNodeList* SmCursor::LineToList(SmStructureNode* pLine, SmNodeList* list){
    SmNodeIterator it(pLine);
    while(it.Next()){
        switch(it->GetType()){
            case NUNHOR:
            case NEXPRESSION:
            case NBINHOR:
            case NALIGN:
            case NFONT:
                LineToList((SmStructureNode*)it.Current(), list);
                break;
            case NERROR:
                delete it.Current();
                break;
            default:
                list->push_back(it.Current());
        }
    }
    SmNodeArray emptyArray(0);
    pLine->SetSubNodes(emptyArray);
    delete pLine;
    return list;
}

SmNodeList* SmCursor::CloneLineToList(SmStructureNode* pLine, bool bOnlyIfSelected, SmNodeList* pList){
    SmCloningVisitor aCloneFactory;
    SmNodeIterator it(pLine);
    while(it.Next()){
        if( IsLineCompositionNode( it.Current() ) )
            CloneLineToList( (SmStructureNode*)it.Current(), bOnlyIfSelected, pList );
        else if( (!bOnlyIfSelected || it->IsSelected()) && it->GetType() != NERROR ) {
            //Only clone selected text from SmTextNode
            if(it->GetType() == NTEXT) {
                SmTextNode *pText = (SmTextNode*)it.Current();
                SmTextNode *pClone = new SmTextNode( it->GetToken(), pText->GetFontDesc() );
                int start = pText->GetSelectionStart(),
                    length = pText->GetSelectionEnd() - pText->GetSelectionStart();
                pClone->ChangeText(pText->GetText().Copy(start, length));
                pClone->SetScaleMode(pText->GetScaleMode());
                pList->push_back(pClone);
            } else
                pList->push_back(aCloneFactory.Clone(it.Current()));
        }
    }
    return pList;
}

bool SmCursor::IsLineCompositionNode(SmNode* pNode){
    switch(pNode->GetType()){
        case NUNHOR:
        case NEXPRESSION:
        case NBINHOR:
        case NALIGN:
        case NFONT:
            return true;
        default:
            return false;
    }
    return false;
}

int SmCursor::CountSelectedNodes(SmNode* pNode){
    int nCount = 0;
    SmNodeIterator it(pNode);
    while(it.Next()){
        if(it->IsSelected() && !IsLineCompositionNode(it.Current()))
            nCount++;
        nCount += CountSelectedNodes(it.Current());
    }
    return nCount;
}

bool SmCursor::HasComplexSelection(){
    if(!HasSelection())
        return false;
    AnnotateSelection();

    return CountSelectedNodes(pTree) > 1;
}

void SmCursor::FinishEdit(SmNodeList* pLineList,
                          SmStructureNode* pParent,
                          int nParentIndex,
                          SmCaretPos PosAfterEdit,
                          SmNode* pStartLine) {
    //Store number of nodes in line for later
    int entries = pLineList->size();

    //Parse list of nodes to a tree
    SmNodeListParser parser;
    SmNode* pLine = parser.Parse(pLineList);
    delete pLineList;

    //Check if we're making the body of a subsup node bigger than one
    if(pParent->GetType() == NSUBSUP &&
       nParentIndex == 0 &&
       entries > 1) {
        //Wrap pLine in scalable round brackets
        SmToken aTok(TLEFT, '\0', "left", 0, 5);
        SmBraceNode *pBrace = new SmBraceNode(aTok);
        pBrace->SetScaleMode(SCALE_HEIGHT);
        SmNode *pLeft  = CreateBracket(RoundBrackets, true),
               *pRight = CreateBracket(RoundBrackets, false);
        SmBracebodyNode *pBody = new SmBracebodyNode(SmToken());
        pBody->SetSubNodes(pLine, NULL);
        pBrace->SetSubNodes(pLeft, pBody, pRight);
        pBrace->Prepare(pDocShell->GetFormat(), *pDocShell);
        pLine = pBrace;
        //TODO: Consider the following alternative behavior:
        //Consider the line: A + {B + C}^D lsub E
        //Here pLineList is B, + and C and pParent is a subsup node with
        //both RSUP and LSUB set. Imagine the user just inserted "B +" in
        //the body of the subsup node...
        //The most natural thing to do would be to make the line like this:
        //A + B lsub E + C ^ D
        //E.g. apply LSUB and LSUP to the first element in pLineList and RSUP
        //and RSUB to the last eleent in pLineList. But how should this act
        //for CSUP and CSUB ???
        //For this reason and because brackets was faster to implement, this solution
        //have been choosen. It might be worth working on the other solution later...
    }

    //Set pStartLine if NULL
    if(!pStartLine)
        pStartLine = pLine;

    //Insert it back into the parent
    pParent->SetSubNode(nParentIndex, pLine);

    //Rebuild graph of caret position
    anchor = NULL;
    position = NULL;
    BuildGraph();
    AnnotateSelection(); //Update selection annotation!

    //Set caret position
    if(!SetCaretPosition(PosAfterEdit, true))
        SetCaretPosition(SmCaretPos(pStartLine, 0), true);

    //End edit section
    EndEdit();
}

void SmCursor::BeginEdit(){
    if(nEditSections++ > 0) return;

    bIsEnabledSetModifiedSmDocShell = pDocShell->IsEnableSetModified();
    if( bIsEnabledSetModifiedSmDocShell )
        pDocShell->EnableSetModified( FALSE );
}

void SmCursor::EndEdit(){
    if(--nEditSections > 0) return;

    pDocShell->SetFormulaArranged(FALSE);
    //Okay, I don't know what this does... :)
    //It's used in SmDocShell::SetText and with places where everything is modified.
    //I think it does some magic, with sfx, but everything is totally undocumented so
    //it's kinda hard to tell...
    if ( bIsEnabledSetModifiedSmDocShell )
        pDocShell->EnableSetModified( bIsEnabledSetModifiedSmDocShell );
    //I think this notifies people around us that we've modified this document...
    pDocShell->SetModified(TRUE);
    //I think SmDocShell uses this value when it sends an update graphics event
    //Anyway comments elsewhere suggests it need to be updated...
    pDocShell->nModifyCount++;

    //TODO: Consider copying the update accessability code from SmDocShell::SetText in here...
    //This somehow updates the size of SmGraphicView if it is running in embedded mode
    if( pDocShell->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
        pDocShell->OnDocumentPrinterChanged(0);

    //Request a replaint...
    RequestRepaint();

    //Update the edit engine and text of the document
    String formula;
    SmNodeToTextVisitor(pTree, formula);
    //pTree->CreateTextFromNode(formula);
    pDocShell->aText = formula;
    pDocShell->GetEditEngine().SetText(formula);
}

void SmCursor::RequestRepaint(){
    SmViewShell *pViewSh = SmGetActiveView();
    if( pViewSh ) {
        if ( SFX_CREATE_MODE_EMBEDDED == pDocShell->GetCreateMode() )
            pDocShell->Repaint();
        else
            pViewSh->GetGraphicWindow().Invalidate();
    }
}

/////////////////////////////////////// SmNodeListParser ///////////////////////////////////////

SmNode* SmNodeListParser::Parse(SmNodeList* list, bool bDeleteErrorNodes){
    pList = list;
    if(bDeleteErrorNodes){
        //Delete error nodes
        SmNodeList::iterator it = pList->begin();
        while(it != pList->end()) {
            if((*it)->GetType() == NERROR){
                //Delete and erase
                delete *it;
                it = pList->erase(it);
            }else
                it++;
        }
    }
    SmNode* retval = Expression();
    pList = NULL;
    return retval;
}

SmNode* SmNodeListParser::Expression(){
    SmNodeArray NodeArray;
    //Accept as many relations as there is
    while(Terminal())
        NodeArray.push_back(Relation());

    //Create SmExpressionNode, I hope SmToken() will do :)
    SmStructureNode* pExpr = new SmExpressionNode(SmToken());
    pExpr->SetSubNodes(NodeArray);
    return pExpr;
}

SmNode* SmNodeListParser::Relation(){
    //Read a sum
    SmNode* pLeft = Sum();
    //While we have tokens and the next is a relation
    while(Terminal() && IsRelationOperator(Terminal()->GetToken())){
        //Take the operator
        SmNode* pOper = Take();
        //Find the right side of the relation
        SmNode* pRight = Sum();
        //Create new SmBinHorNode
        SmStructureNode* pNewNode = new SmBinHorNode(SmToken());
        pNewNode->SetSubNodes(pLeft, pOper, pRight);
        pLeft = pNewNode;
    }
    return pLeft;
}

SmNode* SmNodeListParser::Sum(){
    //Read a product
    SmNode* pLeft = Product();
    //While we have tokens and the next is a sum
    while(Terminal() && IsSumOperator(Terminal()->GetToken())){
        //Take the operator
        SmNode* pOper = Take();
        //Find the right side of the sum
        SmNode* pRight = Product();
        //Create new SmBinHorNode
        SmStructureNode* pNewNode = new SmBinHorNode(SmToken());
        pNewNode->SetSubNodes(pLeft, pOper, pRight);
        pLeft = pNewNode;
    }
    return pLeft;
}

SmNode* SmNodeListParser::Product(){
    //Read a Factor
    SmNode* pLeft = Factor();
    //While we have tokens and the next is a product
    while(Terminal() && IsProductOperator(Terminal()->GetToken())){
        //Take the operator
        SmNode* pOper = Take();
        //Find the right side of the operation
        SmNode* pRight = Factor();
        //Create new SmBinHorNode
        SmStructureNode* pNewNode = new SmBinHorNode(SmToken());
        pNewNode->SetSubNodes(pLeft, pOper, pRight);
        pLeft = pNewNode;
    }
    return pLeft;
}

SmNode* SmNodeListParser::Factor(){
    //Read unary operations
    if(!Terminal())
        return Error();
    //Take care of unary operators
    else if(IsUnaryOperator(Terminal()->GetToken()))
    {
        SmStructureNode *pUnary = new SmUnHorNode(SmToken());
        SmNode *pOper = Terminal(),
               *pArg;

        if(Next())
            pArg = Factor();
        else
            pArg = Error();

        pUnary->SetSubNodes(pOper, pArg);
        return pUnary;
    }
    return Postfix();
}

SmNode* SmNodeListParser::Postfix(){
    if(!Terminal())
        return Error();
    SmNode *pArg = NULL;
    if(IsPostfixOperator(Terminal()->GetToken()))
        pArg = Error();
    else if(IsOperator(Terminal()->GetToken()))
        return Error();
    else
        pArg = Take();
    while(Terminal() && IsPostfixOperator(Terminal()->GetToken())) {
        SmStructureNode *pUnary = new SmUnHorNode(SmToken());
        SmNode *pOper = Take();
        pUnary->SetSubNodes(pArg, pOper);
        pArg = pUnary;
    }
    return pArg;
}

SmNode* SmNodeListParser::Error(){
    return new SmErrorNode(PE_UNEXPECTED_TOKEN, SmToken());
}

BOOL SmNodeListParser::IsOperator(const SmToken &token) {
    return  IsRelationOperator(token) ||
            IsSumOperator(token) ||
            IsProductOperator(token) ||
            IsUnaryOperator(token) ||
            IsPostfixOperator(token);
}

BOOL SmNodeListParser::IsRelationOperator(const SmToken &token) {
    return token.nGroup & TGRELATION;
}

BOOL SmNodeListParser::IsSumOperator(const SmToken &token) {
    return token.nGroup & TGSUM;
}

BOOL SmNodeListParser::IsProductOperator(const SmToken &token) {
    return token.nGroup & TGPRODUCT &&
           token.eType != TWIDESLASH &&
           token.eType != TWIDEBACKSLASH &&
           token.eType != TUNDERBRACE &&
           token.eType != TOVERBRACE &&
           token.eType != TOVER;
}

BOOL SmNodeListParser::IsUnaryOperator(const SmToken &token) {
    return  token.nGroup & TGUNOPER &&
            (token.eType == TPLUS ||
             token.eType == TMINUS ||
             token.eType == TPLUSMINUS ||
             token.eType == TMINUSPLUS ||
             token.eType == TNEG ||
             token.eType == TUOPER);
}

BOOL SmNodeListParser::IsPostfixOperator(const SmToken &token) {
    return token.eType == TFACT;
}
