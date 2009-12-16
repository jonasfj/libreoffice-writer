/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: $
 * $Revision: $
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

package complex.writer;

import complexlib.ComplexTestCase;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.XComponentContext;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.beans.XPropertySet;
import com.sun.star.beans.XPropertySetInfo;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.Pair;
import com.sun.star.beans.StringPair;
import com.sun.star.container.XNamed;
import com.sun.star.container.XContentEnumerationAccess;
import com.sun.star.container.XEnumerationAccess;
import com.sun.star.container.XEnumeration;
import com.sun.star.container.NoSuchElementException;
import com.sun.star.frame.XStorable;
import com.sun.star.util.XCloseable;
import com.sun.star.text.XText;
import com.sun.star.text.XTextContent;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextRange;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XWordCursor;
import com.sun.star.text.XSentenceCursor;
import com.sun.star.text.XParagraphCursor;
import com.sun.star.text.XFootnote;
import com.sun.star.text.XTextField;
import com.sun.star.text.TextContentAnchorType;
import static com.sun.star.text.TextContentAnchorType.*;
import static com.sun.star.text.ControlCharacter.*;
import com.sun.star.rdf.XMetadatable;

import java.util.Map;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.util.Stack;


class TreeNodeEnum
{
    private Object[] m_Elements;
    private int m_Pos;
    TreeNodeEnum(Object[] Elements) { m_Elements = Elements; m_Pos = 0; }
    boolean hasNext() { return m_Pos < m_Elements.length; }
    TreeNode next() { return (TreeNode) m_Elements[m_Pos++]; }
}

/** base class for tree nodes. only instance: root of tree. */
class TreeNode
{
    private List<TreeNode> m_Children;
    String getType() { return "__ROOT__"; }
    boolean hasContent() { return false; }
    boolean isNesting() { return false; }
    TreeNode dup() { throw new RuntimeException("TreeNode.dup"); }
    TreeNodeEnum createEnumeration() {
        return new TreeNodeEnum(m_Children.toArray());
    }
    TreeNode() { m_Children = new ArrayList<TreeNode>(); }
    TreeNode appendChild(TreeNode child)
    { m_Children.add(child); return this; }
    public String toString() {
        return "<" + getType() + ">";
    }
}

abstract class MarkNode extends TreeNode
{
    boolean m_isPoint;
    boolean m_isStart = false;
    String m_Name;
    boolean isPoint() { return m_isPoint; }
    boolean isStart() { return m_isStart; }
    String getName() { return m_Name; }
    MarkNode(String name) { m_Name = name; m_isPoint = true; }
    boolean equals(MarkNode other) {
        return (other.m_Name.equals(m_Name)) && (other.m_isPoint == m_isPoint)
            && (other.m_isStart == m_isStart);
    }
    public String toString() {
        return super.toString() + "\tname: " + m_Name +
            "\tisPoint: " + m_isPoint + "\tisStart: " + m_isStart;
    }
}

class BookmarkNode extends MarkNode
{
    private StringPair m_XmlId;
    StringPair getXmlId() { return m_XmlId; }
    BookmarkNode dup() { return new BookmarkNode(getName(), getXmlId()); }
    BookmarkNode(String name) { this(name, new StringPair());  }
    BookmarkNode(String name, StringPair xmlid) {
        super(name); m_XmlId = xmlid;
    }
    String getType() { return "Bookmark"; }
    public boolean equals(Object other) {
        return (other instanceof BookmarkNode)
            && super.equals((MarkNode) other)
            && MetaNode.eq(((BookmarkNode) other).m_XmlId, m_XmlId);
    }
    public String toString() {
        return super.toString()
            + "\txmlid: " + m_XmlId.First + "#" + m_XmlId.Second;
    }
}

class BookmarkStartNode extends BookmarkNode
{
    BookmarkStartNode dup() { return new BookmarkStartNode(getName()); }
    BookmarkStartNode(String name) { this(name, new StringPair()); }
    BookmarkStartNode(String name, StringPair xmlid) {
        super(name, xmlid); m_isPoint = false; m_isStart = true;
    }
}

class BookmarkEndNode extends BookmarkNode
{
    BookmarkEndNode dup() { return new BookmarkEndNode(getName()); }
    BookmarkEndNode(String name) { this(name, new StringPair()); }
    BookmarkEndNode(String name, StringPair xmlid) {
        super(name, xmlid); m_isPoint = false; m_isStart = false;
    }
}

class ReferenceMarkNode extends MarkNode
{
    ReferenceMarkNode dup() { return new ReferenceMarkNode(getName()); }
    ReferenceMarkNode(String name) { super(name); }
    String getType() { return "ReferenceMark"; }
    public boolean equals(Object other) {
        return (other instanceof ReferenceMarkNode)
            && super.equals((MarkNode) other);
    }
}

class ReferenceMarkStartNode extends ReferenceMarkNode
{
    ReferenceMarkStartNode dup()
    { return new ReferenceMarkStartNode(getName()); }
    ReferenceMarkStartNode(String name) {
        super(name); m_isPoint = false; m_isStart = true;
    }
}

class ReferenceMarkEndNode extends ReferenceMarkNode
{
    ReferenceMarkEndNode dup()
    { return new ReferenceMarkEndNode(getName()); }
    ReferenceMarkEndNode(String name) {
        super(name); m_isPoint = false; m_isStart = false;
    }
}

class DocumentIndexMarkNode extends MarkNode
{
    DocumentIndexMarkNode dup()
    { return new DocumentIndexMarkNode(getName()); }
    DocumentIndexMarkNode(String name) { super(name); }
    String getType() { return "DocumentIndexMark"; }
    public boolean equals(Object other) {
        return (other instanceof DocumentIndexMarkNode)
            && super.equals((MarkNode) other);
    }
}

class DocumentIndexMarkStartNode extends DocumentIndexMarkNode
{
    DocumentIndexMarkStartNode dup()
    { return new DocumentIndexMarkStartNode(getName()); }
    DocumentIndexMarkStartNode(String name) {
        super(name); m_isPoint = false; m_isStart = true;
    }
}

class DocumentIndexMarkEndNode extends DocumentIndexMarkNode
{
    DocumentIndexMarkEndNode dup()
    { return new DocumentIndexMarkEndNode(getName()); }
    DocumentIndexMarkEndNode(String name) {
        super(name); m_isPoint = false; m_isStart = false;
    }
}

abstract class ContentNode extends TreeNode
{
    private String m_Content;
    String getContent() { return m_Content; }
    boolean hasContent() { return true; }
    ContentNode(String content) {
        m_Content = content;
    }
    TreeNode appendChild(TreeNode t) {
        throw new RuntimeException("ContentNode.appendChild");
    }
    public String toString() {
        return super.toString() + "\tcontent: " + m_Content;
    }
    boolean equals(ContentNode other) {
        return (other.m_Content.equals(m_Content));
    }
}

class TextNode extends ContentNode
{
    TextNode dup() { return new TextNode(getContent()); }
    TextNode(String content) { super(content); }
    String getType() { return "Text"; }
    public boolean equals(Object other) {
        return (other instanceof TextNode) && super.equals((ContentNode) other);
    }
}

class TextFieldNode extends ContentNode
{
    TextFieldNode dup() { return new TextFieldNode(getContent()); }
    TextFieldNode(String content) { super(content); }
    String getType() { return "TextField"; }
    public boolean equals(Object other) {
        return (other instanceof TextFieldNode)
            && super.equals((ContentNode) other);
    }
}

class FrameNode extends TreeNode
{
    private String m_Name;
    private TextContentAnchorType m_Anchor;
    String getName() { return m_Name; }
    TextContentAnchorType getAnchor() { return m_Anchor; }
    FrameNode dup() { return new FrameNode(getName(), getAnchor()); }
    FrameNode(String name, TextContentAnchorType anchor) {
        m_Name = name; m_Anchor = anchor;
    }
    String getType() { return "Frame"; }
    public boolean equals(Object other) {
        return (other instanceof FrameNode)
            && (((FrameNode) other).m_Name.equals(m_Name))
            && (((FrameNode) other).m_Anchor == m_Anchor);
    }
    public String toString() {
        return super.toString()
            + "\tname: " + m_Name + "\tanchor: " + toString(m_Anchor);
    }
    static String toString(TextContentAnchorType anchor) {
        switch (anchor.getValue()) {
            case AS_CHARACTER_value: return "AS_CHARACTER";
            case AT_CHARACTER_value: return "AT_CHARACTER";
            case AT_PARAGRAPH_value: return "AT_PARAGRAPH";
            case AT_PAGE_value:      return "AT_PAGE";
            case AT_FRAME_value:     return "AT_FRAME";
            default: throw new RuntimeException("unknown anchor");
        }
    }
}

class FootnoteNode extends TreeNode
{
    private String m_Label;
    String getLabel() { return m_Label; }
    FootnoteNode dup() { return new FootnoteNode(getLabel()); }
    FootnoteNode(String label) { m_Label = label; }
    String getType() { return "Footnote"; }
    public boolean equals(Object other) {
        return (other instanceof FootnoteNode)
            && (((FootnoteNode) other).m_Label.equals(m_Label));
    }
    public String toString() {
        return super.toString() + "\tlabel: " + m_Label;
    }
}

class ControlCharacterNode extends TreeNode
{
    private short m_Char;
    short getChar() { return m_Char; }
    ControlCharacterNode dup() { return new ControlCharacterNode(getChar()); }
    ControlCharacterNode(short c) { m_Char = c; }
    String getType() { return "ControlCharacter"; }
    public boolean equals(Object other) {
        return (other instanceof ControlCharacterNode)
            && (((ControlCharacterNode) other).m_Char == m_Char);
    }
    public String toString() {
        return super.toString() + "\tchar: " + m_Char;
    }
}

class SoftPageBreakNode extends TreeNode
{
    String getType() { return "SoftPageBreak"; }
    public boolean equals(Object other) {
        return (other instanceof SoftPageBreakNode);
    }
}

class HyperlinkNode extends TreeNode
{
    private String m_URL;
    String getURL() { return m_URL; }
    HyperlinkNode dup() { return new HyperlinkNode(getURL()); }
    HyperlinkNode(String url) {
        if (url.length() == 0) throw new RuntimeException("HyperlinkNode");
        m_URL = url;
    }
    String getType() { return "Hyperlink"; }
    boolean isNesting() { return true; }
    public boolean equals(Object other) {
        return (other instanceof HyperlinkNode)
            && (((HyperlinkNode) other).m_URL.equals(m_URL));
    }
    public String toString() {
        return super.toString() + "\turl: " + m_URL;
    }
}

class RubyNode extends TreeNode
{
    private String m_Ruby;
    String getRubyText() { return m_Ruby; }
    RubyNode dup() { return new RubyNode(getRubyText()); }
    RubyNode(String ruby) {
        if (ruby.length() == 0) throw new RuntimeException("RubyNode");
        m_Ruby = ruby;
    }
    String getType() { return "Ruby"; }
    boolean isNesting() { return true; }
    public boolean equals(Object other) {
        return (other instanceof RubyNode)
            && (((RubyNode) other).m_Ruby.equals(m_Ruby));
    }
    public String toString() {
        return super.toString() + "\trubytext: " + m_Ruby;
    }
}

class MetaNode extends TreeNode
{
    private StringPair m_XmlId;
    StringPair getXmlId() { return m_XmlId; }
    MetaNode dup() { return new MetaNode(getXmlId()); }
    MetaNode (StringPair xmlid) { m_XmlId = xmlid; }
    String getType() { return "InContentMetadata"; }
    boolean isNesting() { return true; }
    public boolean equals(Object other) {
        return (other instanceof MetaNode)
            && eq(((MetaNode) other).m_XmlId, m_XmlId);
    }
    static boolean eq(StringPair left, StringPair right)
    {
        return left.First.equals(right.First)
            && left.Second.equals(right.Second);
    }
    public String toString() {
        return super.toString()
            + "\txmlid: " + m_XmlId.First + "#" + m_XmlId.Second;
    }
}

class MetaFieldNode extends MetaNode
{
    MetaFieldNode dup() { return new MetaFieldNode(getXmlId()); }
    MetaFieldNode (StringPair xmlid) { super(xmlid); }
    String getType() { return "MetadataField"; }
}

class Range
{
    private short m_Start;
    private short m_End;
    private TreeNode m_Node;
    short getStart()    { return m_Start; }
    short getEnd()      { return m_End  ; }
    short getExtent()   { return (short) (m_End - m_Start); }
    TreeNode getNode()  { return m_Node; }
    Range(int start, int end, TreeNode node)
    { m_Start = (short) start; m_End = (short) end; m_Node = node; }
}

//----------------------------------------------------------------------

/* this is where we nail the pudding to the wall */
class FuzzyTester
{
    private long m_DiffContent = 0;
    private long m_DiffMissing = 0;
    private long m_DiffNesting = 0;
    private long m_DiffSpuriousEmptyText = 0;
    private long m_DiffSequence = 0; // ignored?
    private Stack<Pair<TreeNode, TreeNodeEnum>> m_StackExpected;
    private Stack<Pair<TreeNode, TreeNodeEnum>> m_StackActual;
    private List<TreeNode> m_BufferExpected;
    private List<TreeNode> m_BufferActual;
    private share.LogWriter m_Log;

    FuzzyTester(share.LogWriter log) {
        m_Log = log;
        m_BufferExpected = new ArrayList<TreeNode>();
        m_BufferActual = new ArrayList<TreeNode>();
        m_StackExpected = new Stack<Pair<TreeNode, TreeNodeEnum>>();
        m_StackActual = new Stack<Pair<TreeNode, TreeNodeEnum>>();
    }

    /** idea: traverse both trees, enumerate nodes, stopping at content nodes.
        then compare buffers. */
    boolean doTest(TreeNode expected, TreeNode actual)
    {
        if (!expected.getType().equals("__ROOT__"))
            throw new RuntimeException("doTest: expected: root");
        if (!actual.getType().equals("__ROOT__"))
            throw new RuntimeException("doTest: actual: root");
        m_StackExpected.push(new Pair(expected, expected.createEnumeration()));
        m_StackActual.push(new Pair(actual, actual.createEnumeration()));
        do {
            traverse(m_StackExpected, m_BufferExpected);
            traverse(m_StackActual, m_BufferActual);
        //??? root?
            testBuffer();
        } while (!m_StackExpected.empty() || !m_StackActual.empty());
        if (m_DiffSequence != 0) {
            m_Log.println("warning: " +  m_DiffSequence
                    + " differences in sequence");
        }
        if (m_DiffSpuriousEmptyText != 0) {
            m_Log.println("warning: " +  m_DiffSpuriousEmptyText
                    + " spurious empty text nodes");
        }
        if (m_DiffNesting != 0) {
            m_Log.println("WARNING: " +  m_DiffNesting
                    + " differences in nesting");
        }
        return (m_DiffContent == 0) && (m_DiffMissing == 0);
    }

    private void traverse(Stack<Pair<TreeNode, TreeNodeEnum>> stack,
                  List<TreeNode> buffer)
    {
        while (!stack.empty()) {
            TreeNodeEnum topEnum = stack.peek().Second;
            if (topEnum.hasNext()) {
                TreeNode node = topEnum.next();
                buffer.add(node);
                TreeNodeEnum nodeEnum = node.createEnumeration();
                if (nodeEnum.hasNext()) {
                    stack.push(new Pair(node, nodeEnum));
                }
                if (node.hasContent()) {
                    if (!((node instanceof TextNode) // spurious empty text?
                        && ((TextNode) node).getContent().length() == 0)) {
                            return; // break here
                    }
                }
            } else {
                buffer.add(stack.peek().First);
                stack.pop();
            }
        }
    }

    private void testTerminatingNode()
    {
        int lenExpected = m_BufferExpected.size();
        int lenActual   = m_BufferActual.size();
        if (lenExpected == 0 || lenActual == 0)
            return;
        TreeNode expected = m_BufferExpected.get(lenExpected - 1);
        TreeNode actual = m_BufferActual.get(lenActual - 1);

        boolean eRoot = expected.getType().equals("__ROOT__");
        boolean aRoot = actual.getType().equals("__ROOT__");
        if (eRoot || aRoot) {
            if (!(eRoot && aRoot)) {
                if (aRoot) printMissing(expected);
                else       printUnexpected(actual);
                m_DiffMissing++;
            }
            return;
        }

        testContentNode((ContentNode) expected, (ContentNode) actual);

        m_BufferExpected.set(lenExpected - 1, null);
        m_BufferActual.set(lenActual - 1, null);
    }

    private void testContentNode(ContentNode expected, ContentNode actual)
    {
        String contentExpected = expected.getContent();
        String contentActual = actual.getContent();
        if (!expected.equals(actual)) {
            printDiff("text content differs", contentExpected, contentActual);
            m_DiffContent++;
        }
    }

    private void testBuffer()
    {
        int lenExpected = m_BufferExpected.size();
        int lenActual   = m_BufferActual.size();
        for (int i = 0; i < lenExpected - 1; i++ )
        {
            TreeNode node = m_BufferExpected.get(i);
            int j = m_BufferActual.indexOf(node);
            if (j >= 0) {
                TreeNode other = m_BufferActual.get(j);
                if (j != i)
                {
                    //FIXME how bad is this?
                    printDiff("position differs",
                            String.valueOf(i), String.valueOf(j));
                    // a hacky hack
                    int min = Math.min(i,j);
                    int max = Math.max(Math.min(lenActual - 1, i),j);
                    for (int k = min; k != max; k ++) {
                        TreeNode tmp = m_BufferActual.get(k);
                        if (tmp != null && tmp.isNesting()) {
                            printNesting(node, tmp);
                            m_DiffNesting++;
                        }
                    }
                    m_DiffSequence++;
                }
                m_BufferActual.set(j, null);
            } else {
//m_Log.println("testBuffer:");
                printMissing(node);
                m_DiffMissing++;
            }
        }
        for (int j = 0; j < lenActual - 1; j++)
        {
            TreeNode node = m_BufferActual.get(j);
            if (node != null)
            {
//m_Log.println("testBuffer:");
                printUnexpected(node);
                if ((node instanceof TextNode) &&
                        ((TextNode) node).getContent().length() == 0) {
                    m_DiffSpuriousEmptyText++;
                } else {
                    m_DiffMissing++;
                }
            }
        }
        testTerminatingNode();
        m_BufferExpected.clear();
        m_BufferActual.clear();
    }

    void printDiff(String prefix, String expected, String actual)
    {
        m_Log.println(prefix +
                ":\texpected: " + expected + "\tactual: " + actual);
    }

    void printNesting(TreeNode node, TreeNode nesting)
    {
        m_Log.println("node: " + node.toString()
                + " possibly moved across nesting " + nesting.toString());
    }

    void printMissing(TreeNode node)
    {
        m_Log.println("   missing node: " + node.toString());

    }

    void printUnexpected(TreeNode node)
    {
        m_Log.println("unexpected node: " + node.toString());

    }
}


//----------------------------------------------------------------------

class EnumConverter
{
    private Stack<TreeNode> m_Stack;
    TextPortionEnumerationTest m_T;

    EnumConverter(TextPortionEnumerationTest err) {
        m_Stack = new Stack<TreeNode>(); m_T = err;
    }

    TreeNode convert(XEnumeration xEnum) throws Exception
    {
        TreeNode root = new TreeNode();
        m_Stack.push(root);
        TreeNode ret = convertChildren(xEnum);
        m_T.assure("EnumConverter.convert: stack", m_Stack.empty());
        return ret;
    }

    TreeNode convertChildren(XEnumeration xEnum) throws Exception
    {
        while (xEnum.hasMoreElements()) {
            TreeNode node;
            Object xElement = xEnum.nextElement();
            XTextRange xPortion = (XTextRange)
                UnoRuntime.queryInterface(XTextRange.class, xElement);
            XPropertySet xPropSet = (XPropertySet)
                UnoRuntime.queryInterface(XPropertySet.class, xPortion);
            String type = (String) xPropSet.getPropertyValue("TextPortionType");
            if (type.equals("Text")) {
                String text = xPortion.getString();
                node = new TextNode(text);
                String url = (String) xPropSet.getPropertyValue("HyperLinkURL");
                if (url.length() > 0) {
                    TreeNode temp = node;
                    node = new HyperlinkNode(url);
                    node.appendChild(temp);
                }
            } else if (type.equals("TextField")) {
                Object xField = xPropSet.getPropertyValue("TextField");
                XServiceInfo xService = (XServiceInfo)
                    UnoRuntime.queryInterface(XServiceInfo.class, xField);
                if (xService.supportsService(
                        "com.sun.star.text.textfield.MetadataField"))
                {
                    XMetadatable xMeta = (XMetadatable)
                        UnoRuntime.queryInterface(XMetadatable.class, xField);
                    StringPair xmlid = xMeta.getMetadataReference();
                    node = new MetaFieldNode(xmlid);
                    m_Stack.push(node);
                    XEnumerationAccess xEA = (XEnumerationAccess)
                        UnoRuntime.queryInterface(XEnumerationAccess.class,
                        xMeta);
                    XEnumeration xEnumChildren = xEA.createEnumeration();
                    TreeNode node2 = convertChildren(xEnumChildren);
                    m_T.assure("stack error: meta-field", node == node2);
                } else {
                    XPropertySet xFieldPropSet = (XPropertySet)
                        UnoRuntime.queryInterface(XPropertySet.class, xField);
                    String content = (String)
                        xFieldPropSet.getPropertyValue("Content");
                    boolean isFixed = (Boolean)
                        xFieldPropSet.getPropertyValue("IsFixed");
                    m_T.assure("field not fixed?", isFixed);
                    node = new TextFieldNode(content);
                }
            } else if (type.equals("Footnote")) {
                Object xNote = xPropSet.getPropertyValue("Footnote");
                XFootnote xFootnote = (XFootnote)
                    UnoRuntime.queryInterface(XFootnote.class, xNote);
                String label = xFootnote.getLabel();
                node = new FootnoteNode(label);
            } else if (type.equals("Frame")) {
                XContentEnumerationAccess xCEA = (XContentEnumerationAccess)
                    UnoRuntime.queryInterface(XContentEnumerationAccess.class,
                        xPortion);
                XEnumeration xContentEnum = xCEA.createContentEnumeration("");
                while (xContentEnum.hasMoreElements()) {
                    Object xFrame = xContentEnum.nextElement();
                    XPropertySet xFramePropSet = (XPropertySet)
                        UnoRuntime.queryInterface(XPropertySet.class, xFrame);
                    TextContentAnchorType anchor = (TextContentAnchorType)
                        xFramePropSet.getPropertyValue("AnchorType");
                    XNamed xNamed = (XNamed)
                        UnoRuntime.queryInterface(XNamed.class, xFrame);
                    String name = xNamed.getName();
                    node = new FrameNode(name, anchor);
                    m_Stack.peek().appendChild(node);
                }
                continue;
            } else if (type.equals("ControlCharacter")) {
                short c = (Short)
                    xPropSet.getPropertyValue("ControlCharacter");
                node = new ControlCharacterNode(c);
            } else if (type.equals("Bookmark")) {
                Object xMark = xPropSet.getPropertyValue("Bookmark");
                XNamed xNamed = (XNamed)
                    UnoRuntime.queryInterface(XNamed.class, xMark);
                String name = xNamed.getName();
                XMetadatable xMetadatable = (XMetadatable)
                    UnoRuntime.queryInterface(XMetadatable.class, xMark);
                StringPair xmlid = xMetadatable.getMetadataReference();
                boolean isCollapsed = (Boolean)
                    xPropSet.getPropertyValue("IsCollapsed");
                if (isCollapsed) {
                    node = new BookmarkNode(name, xmlid);
                } else {
                    boolean isStart = (Boolean)
                        xPropSet.getPropertyValue("IsStart");
                    if (isStart) {
                        node = new BookmarkStartNode(name, xmlid);
                    } else {
                        node = new BookmarkEndNode(name, xmlid);
                    }
                }
            } else if (type.equals("ReferenceMark")) {
                Object xMark = xPropSet.getPropertyValue("ReferenceMark");
                XNamed xNamed = (XNamed)
                    UnoRuntime.queryInterface(XNamed.class, xMark);
                String name = xNamed.getName();
                boolean isCollapsed = (Boolean)
                    xPropSet.getPropertyValue("IsCollapsed");
                if (isCollapsed) {
                    node = new ReferenceMarkNode(name);
                } else {
                    boolean isStart = (Boolean)
                        xPropSet.getPropertyValue("IsStart");
                    if (isStart) {
                        node = new ReferenceMarkStartNode(name);
                    } else {
                        node = new ReferenceMarkEndNode(name);
                    }
                }
            } else if (type.equals("DocumentIndexMark")) {
                Object xMark = xPropSet.getPropertyValue("DocumentIndexMark");
                XPropertySet xMarkSet = (XPropertySet)
                    UnoRuntime.queryInterface(XPropertySet.class, xMark);
                String name = (String) xMarkSet.getPropertyValue("PrimaryKey");
                boolean isCollapsed = (Boolean)
                    xPropSet.getPropertyValue("IsCollapsed");
                if (isCollapsed) {
                    node = new DocumentIndexMarkNode(name);
                } else {
                    boolean isStart = (Boolean)
                        xPropSet.getPropertyValue("IsStart");
                    if (isStart) {
                        node = new DocumentIndexMarkStartNode(name);
                    } else {
                        node = new DocumentIndexMarkEndNode(name);
                    }
                }
            } else if (type.equals("SoftPageBreak")) {
                node = new SoftPageBreakNode();
            } else if (type.equals("Ruby")) {
                boolean isStart = (Boolean)
                    xPropSet.getPropertyValue("IsStart");
                if (isStart) {
                    // ARRGH!!! stupid api...
                    // the text is ONLY at the start!
                    String ruby = (String)
                        xPropSet.getPropertyValue("RubyText");
                    node = new RubyNode(ruby);
                    m_Stack.push(node);
                    continue;
                } else {
                    node = m_Stack.pop();
                    m_T.assure("stack error: Ruby expected; is: " +
                            node.toString(), node instanceof RubyNode);
//                    m_T.assure("stack error: ruby",
//                            ruby.equals(((RubyNode)node).getRubyText()));
                }
            } else if (type.equals("InContentMetadata")) {
                Object xMeta = xPropSet.getPropertyValue("InContentMetadata");
                XMetadatable xMetadatable = (XMetadatable)
                    UnoRuntime.queryInterface(XMetadatable.class, xMeta);
                StringPair xmlid = xMetadatable.getMetadataReference();
                node = new MetaNode(xmlid);
                m_Stack.push(node);
                XEnumerationAccess xEA = (XEnumerationAccess)
                    UnoRuntime.queryInterface(XEnumerationAccess.class, xMeta);
                XEnumeration xEnumChildren = xEA.createEnumeration();
                TreeNode node2 = convertChildren(xEnumChildren);
                m_T.assure("stack error: meta", node == node2);
//            } else if (type.equals("MetadataField")) {
//                    Object xMeta = xPropSet.getPropertyValue("MetadataField");
            } else {
                throw new RuntimeException("unexpected type: " + type);
            }
            m_Stack.peek().appendChild(node);
        }
        TreeNode ret = m_Stack.pop();
        return ret;
    }
}


//----------------------------------------------------------------------

abstract class Inserter
{
    private XMultiServiceFactory m_xDocFactory;
    XText m_xText;
    XParagraphCursor m_xCursor;

    Inserter(XTextDocument xDoc)
    {
        m_xDocFactory = (XMultiServiceFactory)
            UnoRuntime.queryInterface(XMultiServiceFactory.class, xDoc);
        m_xText = xDoc.getText();
        XTextCursor xCursor = m_xText.createTextCursor();
        m_xCursor = (XParagraphCursor)
            UnoRuntime.queryInterface(XParagraphCursor.class, xCursor);
    }

    void initParagraph() throws Exception
    {
        m_xCursor.gotoStartOfParagraph(false);
        m_xText.insertControlCharacter(m_xCursor, PARAGRAPH_BREAK, false);
        // we split the first (empty) paragraph, and then insert into the
        // second (empty) paragraph; this ensures first is always empty!
    }

    void insertControlCharacter(XTextCursor xCursor, short cchar)
        throws Exception
    {
        m_xText.insertControlCharacter(xCursor, cchar, false);
    }

    void insertText(XTextCursor xCursor, String text)
    {
        xCursor.setString(text);
    }

    void insertTextField(XTextCursor xCursor, String content) throws Exception
    {
        XTextContent xContent = makeTextField(content);
        xContent.attach(xCursor);
    }

    XTextContent makeTextField(String content)
        throws Exception
    {
        Object xField =
            m_xDocFactory.createInstance("com.sun.star.text.textfield.Author");
        XTextContent xContent = (XTextContent)
            UnoRuntime.queryInterface(XTextContent.class, xField);
        XPropertySet xPropSet = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, xField);
        xPropSet.setPropertyValue("IsFixed", true);
        xPropSet.setPropertyValue("FullName", false);
        xPropSet.setPropertyValue("Content", content);
        return xContent;
    }

    void insertFrame(XTextRange xCursor, String name,
            TextContentAnchorType anchor) throws Exception
    {
        XTextContent xContent = makeFrame(name, anchor);
        xContent.attach(xCursor);
    }

    XTextContent makeFrame(String name, TextContentAnchorType anchor)
        throws Exception
    {
        Object xFrame =
            m_xDocFactory.createInstance("com.sun.star.text.TextFrame");
        XTextContent xContent = (XTextContent)
            UnoRuntime.queryInterface(XTextContent.class, xFrame);
        XPropertySet xPropSet = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, xFrame);
        xPropSet.setPropertyValue("AnchorType", anchor);
        XNamed xNamed = (XNamed)
            UnoRuntime.queryInterface(XNamed.class, xContent);
        xNamed.setName(name);
        return xContent;
    }

    void insertFootnote(XTextCursor xCursor, String label) throws Exception
    {
        XTextContent xContent = makeFootnote(label);
        xContent.attach(xCursor);
    }

    XTextContent makeFootnote(String label) throws Exception
    {
        Object xNote =
            m_xDocFactory.createInstance("com.sun.star.text.Footnote");
        XTextContent xContent = (XTextContent)
            UnoRuntime.queryInterface(XTextContent.class, xNote);
        XFootnote xFootnote = (XFootnote)
            UnoRuntime.queryInterface(XFootnote.class, xNote);
        xFootnote.setLabel(label);
        return xContent;
    }

    void insertBookmark(XTextCursor xCursor, String name, StringPair xmlid)
        throws Exception
    {
        XTextContent xContent = makeBookmark(name);
        xContent.attach(xCursor);
        if (!xmlid.First.equals(""))
        {
            XMetadatable xMetadatable = (XMetadatable)
                UnoRuntime.queryInterface(XMetadatable.class, xContent);
            xMetadatable.setMetadataReference(xmlid);
        }
    }

    XTextContent makeBookmark(String name) throws Exception
    {
        Object xBookmark =
            m_xDocFactory.createInstance("com.sun.star.text.Bookmark");
        XTextContent xContent = (XTextContent)
            UnoRuntime.queryInterface(XTextContent.class, xBookmark);
        XNamed xNamed = (XNamed)
            UnoRuntime.queryInterface(XNamed.class, xContent);
        xNamed.setName(name);
        return xContent;
    }

    void insertReferenceMark(XTextCursor xCursor, String name) throws Exception
    {
        XTextContent xContent = makeReferenceMark(name);
        xContent.attach(xCursor);
    }

    XTextContent makeReferenceMark(String name) throws Exception
    {
        Object xMark =
            m_xDocFactory.createInstance("com.sun.star.text.ReferenceMark");
        XTextContent xContent = (XTextContent)
            UnoRuntime.queryInterface(XTextContent.class, xMark);
        XNamed xNamed = (XNamed)
            UnoRuntime.queryInterface(XNamed.class, xContent);
        xNamed.setName(name);
        return xContent;
    }

    void insertDocumentIndexMark(XTextCursor xCursor, String key)
        throws Exception
    {
        XTextContent xContent = makeDocumentIndexMark(key);
        xContent.attach(xCursor);
    }

    XTextContent makeDocumentIndexMark(String key) throws Exception
    {
        Object xMark =
            m_xDocFactory.createInstance("com.sun.star.text.DocumentIndexMark");
        XTextContent xContent = (XTextContent)
            UnoRuntime.queryInterface(XTextContent.class, xMark);
        XPropertySet xPropSet = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, xMark);
        xPropSet.setPropertyValue("PrimaryKey", key);
        return xContent;
    }

    void insertHyperlink(XTextCursor xCursor, String url) throws Exception
    {
        XPropertySet xPropSet = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, xCursor);
        xPropSet.setPropertyValue("HyperLinkURL", url);
    }

    void insertRuby(XTextCursor xCursor, String rubytext) throws Exception
    {
        XPropertySet xPropSet = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, xCursor);
        xPropSet.setPropertyValue("RubyText", rubytext);
    }

    void insertMeta(XTextCursor xCursor, StringPair xmlid)
        throws Exception
    {
        XTextContent xContent = makeMeta();
        xContent.attach(xCursor);
        XMetadatable xMetadatable = (XMetadatable)
            UnoRuntime.queryInterface(XMetadatable.class, xContent);
        xMetadatable.setMetadataReference(xmlid);
    }

    XTextContent makeMeta() throws Exception
    {
        Object xMeta = m_xDocFactory.createInstance(
                "com.sun.star.text.InContentMetadata");
        XTextContent xContent = (XTextContent)
            UnoRuntime.queryInterface(XTextContent.class, xMeta);
        return xContent;
    }

    void insertMetaField(XTextCursor xCursor, StringPair xmlid)
        throws Exception
    {
        XTextContent xContent = makeMetaField();
        xContent.attach(xCursor);
        XMetadatable xMetadatable = (XMetadatable)
            UnoRuntime.queryInterface(XMetadatable.class, xContent);
        xMetadatable.setMetadataReference(xmlid);
    }

    XTextField makeMetaField() throws Exception
    {
        Object xMeta = m_xDocFactory.createInstance(
                "com.sun.star.text.textfield.MetadataField");
        XTextField xContent = (XTextField)
            UnoRuntime.queryInterface(XTextField.class, xMeta);
        return xContent;
    }

}

class TreeInserter extends Inserter
{
    private Map<String, XTextRange> m_BookmarkStarts;
    private Map<String, XTextRange> m_ReferenceMarkStarts;
    private Map<String, XTextRange> m_DocumentIndexMarkStarts;
    private List<Pair<XTextRange, FrameNode>> m_FrameHints;

    TreeInserter(XTextDocument xDoc)
    {
        super(xDoc);
        m_BookmarkStarts = new HashMap<String, XTextRange>();
        m_ReferenceMarkStarts = new HashMap<String, XTextRange>();
        m_DocumentIndexMarkStarts = new HashMap<String, XTextRange>();
        m_FrameHints = new ArrayList<Pair<XTextRange, FrameNode>>();
    }

    void insertTree(TreeNode tree) throws Exception
    {
        if (!tree.getType().equals("__ROOT__"))
            throw new RuntimeException("insertTree: test error: no root");
        initParagraph();
        insertChildren(tree.createEnumeration());
        for (int i = 0; i < m_FrameHints.size(); ++i) {
            Pair<XTextRange, FrameNode> p = m_FrameHints.get(i);
            insertFrame(p.First, p.Second.getName(), p.Second.getAnchor());
        }
    }

    void insertChildren(TreeNodeEnum children) throws Exception
    {
        while (children.hasNext()) {
            m_xCursor.gotoEndOfParagraph(false);
            TreeNode node = children.next();
            String type = node.getType();
            if (type.equals("Bookmark")) {
                BookmarkNode bkmk = (BookmarkNode) node;
                String name = bkmk.getName();
                StringPair id = bkmk.getXmlId();
                if (bkmk.isPoint()) {
                    insertBookmark(m_xCursor, name, id);
                } else if (bkmk.isStart()) {
                    m_BookmarkStarts.put(name, m_xCursor.getStart());
                } else {
                    XTextRange xRange = m_BookmarkStarts.get(name);
                    XParagraphCursor xCursor = mkCursor(xRange);
                    insertBookmark(xCursor, name, id);
                }
            } else if (type.equals("ReferenceMark")) {
                ReferenceMarkNode mark = (ReferenceMarkNode) node;
                String name = mark.getName();
                if (mark.isPoint()) {
                    insertReferenceMark(m_xCursor, name);
                } else if (mark.isStart()) {
                    m_ReferenceMarkStarts.put(name, m_xCursor.getStart());
                } else {
                    XTextRange xRange = m_ReferenceMarkStarts.get(name);
                    XParagraphCursor xCursor = mkCursor(xRange);
                    insertReferenceMark(xCursor, name);
                }
            } else if (type.equals("DocumentIndexMark")) {
                DocumentIndexMarkNode mark = (DocumentIndexMarkNode) node;
                String name = mark.getName();
                if (mark.isPoint()) {
                    insertDocumentIndexMark(m_xCursor, name);
                } else if (mark.isStart()) {
                    m_DocumentIndexMarkStarts.put(name, m_xCursor.getStart());
                } else {
                    XTextRange xRange = m_DocumentIndexMarkStarts.get(name);
                    XParagraphCursor xCursor = mkCursor(xRange);
                    insertDocumentIndexMark(xCursor, name);
                }
            } else if (type.equals("Hyperlink")) {
                HyperlinkNode href = (HyperlinkNode) node;
                XTextRange xRange = m_xCursor.getStart();
                insertChildren(href.createEnumeration());
                XParagraphCursor xCursor = mkCursor(xRange);
                insertHyperlink(xCursor, href.getURL());
            } else if (type.equals("Ruby")) {
                RubyNode ruby = (RubyNode) node;
                XTextRange xRange = m_xCursor.getStart();
                insertChildren(ruby.createEnumeration());
                XParagraphCursor xCursor = mkCursor(xRange);
                insertRuby(xCursor, ruby.getRubyText());
            } else if (type.equals("InContentMetadata")) {
                MetaNode meta = (MetaNode) node;
                XTextRange xRange = m_xCursor.getStart();
                insertChildren(meta.createEnumeration());
                XParagraphCursor xCursor = mkCursor(xRange);
                insertMeta(xCursor, meta.getXmlId());
            } else if (type.equals("MetadataField")) {
                MetaFieldNode meta = (MetaFieldNode) node;
                XTextRange xRange = m_xCursor.getStart();
                insertChildren(meta.createEnumeration());
                XParagraphCursor xCursor = mkCursor(xRange);
                insertMetaField(xCursor, meta.getXmlId());
            } else if (type.equals("Text")) {
                TextNode text = (TextNode) node;
                insertText(m_xCursor, text.getContent());
            } else if (type.equals("TextField")) {
                TextFieldNode field = (TextFieldNode) node;
                insertTextField(m_xCursor, field.getContent());
            } else if (type.equals("Footnote")) {
                FootnoteNode note = (FootnoteNode) node;
                insertFootnote(m_xCursor, note.getLabel());
            } else if (type.equals("Frame")) {
                FrameNode frame = (FrameNode) node;
                if (frame.getAnchor() == AT_CHARACTER) {
                    m_FrameHints.add( new Pair(m_xCursor.getStart(), frame) );
                } else {
                    insertFrame(m_xCursor, frame.getName(), frame.getAnchor());
                }
            } else if (type.equals("ControlCharacter")) {
                ControlCharacterNode cchar = (ControlCharacterNode) node;
                insertControlCharacter(m_xCursor, cchar.getChar());
            } else if (type.equals("SoftPageBreak")) {
                SoftPageBreakNode spbk = (SoftPageBreakNode) node;
                throw new RuntimeException("sorry, cannot test SoftPageBreak");
            } else {
                throw new RuntimeException("unexpected type: " + type);
            }
        }
    }

    XParagraphCursor mkCursor(XTextRange xRange)
    {
        XTextCursor xCursor = m_xText.createTextCursorByRange(xRange);
        XParagraphCursor xParaCursor = (XParagraphCursor)
            UnoRuntime.queryInterface(XParagraphCursor.class, xCursor);
        xParaCursor.gotoEndOfParagraph(true);
        return xParaCursor;
    }
}


// FIXME: this does not account for inserted dummy characters!
class RangeInserter extends Inserter
{
    RangeInserter(XTextDocument xDoc) throws Exception
    { super(xDoc); initParagraph(); }

    /*
    void insertText(int pos, String text)
    {
        m_xCursor.gotoStartOfParagraph(false);
        m_xCursor.goRight((short) pos, false);
        insertText(m_xCursor, text);
    }
    */

    void insertRange(Range range) throws Exception
    {
        m_xCursor.gotoStartOfParagraph(false);
        m_xCursor.goRight(range.getStart(), false);
        m_xCursor.goRight(range.getExtent(), true);
        insertNode(m_xCursor, range.getNode());
    }

    void insertNode(XParagraphCursor xCursor, TreeNode node) throws Exception
    {
        String type = node.getType();
        if (type.equals("Bookmark")) {
            BookmarkNode bkmk = (BookmarkNode) node;
            if (bkmk.isPoint()) throw new RuntimeException("range only");
            insertBookmark(xCursor, bkmk.getName(), bkmk.getXmlId());
        } else if (type.equals("ReferenceMark")) {
            ReferenceMarkNode mark = (ReferenceMarkNode) node;
            if (mark.isPoint()) throw new RuntimeException("range only");
            insertReferenceMark(xCursor, mark.getName());
        } else if (type.equals("DocumentIndexMark")) {
            DocumentIndexMarkNode mark = (DocumentIndexMarkNode) node;
            if (mark.isPoint()) throw new RuntimeException("range only");
            insertDocumentIndexMark(xCursor, mark.getName());
        } else if (type.equals("Hyperlink")) {
            HyperlinkNode href = (HyperlinkNode) node;
            insertHyperlink(xCursor, href.getURL());
        } else if (type.equals("Ruby")) {
            RubyNode ruby = (RubyNode) node;
            insertRuby(xCursor, ruby.getRubyText());
        } else if (type.equals("InContentMetadata")) {
            MetaNode meta = (MetaNode) node;
            insertMeta(xCursor, meta.getXmlId());
        } else if (type.equals("MetadataField")) {
            MetaFieldNode meta = (MetaFieldNode) node;
            insertMetaField(xCursor, meta.getXmlId());
        } else if (type.equals("Text")) {
            TextNode text = (TextNode) node;
            insertText(xCursor, text.getContent());
        } else if (type.equals("TextField")) {
            TextFieldNode field = (TextFieldNode) node;
            insertTextField(m_xCursor, field.getContent());
        } else if (type.equals("Footnote")) {
            FootnoteNode note = (FootnoteNode) node;
            insertFootnote(m_xCursor, note.getLabel());
        } else if (type.equals("Frame")) {
            FrameNode frame = (FrameNode) node;
            insertFrame(xCursor, frame.getName(), frame.getAnchor());
        } else if (type.equals("ControlCharacter")) {
            ControlCharacterNode cchar = (ControlCharacterNode) node;
            insertControlCharacter(m_xCursor, cchar.getChar());
        } else if (type.equals("SoftPageBreak")) {
            throw new RuntimeException("sorry, cannot test SoftPageBreak");
        } else {
            throw new RuntimeException("unexpected type: " + type);
        }
    }
}


//----------------------------------------------------------------------

public class TextPortionEnumerationTest extends ComplexTestCase
{
    private XMultiServiceFactory m_xMSF = null;
    private XComponentContext m_xContext = null;
    private XTextDocument m_xDoc = null;
    private String m_TmpDir = null;

    private int m_Count = 1;

//    public String[] getTestMethodNames() { return new String[] { "testLoadStore" }; }
    public String[] getTestMethodNames() {
        return new String[] {
            "testText",
            "testTextField",
//            "testControlChar",
//            "testSoftPageBreak",
            "testFootnote",
            "testFrameAs",
            "testFrameAt",
            "testBookmarkPoint",
            "testBookmark",
            "testBookmarkPointXmlId",
            "testBookmarkXmlId",
            "testRefmarkPoint",
            "testRefmark",
            "testToxmarkPoint",
            "testToxmark",
            "testHyperlink",
            "testHyperlinkEmpty",
            "testRuby",
            "testRubyEmpty",
            "testMeta",
            "testMetaEmpty",
            "testMetaField",
            "testMetaFieldEmpty",
            "testBookmark1",
            "testBookmark2",
            "testRefMark2",
            "testRefMark3",
            "testToxMark2",
            "testToxMark3",
            "testMarks1",
            "testMarks2",
            "testMarks3",
            "testFrameMark1",
            "testFrameMark2",
            "testFrameMark3",
            "testFrameMark4",
            "testFrames1",
            "testFrames2",
            "testFrames3",
            "testFrames4",
            "testFrames5",
            "testRubyHyperlink1",
            "testRubyHyperlink2",
            "testEnd1",
            "testEnd2",
            "testEnd3",
            "testEnd4",
            "testEnd5",
            "testEmpty1",
            "testEmpty2",
            "testEmpty3",
            "test1",
            "testRange1",
            "testRangeHyperlinkHyperlink",
            "testRangeHyperlinkRuby",
            "testRangeRubyHyperlink",
            "testRangeRubyRuby",
            "testRangeHyperlinkMeta",
            "testRangeRubyMeta",
            "testRangeMetaHyperlink",
            "testRangeMetaRuby",
            "testRangeMetaMeta",
            "testRange2",
            "testRange3",
            "testRange4",
            "testRange5",
            "testRange6",
            "testRange7",
            "testMetaXText",
            "testMetaXTextCursor",
            "testMetaXTextAttachToxMark",
            "testMetaXTextAttachRefMark",
            "testMetaXTextAttachTextField",
            "testMetaXTextAttachFootnote",
            "testMetaXTextAttachMeta",
            "testMetaFieldXTextField",
            "testMetaFieldXPropertySet",
            "testLoadStore",
        };
    }

    public void before() throws Exception
    {
        m_xMSF = (XMultiServiceFactory) param.getMSF();
        XPropertySet xPropertySet = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, m_xMSF);
        Object defaultCtx = xPropertySet.getPropertyValue("DefaultContext");
        m_xContext = (XComponentContext)
            UnoRuntime.queryInterface(XComponentContext.class, defaultCtx);
        assure("could not get component context.", m_xContext != null);
        m_xDoc = util.WriterTools.createTextDoc(m_xMSF);
        m_TmpDir = util.utils.getOfficeTemp/*Dir*/(m_xMSF);
        log.println("tempdir: " + m_TmpDir);
    }

    public void after()
    {
        if (m_xDoc != null) {
            util.DesktopTools.closeDoc(m_xDoc);
            m_xDoc = null;
        }
    }

    public void testText() throws Exception
    {
        TreeNode root = new TreeNode();
        TreeNode text = new TextNode("abc");
        root.appendChild(text);
        doTest(root);
    }

    public void testTextField() throws Exception
    {
        String name = mkName("ruby");
        TreeNode root = new TreeNode();
        TreeNode txtf = new TextFieldNode("abc");
        root.appendChild(txtf);
        doTest(root);
    }

    public void testControlChar() throws Exception
    {
//FIXME this is converted to a text portion: ControlCharacter is obsolete
        TreeNode root = new TreeNode();
        TreeNode cchr = new ControlCharacterNode(HARD_HYPHEN);
        root.appendChild(cchr);
        doTest(root);
    }

    public void testSoftPageBreak() throws Exception
    {
//FIXME: insert a soft page break: not done
        TreeNode root = new TreeNode();
        TreeNode spbk = new SoftPageBreakNode();
        TreeNode text = new TextNode("abc");
        root.appendChild(spbk);
        root.appendChild(text);
        doTest(root);
    }

    public void testFootnote() throws Exception
    {
        String name = mkName("ftn");
        TreeNode root = new TreeNode();
        TreeNode ftnd = new FootnoteNode(name);
        root.appendChild(ftnd);
        doTest(root);
    }

    public void testFrameAs() throws Exception
    {
        String name = mkName("frame");
        TreeNode root = new TreeNode();
        TreeNode fram = new FrameNode(name, AS_CHARACTER);
        root.appendChild(fram);
        doTest(root);
    }

    public void testFrameAt() throws Exception
    {
        String name = mkName("frame");
        TreeNode root = new TreeNode();
//        TreeNode text = new TextNode(""); // necessary?
        TreeNode fram = new FrameNode(name, AT_CHARACTER);
//        root.appendChild(text);
        root.appendChild(fram);
        doTest(root);
    }

    public void testBookmarkPoint() throws Exception
    {
        String name = mkName("mark");
        TreeNode root = new TreeNode();
        TreeNode bkmk = new BookmarkNode(name);
        TreeNode text = new TextNode("abc");
        root.appendChild(bkmk);
        root.appendChild(text);
        doTest(root);
    }

    public void testBookmark() throws Exception
    {
        String name = mkName("mark");
        TreeNode root = new TreeNode();
        TreeNode bkm1 = new BookmarkStartNode(name);
        TreeNode text = new TextNode("abc");
        TreeNode bkm2 = new BookmarkEndNode(name);
        root.appendChild(bkm1);
        root.appendChild(text);
        root.appendChild(bkm2);
        doTest(root);
    }

    public void testBookmarkPointXmlId() throws Exception
    {
        String name = mkName("mark");
        StringPair id = mkId("id");
        TreeNode root = new TreeNode();
        TreeNode bkmk = new BookmarkNode(name, id);
        TreeNode text = new TextNode("abc");
        root.appendChild(bkmk);
        root.appendChild(text);
        doTest(root);
    }

    public void testBookmarkXmlId() throws Exception
    {
        String name = mkName("mark");
        StringPair id = mkId("id");
        TreeNode root = new TreeNode();
        TreeNode bkm1 = new BookmarkStartNode(name, id);
        TreeNode text = new TextNode("abc");
        TreeNode bkm2 = new BookmarkEndNode(name, id);
        root.appendChild(bkm1);
        root.appendChild(text);
        root.appendChild(bkm2);
        doTest(root);
    }

    public void testRefmarkPoint() throws Exception
    {
        String name = mkName("refmark");
        TreeNode root = new TreeNode();
        TreeNode rfmk = new ReferenceMarkNode(name);
        TreeNode text = new TextNode("abc");
        root.appendChild(rfmk);
        root.appendChild(text);
        doTest(root);
    }

    public void testRefmark() throws Exception
    {
        String name = mkName("refmark");
        TreeNode root = new TreeNode();
        TreeNode rfm1 = new ReferenceMarkStartNode(name);
        TreeNode text = new TextNode("abc");
        TreeNode rfm2 = new ReferenceMarkEndNode(name);
        root.appendChild(rfm1);
        root.appendChild(text);
        root.appendChild(rfm2);
        doTest(root);
    }

    public void testToxmarkPoint() throws Exception
    {
        String name = mkName("toxmark");
        TreeNode root = new TreeNode();
        TreeNode txmk = new DocumentIndexMarkNode(name);
        TreeNode text = new TextNode("abc");
        root.appendChild(txmk);
        root.appendChild(text);
        doTest(root);
    }

    public void testToxmark() throws Exception
    {
        String name = mkName("toxmark");
        TreeNode root = new TreeNode();
        TreeNode txm1 = new DocumentIndexMarkStartNode(name);
        TreeNode text = new TextNode("abc");
        TreeNode txm2 = new DocumentIndexMarkEndNode(name);
        root.appendChild(txm1);
        root.appendChild(text);
        root.appendChild(txm2);
        doTest(root);
    }

    public void testHyperlink() throws Exception
    {
        String name = mkName("url");
        TreeNode root = new TreeNode();
        TreeNode href = new HyperlinkNode(name);
        TreeNode text = new TextNode("abc");
        href.appendChild(text);
        root.appendChild(href);
        doTest(root);
    }

    public void testHyperlinkEmpty() throws Exception
    {
        String name = mkName("url");
        TreeNode root = new TreeNode();
        TreeNode href = new HyperlinkNode(name);
        TreeNode text = new TextNode("");
        href.appendChild(text);
        root.appendChild(href);
        doTest(root);
    }

    public void testRuby() throws Exception
    {
        String name = mkName("ruby");
        TreeNode root = new TreeNode();
        TreeNode ruby = new RubyNode(name);
        TreeNode text = new TextNode("abc");
        ruby.appendChild(text);
        root.appendChild(ruby);
        doTest(root);
    }

    public void testRubyEmpty() throws Exception
    {
        // BUG: #i91534#
        String name = mkName("ruby");
        TreeNode root = new TreeNode();
        TreeNode ruby = new RubyNode(name);
        root.appendChild(ruby);
        doTest(root);
    }

    public void testMeta() throws Exception
    {
        StringPair id = new StringPair("content.xml", mkName("id"));
        TreeNode root = new TreeNode();
        TreeNode meta = new MetaNode(id);
        TreeNode text = new TextNode("abc");
        root.appendChild(new TextNode("123"));
        meta.appendChild(text);
        root.appendChild(meta);
        doTest(root);
    }

    public void testMetaEmpty() throws Exception
    {
        StringPair id = new StringPair("content.xml", mkName("id"));
        TreeNode root = new TreeNode();
        TreeNode meta = new MetaNode(id);
//        TreeNode text = new TextNode("");
//        meta.appendChild(text);
        root.appendChild(meta);
        doTest(root);
    }

    public void testMetaField() throws Exception
    {
        StringPair id = new StringPair("content.xml", mkName("id"));
        TreeNode root = new TreeNode();
        TreeNode meta = new MetaFieldNode(id);
        TreeNode text = new TextNode("abc");
        root.appendChild(new TextNode("123"));
        meta.appendChild(text);
        root.appendChild(meta);
        doTest(root);
    }

    public void testMetaFieldEmpty() throws Exception
    {
        StringPair id = new StringPair("content.xml", mkName("id"));
        TreeNode root = new TreeNode();
        TreeNode meta = new MetaFieldNode(id);
//        TreeNode text = new TextNode("");
//        meta.appendChild(text);
        root.appendChild(meta);
        doTest(root);
    }

    public void testBookmark1() throws Exception
    {
        String name1 = mkName("mark");
        String name2 = mkName("mark");
        String name3 = mkName("mark");
        TreeNode root = new TreeNode();
        root.appendChild( new BookmarkStartNode(name1) );
        root.appendChild( new BookmarkNode(name2) );
        root.appendChild( new BookmarkStartNode(name3) );
        root.appendChild( new TextNode("abc") );
        root.appendChild( new BookmarkEndNode(name1) );
        root.appendChild( new TextNode("de") );
        root.appendChild( new BookmarkEndNode(name3) );
        doTest(root);
    }

    public void testBookmark2() throws Exception
    {
        String name1 = mkName("mark");
        String name2 = mkName("mark");
        String name3 = mkName("mark");
        TreeNode root = new TreeNode();
        root.appendChild( new BookmarkStartNode(name1) );
        root.appendChild( new TextNode("abc") );
        root.appendChild( new BookmarkNode(name2) );
        root.appendChild( new BookmarkStartNode(name3) );
        root.appendChild( new BookmarkEndNode(name1) );
        root.appendChild( new TextNode("de") );
        root.appendChild( new BookmarkEndNode(name3) );
        doTest(root);
    }

    public void testRefMark2() throws Exception
    {
        String name1 = mkName("refmark");
        TreeNode root = new TreeNode();
        root.appendChild( new ReferenceMarkStartNode(name1) );
        root.appendChild( new TextNode("abc") );
        // BUG: #i102541# (this is actually not unoportenum's fault)
        root.appendChild( new ReferenceMarkEndNode(name1) );
        root.appendChild( new TextNode("de") );
        doTest(root);
    }

    public void testRefMark3() throws Exception
    {
        String name1 = mkName("refmark");
        String name2 = mkName("refmark");
        TreeNode root = new TreeNode();
        root.appendChild( new ReferenceMarkStartNode(name1) );
        root.appendChild( new ReferenceMarkStartNode(name2) );
        root.appendChild( new TextNode("abc") );
        root.appendChild( new ReferenceMarkEndNode(name2) );
        root.appendChild( new ReferenceMarkEndNode(name1) );
        root.appendChild( new TextNode("de") );
        doTest(root);
    }

    public void testToxMark2() throws Exception
    {
        String name1 = mkName("toxmark");
        TreeNode root = new TreeNode();
        root.appendChild( new DocumentIndexMarkStartNode(name1) );
        root.appendChild( new TextNode("abc") );
        root.appendChild( new DocumentIndexMarkEndNode(name1) );
        root.appendChild( new TextNode("de") );
        doTest(root);
    }

    public void testToxMark3() throws Exception
    {
        String name1 = mkName("toxmark");
        String name2 = mkName("toxmark");
        TreeNode root = new TreeNode();
        root.appendChild( new DocumentIndexMarkStartNode(name1) );
        root.appendChild( new DocumentIndexMarkStartNode(name2) );
        root.appendChild( new TextNode("abc") );
        root.appendChild( new DocumentIndexMarkEndNode(name2) );
        root.appendChild( new DocumentIndexMarkEndNode(name1) );
        root.appendChild( new TextNode("de") );
        doTest(root);
    }

    public void testMarks1() throws Exception
    {
        String name1 = mkName("bookmark");
        String name2 = mkName("toxmark");
        String name3 = mkName("refmark");
        String name4 = mkName("toxmark");
        TreeNode root = new TreeNode();
        root.appendChild( new BookmarkStartNode(name1) );
        root.appendChild( new DocumentIndexMarkNode(name2) );
        root.appendChild( new ReferenceMarkStartNode(name3) );
        root.appendChild( new TextNode("abc") );
        root.appendChild( new BookmarkEndNode(name1) );
        root.appendChild( new DocumentIndexMarkStartNode(name4) );
        root.appendChild( new TextNode("de") );
        root.appendChild( new DocumentIndexMarkEndNode(name4) );
        root.appendChild( new ReferenceMarkEndNode(name3) );
        doTest(root);
    }

    public void testMarks2() throws Exception
    {
        String name1 = mkName("bookmark");
        String name2 = mkName("refmark");
        String name3 = mkName("refmark");
        String name4 = mkName("toxmark");
        String name5 = mkName("refmark");
        TreeNode root = new TreeNode();
        root.appendChild( new BookmarkStartNode(name1) );
        root.appendChild( new ReferenceMarkNode(name2) );
        root.appendChild( new ReferenceMarkStartNode(name3) );
        root.appendChild( new TextNode("abc") );
        root.appendChild( new DocumentIndexMarkStartNode(name4) );
        root.appendChild( new ReferenceMarkStartNode(name5) );
        // BUG: #i102541# (this is actually not unoportenum's fault)
        root.appendChild( new ReferenceMarkEndNode(name3) );
        root.appendChild( new TextNode("de") );
        root.appendChild( new DocumentIndexMarkEndNode(name4) );
        root.appendChild( new BookmarkEndNode(name1) );
        root.appendChild( new ReferenceMarkEndNode(name5) );
        doTest(root);
    }

    public void testMarks3() throws Exception
    {
        String name1 = mkName("bookmark");
        String name2 = mkName("refmark");
        String name3 = mkName("refmark");
        String name4 = mkName("toxmark");
        String name5 = mkName("refmark");
        TreeNode root = new TreeNode();
        root.appendChild( new BookmarkStartNode(name1) );
        root.appendChild( new DocumentIndexMarkNode(name2) );
        root.appendChild( new DocumentIndexMarkStartNode(name3) );
        root.appendChild( new TextNode("abc") );
        root.appendChild( new ReferenceMarkStartNode(name4) );
        root.appendChild( new DocumentIndexMarkStartNode(name5) );
        root.appendChild( new DocumentIndexMarkEndNode(name3) );
        root.appendChild( new TextNode("de") );
        root.appendChild( new ReferenceMarkEndNode(name4) );
        root.appendChild( new BookmarkEndNode(name1) );
        root.appendChild( new DocumentIndexMarkEndNode(name5) );
        doTest(root);
    }

    public void testFrameMark1() throws Exception
    {
        String name1 = mkName("bookmark");
        String name2 = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new TextNode("abc") );
        root.appendChild( new BookmarkNode(name1) );
        root.appendChild( new TextNode("de") );
        root.appendChild( new FrameNode(name2, AS_CHARACTER) );
        doTest(root);
    }

    public void testFrameMark2() throws Exception
    {
        // BUG: #i98530#
        String name1 = mkName("bookmark");
        String name2 = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new TextNode("abc") );
        root.appendChild( new BookmarkNode(name1) );
        root.appendChild( new TextNode("de") );
        root.appendChild( new FrameNode(name2, AT_CHARACTER) );
        doTest(root);
    }

    public void testFrameMark3() throws Exception
    {
        String name1 = mkName("frame");
        String name2 = mkName("bookmark");
        TreeNode root = new TreeNode();
        root.appendChild( new TextNode("abc") );
        root.appendChild( new FrameNode(name1, AS_CHARACTER) );
        root.appendChild( new TextNode("de") );
        root.appendChild( new BookmarkNode(name2) );
        doTest(root);
    }

    public void testFrameMark4() throws Exception
    {
        String name1 = mkName("frame");
        String name2 = mkName("bookmark");
        TreeNode root = new TreeNode();
        root.appendChild( new TextNode("abc") );
        root.appendChild( new FrameNode(name1, AT_CHARACTER) );
        root.appendChild( new TextNode("de") );
        root.appendChild( new BookmarkNode(name2) );
        doTest(root);
    }

    public void testFrames1() throws Exception
    {
        String name1 = mkName("frame");
        String name2 = mkName("frame");
        String name3 = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new FrameNode(name1, AT_CHARACTER) );
        root.appendChild( new FrameNode(name2, AT_CHARACTER) );
        root.appendChild( new FrameNode(name3, AT_CHARACTER) );
        doTest(root);
    }

    public void testFrames2() throws Exception
    {
        String name1 = mkName("frame");
        String name2 = mkName("frame");
        String name3 = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new FrameNode(name1, AS_CHARACTER) );
        root.appendChild( new FrameNode(name2, AS_CHARACTER) );
        root.appendChild( new FrameNode(name3, AS_CHARACTER) );
        doTest(root);
    }

    public void testFrames3() throws Exception
    {
        String name1 = mkName("frame");
        String name2 = mkName("frame");
        String name3 = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new FrameNode(name1, AT_CHARACTER) );
        root.appendChild( new FrameNode(name2, AS_CHARACTER) );
        root.appendChild( new FrameNode(name3, AT_CHARACTER) );
        doTest(root);
    }

    public void testFrames4() throws Exception
    {
        String name1 = mkName("frame");
        String name2 = mkName("frame");
        String name3 = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new FrameNode(name1, AT_CHARACTER) );
        root.appendChild( new FrameNode(name2, AT_CHARACTER) );
        root.appendChild( new FrameNode(name3, AS_CHARACTER) );
        doTest(root);
    }

    public void testFrames5() throws Exception
    {
        String name1 = mkName("frame");
        String name2 = mkName("frame");
        String name3 = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new FrameNode(name1, AS_CHARACTER) );
        root.appendChild( new FrameNode(name2, AT_CHARACTER) );
        root.appendChild( new FrameNode(name3, AT_CHARACTER) );
        doTest(root);
    }

    public void testRubyHyperlink1() throws Exception
    {
        String name1 = mkName("ruby");
        String name2 = mkName("url");
        TreeNode root = new TreeNode();
        TreeNode ruby = new RubyNode(name1);
        TreeNode href = new HyperlinkNode(name2);
        href.appendChild( new TextNode("abc") );
        ruby.appendChild(href);
        root.appendChild(ruby);
        doTest(root);
    }

    public void testRubyHyperlink2() throws Exception
    {
        String name1 = mkName("url");
        String name2 = mkName("ruby");
        TreeNode root = new TreeNode();
        TreeNode href = new HyperlinkNode(name1);
        TreeNode ruby = new RubyNode(name2);
        ruby.appendChild( new TextNode("abc") );
        href.appendChild(ruby);
        root.appendChild(href);
        doTest(root);
    }

    public void testEnd1() throws Exception
    {
        String name1 = mkName("bookmark");
        String name2 = mkName("toxmark");
        String name3 = mkName("refmark");
        TreeNode root = new TreeNode();
        root.appendChild( new TextNode("abc") );
        root.appendChild( new BookmarkNode(name1) );
        root.appendChild( new DocumentIndexMarkNode(name2) );
        root.appendChild( new ReferenceMarkNode(name3) );
        doTest(root);
    }

    public void testEnd2() throws Exception
    {
        String name1 = mkName("bookmark");
        String name2 = mkName("frame");
        String name3 = mkName("refmark");
        String name4 = mkName("frame");
        String name5 = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new TextNode("abc") );
        root.appendChild( new BookmarkNode(name1) );
        root.appendChild( new FrameNode(name2, AT_CHARACTER) );
        root.appendChild( new ReferenceMarkNode(name3) );
        root.appendChild( new FrameNode(name4, AT_CHARACTER) );
        root.appendChild( new FrameNode(name5, AT_CHARACTER) );
        doTest(root);
    }

    public void testEnd3() throws Exception
    {
        String name1 = mkName("ftn");
        String name2 = mkName("toxmark");
        TreeNode root = new TreeNode();
        root.appendChild( new TextNode("abc") );
        root.appendChild( new FootnoteNode(name1) );
        root.appendChild( new DocumentIndexMarkNode(name2) );
        doTest(root);
    }

    public void testEnd4() throws Exception
    {
        String name1 = mkName("bookmark");
        String name2 = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new BookmarkStartNode(name1) );
        root.appendChild( new TextNode("abc") );
        root.appendChild( new FrameNode(name2, AS_CHARACTER) );
        root.appendChild( new BookmarkEndNode(name1) );
        doTest(root);
    }

    public void testEnd5() throws Exception
    {
        String name1 = mkName("refmark");
        String name2 = mkName("ruby");
        TreeNode root = new TreeNode();
        root.appendChild( new ReferenceMarkStartNode(name1) );
        root.appendChild( new TextNode("abc") );
        TreeNode ruby = new RubyNode(name2);
        ruby.appendChild( new TextFieldNode("de") );
        root.appendChild(ruby);
        root.appendChild( new ReferenceMarkEndNode(name1) );
        doTest(root);
    }

    public void testEmpty1() throws Exception
    {
        String name1 = mkName("refmark");
        String name2 = mkName("toxmark");
        String name3 = mkName("bookmark");
        String name4 = mkName("frame");
        String name7 = mkName("refmark");
        String name8 = mkName("toxmark");
        String name9 = mkName("bookmark");
        String nameA = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new ReferenceMarkNode(name1) );
        root.appendChild( new DocumentIndexMarkNode(name2) );
        root.appendChild( new BookmarkNode(name3) );
        root.appendChild( new FrameNode(name4, AT_CHARACTER) );
        root.appendChild( new ReferenceMarkNode(name7) );
        root.appendChild( new DocumentIndexMarkNode(name8) );
        root.appendChild( new BookmarkNode(name9) );
        root.appendChild( new FrameNode(nameA, AT_CHARACTER) );
        doTest(root);
    }

    public void testEmpty2() throws Exception
    {
        String name3 = mkName("bookmark");
        String name4 = mkName("frame");
        String name9 = mkName("bookmark");
        String nameA = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new BookmarkNode(name3) );
        root.appendChild( new FrameNode(name4, AT_CHARACTER) );
        root.appendChild( new BookmarkNode(name9) );
        root.appendChild( new FrameNode(nameA, AT_CHARACTER) );
        doTest(root);
    }

    public void testEmpty3() throws Exception
    {
        String name1 = mkName("refmark");
        String name2 = mkName("toxmark");
        String name3 = mkName("bookmark");
        String name4 = mkName("frame");
        String name5 = mkName("url");
        String name6 = mkName("ruby");
        String name7 = mkName("refmark");
        String name8 = mkName("toxmark");
        String name9 = mkName("bookmark");
        String nameA = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new ReferenceMarkNode(name1) );
        root.appendChild( new DocumentIndexMarkNode(name2) );
        root.appendChild( new BookmarkNode(name3) );
        root.appendChild( new FrameNode(name4, AT_CHARACTER) );
        /* currently empty hyperlinks may get eaten...
        TreeNode href = new HyperlinkNode(name5);
        href.appendChild( new TextNode("") );
        root.appendChild(href);
        */
        TreeNode ruby = new RubyNode(name6);
        root.appendChild(ruby);
        root.appendChild( new ReferenceMarkNode(name7) );
        root.appendChild( new DocumentIndexMarkNode(name8) );
        root.appendChild( new BookmarkNode(name9) );
        root.appendChild( new FrameNode(nameA, AT_CHARACTER) );
        doTest(root);
    }

    public void test1() throws Exception
    {
        String name1 = mkName("frame");
        String name2 = mkName("bookmark");
        String name3 = mkName("ruby");
        String name4 = mkName("ftn");
        String name5 = mkName("frame");
        TreeNode root = new TreeNode();
        root.appendChild( new FrameNode(name1, AT_CHARACTER) );
        root.appendChild( new BookmarkStartNode(name2) );
        root.appendChild( new TextNode("abc") );
        TreeNode ruby = new RubyNode(name3);
        ruby.appendChild( new TextNode("de") );
        ruby.appendChild( new FootnoteNode(name4) );
        ruby.appendChild( new BookmarkEndNode(name2) );
        root.appendChild(ruby);
        root.appendChild( new TextNode("fg") );
        root.appendChild( new FrameNode(name5, AT_CHARACTER) );
        root.appendChild( new TextFieldNode("h") );
        doTest(root);
    }

    /* some range tests for the insertion: these are for the current
       API which treats hyperlinks and rubys not as entities, but as formatting
       attributes; if these ever become entities, they should not be split!
     */

    public void testRange1() throws Exception
    {
        String name1 = mkName("url");
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("12345");
        inserter.insertRange(new Range(0, 0, text));
        TreeNode url1 = new HyperlinkNode(name1);
        Range range1 = new Range(0, 5, url1);
        inserter.insertRange(range1);
        TreeNode root = new TreeNode();
        root.appendChild( url1 );
        url1.appendChild( text );
        doTest(root, false);
    }

    public void testRangeHyperlinkHyperlink() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode url1 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(1, 4, url1) );
        // overlap left
        TreeNode url2 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(0, 2, url2) );
        TreeNode root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("12") ) )
            .appendChild( url1.dup().appendChild( new TextNode("34") ) )
            .appendChild( new TextNode("56789") );
        doTest(root, false);
        // overlap right
        TreeNode url3 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(3, 7, url3) );
        root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("12") ) )
            .appendChild( url1.dup().appendChild( new TextNode("3") ) )
            .appendChild( url3.dup().appendChild( new TextNode("4567") ) )
            .appendChild( new TextNode("89") );
        doTest(root, false);
        // around
        TreeNode url4 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(3, 7, url4) );
        root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("12") ) )
            .appendChild( url1.dup().appendChild( new TextNode("3") ) )
            .appendChild( url4.dup().appendChild( new TextNode("4567") ) )
            .appendChild( new TextNode("89") );
        doTest(root, false);
        // inside
        TreeNode url5 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(4, 6, url5) );
        root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("12") ) )
            .appendChild( url1.dup().appendChild( new TextNode("3") ) )
            .appendChild( url4.dup().appendChild( new TextNode("4") ) )
            .appendChild( url5.dup().appendChild( new TextNode("56") ) )
            .appendChild( url4.dup().appendChild( new TextNode("7") ) )
            .appendChild( new TextNode("89") );
        doTest(root, false);
    }

    public void testRangeHyperlinkRuby() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode url1 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(1, 4, url1) );
        // overlap left
        TreeNode rby2 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(0, 2, rby2) );
        TreeNode root = new TreeNode()
            .appendChild( rby2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( url1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( url1.dup().appendChild( new TextNode("34") ) )
            .appendChild( new TextNode("56789") );
        doTest(root, false);
        // overlap right
        TreeNode rby3 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(3, 5, rby3) );
        root = new TreeNode()
            .appendChild( rby2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( url1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( url1.dup().appendChild( new TextNode("3") ) )
            .appendChild( rby3.dup()
                .appendChild( url1.dup().appendChild( new TextNode("4") ) )
                .appendChild( new TextNode("5") ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);
        // around
        TreeNode rby4 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(2, 3, rby4) );
        root = new TreeNode()
            .appendChild( rby2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( url1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( rby4.dup()
                .appendChild( url1.dup().appendChild( new TextNode("3") ) ) )
            .appendChild( rby3.dup()
                .appendChild( url1.dup().appendChild( new TextNode("4") ) )
                .appendChild( new TextNode("5") ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);
        // inside
        TreeNode url5 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(6, 9, url5) );
        TreeNode rby6 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(7, 8, rby6) );
        root = new TreeNode()
            .appendChild( rby2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( url1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( rby4.dup()
                .appendChild( url1.dup().appendChild( new TextNode("3") ) ) )
            .appendChild( rby3.dup()
                .appendChild( url1.dup().appendChild( new TextNode("4") ) )
                .appendChild( new TextNode("5") ) )
            .appendChild( new TextNode("6") )
            .appendChild( url5.dup().appendChild( new TextNode("7") ) )
            .appendChild( rby6.dup()
                .appendChild( url5.dup().appendChild( new TextNode("8") ) ) )
            .appendChild( url5.dup().appendChild( new TextNode("9") ) );
        doTest(root, false);
    }

    public void testRangeRubyHyperlink() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode rby1 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(1, 6, rby1) );
        // overlap left
        TreeNode url2 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(0, 3, url2) );
        TreeNode root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("1") ) )
            .appendChild( rby1.dup()
                .appendChild( url2.dup().appendChild( new TextNode("23") ) )
                .appendChild( new TextNode("456") ) )
            .appendChild( new TextNode("789") );
        doTest(root, false);
        // overlap right
        TreeNode url3 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(5, 7, url3) );
        root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("1") ) )
            .appendChild( rby1.dup()
                .appendChild( url2.dup().appendChild( new TextNode("23") ) )
                .appendChild( new TextNode("45") )
                .appendChild( url3.dup().appendChild( new TextNode("6") ) ) )
            .appendChild( url3.dup().appendChild( new TextNode("7") ) )
            .appendChild( new TextNode("89") );
        doTest(root, false);
        // around (not quite, due to API)
        TreeNode url4 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(1, 8, url4) );
        root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("1") ) )
            .appendChild( rby1.dup()
                .appendChild( url4.dup()
                    .appendChild( new TextNode("23456") ) ) )
            .appendChild( url4.dup().appendChild( new TextNode("78") ) )
            .appendChild( new TextNode("9") );
        doTest(root, false);
        // inside
        TreeNode url5 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(3, 5, url5) );
        root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("1") ) )
            .appendChild( rby1.dup()
                .appendChild( url4.dup()
                    .appendChild( new TextNode("23") ) )
                .appendChild( url5.dup()
                    .appendChild( new TextNode("45") ) )
                .appendChild( url4.dup()
                    .appendChild( new TextNode("6") ) ) )
            .appendChild( url4.dup().appendChild( new TextNode("78") ) )
            .appendChild( new TextNode("9") );
        doTest(root, false);
    }

    public void testRangeRubyRuby() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode rby1 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(1, 4, rby1) );
        // overlap left
        TreeNode rby2 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(0, 2, rby2) );
        TreeNode root = new TreeNode()
            .appendChild( rby2.dup().appendChild( new TextNode("12") ) )
            .appendChild( rby1.dup().appendChild( new TextNode("34") ) )
            .appendChild( new TextNode("56789") );
        doTest(root, false);
        // overlap right
        TreeNode rby3 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(3, 7, rby3) );
        root = new TreeNode()
            .appendChild( rby2.dup().appendChild( new TextNode("12") ) )
            .appendChild( rby1.dup().appendChild( new TextNode("3") ) )
            .appendChild( rby3.dup().appendChild( new TextNode("4567") ) )
            .appendChild( new TextNode("89") );
        doTest(root, false);
        // around
        TreeNode rby4 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(3, 7, rby4) );
        root = new TreeNode()
            .appendChild( rby2.dup().appendChild( new TextNode("12") ) )
            .appendChild( rby1.dup().appendChild( new TextNode("3") ) )
            .appendChild( rby4.dup().appendChild( new TextNode("4567") ) )
            .appendChild( new TextNode("89") );
        doTest(root, false);
        // inside
        TreeNode rby5 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(4, 6, rby5) );
        root = new TreeNode()
            .appendChild( rby2.dup().appendChild( new TextNode("12") ) )
            .appendChild( rby1.dup().appendChild( new TextNode("3") ) )
            .appendChild( rby4.dup().appendChild( new TextNode("4") ) )
            .appendChild( rby5.dup().appendChild( new TextNode("56") ) )
            .appendChild( rby4.dup().appendChild( new TextNode("7") ) )
            .appendChild( new TextNode("89") );
        doTest(root, false);
    }

    public void testRangeHyperlinkMeta() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode url1 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(1, 4, url1) );
        // overlap left
        TreeNode met2 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(0, 2, met2) );
        TreeNode root = new TreeNode()
            .appendChild( met2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( url1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( url1.dup().appendChild( new TextNode("34") ) )
            .appendChild( new TextNode("56789") );
        doTest(root, false);
        // overlap right
        TreeNode met3 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(4/*-1*/, 6/*-1*/, met3) );
        root = new TreeNode()
            .appendChild( met2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( url1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( url1.dup().appendChild( new TextNode("3") ) )
            .appendChild( met3.dup()
                .appendChild( url1.dup().appendChild( new TextNode("4") ) )
                .appendChild( new TextNode("5") ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);
        // around
        TreeNode met4 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(3/*-1*/, 4/*-1*/, met4) );
        root = new TreeNode()
            .appendChild( met2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( url1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( met4.dup()
                .appendChild( url1.dup().appendChild( new TextNode("3") ) ) )
            .appendChild( met3.dup()
                .appendChild( url1.dup().appendChild( new TextNode("4") ) )
                .appendChild( new TextNode("5") ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);
        // inside
        TreeNode url5 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(9/*-3*/, 12/*-3*/, url5) );
        TreeNode met6 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(10/*-3*/, 11/*-3*/, met6) );
        root = new TreeNode()
            .appendChild( met2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( url1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( met4.dup()
                .appendChild( url1.dup().appendChild( new TextNode("3") ) ) )
            .appendChild( met3.dup()
                .appendChild( url1.dup().appendChild( new TextNode("4") ) )
                .appendChild( new TextNode("5") ) )
            .appendChild( new TextNode("6") )
            .appendChild( url5.dup().appendChild( new TextNode("7") ) )
            .appendChild( met6.dup()
                .appendChild( url5.dup().appendChild( new TextNode("8") ) ) )
            .appendChild( url5.dup().appendChild( new TextNode("9") ) );
        doTest(root, false);
    }

    public void testRangeRubyMeta() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode rby1 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(1, 4, rby1) );
        // overlap left
        TreeNode met2 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(0, 2, met2) );
        TreeNode root = new TreeNode()
            .appendChild( met2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( rby1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( rby1.dup().appendChild( new TextNode("34") ) )
            .appendChild( new TextNode("56789") );
        doTest(root, false);
        // overlap right
        TreeNode met3 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(4/*-1*/, 6/*-1*/, met3) );
        root = new TreeNode()
            .appendChild( met2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( rby1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( rby1.dup().appendChild( new TextNode("3") ) )
            .appendChild( met3.dup()
                .appendChild( rby1.dup().appendChild( new TextNode("4") ) )
                .appendChild( new TextNode("5") ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);
        // around
        TreeNode met4 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(3/*-1*/, 4/*-1*/, met4) );
        root = new TreeNode()
            .appendChild( met2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( rby1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( met4.dup()
                .appendChild( rby1.dup().appendChild( new TextNode("3") ) ) )
            .appendChild( met3.dup()
                .appendChild( rby1.dup().appendChild( new TextNode("4") ) )
                .appendChild( new TextNode("5") ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);
        // inside
        TreeNode rby5 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(9/*-3*/, 12/*-3*/, rby5) );
        TreeNode met6 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(10/*-3*/, 11/*-3*/, met6) );
        root = new TreeNode()
            .appendChild( met2.dup()
                .appendChild( new TextNode("1") )
                .appendChild( rby1.dup().appendChild( new TextNode("2") ) ) )
            .appendChild( met4.dup()
                .appendChild( rby1.dup().appendChild( new TextNode("3") ) ) )
            .appendChild( met3.dup()
                .appendChild( rby1.dup().appendChild( new TextNode("4") ) )
                .appendChild( new TextNode("5") ) )
            .appendChild( new TextNode("6") )
            .appendChild( rby5.dup()
                .appendChild( new TextNode("7") )
                .appendChild( met6.dup()
                    .appendChild( new TextNode("8") ) )
                .appendChild( new TextNode("9") ) );
        doTest(root, false);
    }

    public void testRangeMetaHyperlink() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode met1 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(1, 6, met1) );
        // overlap left
        TreeNode url2 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(0, 4/*-1*/, url2) );
        TreeNode root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("1") ) )
            .appendChild( met1.dup()
                .appendChild( url2.dup().appendChild( new TextNode("23") ) )
                .appendChild( new TextNode("456") ) )
            .appendChild( new TextNode("789") );
        doTest(root, false);
        // overlap right
        TreeNode url3 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(6/*-1*/, 8/*-1*/, url3) );
        root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("1") ) )
            .appendChild( met1.dup()
                .appendChild( url2.dup().appendChild( new TextNode("23") ) )
                .appendChild( new TextNode("45") )
                .appendChild( url3.dup().appendChild( new TextNode("6") ) ) )
            .appendChild( url3.dup().appendChild( new TextNode("7") ) )
            .appendChild( new TextNode("89") );
        doTest(root, false);
        // around (not quite, due to API)
        TreeNode url4 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(1, 9/*-1*/, url4) );
        root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("1") ) )
            .appendChild( met1.dup()
                .appendChild( url4.dup()
                    .appendChild( new TextNode("23456") ) ) )
            .appendChild( url4.dup().appendChild( new TextNode("78") ) )
            .appendChild( new TextNode("9") );
        doTest(root, false);
        // inside
        TreeNode url5 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(4/*-1*/, 6/*-1*/, url5) );
        root = new TreeNode()
            .appendChild( url2.dup().appendChild( new TextNode("1") ) )
            .appendChild( met1.dup()
                .appendChild( url4.dup()
                    .appendChild( new TextNode("23") ) )
                .appendChild( url5.dup()
                    .appendChild( new TextNode("45") ) )
                .appendChild( url4.dup()
                    .appendChild( new TextNode("6") ) ) )
            .appendChild( url4.dup().appendChild( new TextNode("78") ) )
            .appendChild( new TextNode("9") );
        doTest(root, false);
    }

    public void testRangeMetaRuby() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode met1 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(1, 5, met1) );
        // overlap left
        TreeNode rby2 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(0, 3/*-1*/, rby2) );
        TreeNode root = new TreeNode()
            .appendChild( rby2.dup().appendChild( new TextNode("1") ) )
            .appendChild( met1.dup()
                .appendChild( rby2.dup().appendChild( new TextNode("2") ) )
                .appendChild( new TextNode("345") ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);
        // overlap right
        TreeNode rby3 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(5/*-1*/, 7/*-1*/, rby3) );
        root = new TreeNode()
            .appendChild( rby2.dup().appendChild( new TextNode("1") ) )
            .appendChild( met1.dup()
                .appendChild( rby2.dup().appendChild( new TextNode("2") ) )
                .appendChild( new TextNode("34") )
                .appendChild( rby3.dup().appendChild( new TextNode("5") ) ) )
            .appendChild( rby3.dup().appendChild( new TextNode("6") ) )
            .appendChild( new TextNode("789") );
        doTest(root, false);
        // around
        TreeNode rby4 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(1, 7/*-1*/, rby4) );
        root = new TreeNode()
            .appendChild( rby2.dup().appendChild( new TextNode("1") ) )
            .appendChild( rby4.dup()
                .appendChild( met1.dup()
                    .appendChild( new TextNode("2345") ) )
                .appendChild( new TextNode("6") ) )
            .appendChild( new TextNode("789") );
        doTest(root, false);
        // inside
        TreeNode met5 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(7/*-1*/, 9/*-1*/, met5) );
        TreeNode rby6 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(9/*-2*/, 10/*-2*/, rby6) );
        root = new TreeNode()
            .appendChild( rby2.dup().appendChild( new TextNode("1") ) )
            .appendChild( rby4.dup()
                .appendChild( met1.dup()
                    .appendChild( new TextNode("2345") ) )
                .appendChild( new TextNode("6") ) )
            .appendChild( met5.dup()
                .appendChild( new TextNode("7") )
                .appendChild( rby6.dup()
                    .appendChild( new TextNode("8") ) ) )
            .appendChild( new TextNode("9") );
        doTest(root, false);
        // inside, with invalid range that includes the dummy char
        TreeNode rby7 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(7/*-1*/, 9/*-2*/, rby7) );
        root = new TreeNode()
            .appendChild( rby2.dup().appendChild( new TextNode("1") ) )
            .appendChild( rby4.dup()
                .appendChild( met1.dup()
                    .appendChild( new TextNode("2345") ) )
                .appendChild( new TextNode("6") ) )
            .appendChild( met5.dup()
                .appendChild( rby7.dup()
                    .appendChild( new TextNode("7") ) )
                .appendChild( rby6.dup()
                    .appendChild( new TextNode("8") ) ) )
            .appendChild( new TextNode("9") );
        doTest(root, false);
        // around, at same position as meta
        TreeNode rby8 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(7/*-1*/, 10/*-2*/, rby8) );
        root = new TreeNode()
            .appendChild( rby2.dup().appendChild( new TextNode("1") ) )
            .appendChild( rby4.dup()
                .appendChild( met1.dup()
                    .appendChild( new TextNode("2345") ) )
                .appendChild( new TextNode("6") ) )
            .appendChild( rby8.dup()
                .appendChild( met5.dup()
                    .appendChild( new TextNode("78") ) ) )
            .appendChild( new TextNode("9") );
        doTest(root, false);
    }

    public void testRangeMetaMeta() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode met1 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(3, 6, met1) );
        // overlap left
        TreeNode met2 = new MetaNode( mkId("id") );
        try {
            inserter.insertRange( new Range(0, 4, met2) );
            assure("testRangeMetaMeta: overlap left allowed", false);
        } catch (com.sun.star.lang.IllegalArgumentException e) { /* ignore */ }
        TreeNode root = new TreeNode()
            .appendChild( new TextNode("123") )
            .appendChild( met1.dup().appendChild( new TextNode("456") ) )
            .appendChild( new TextNode("789") );
        doTest(root, false);
        // overlap right
        TreeNode met3 = new MetaNode( mkId("id") );
        try {
            inserter.insertRange( new Range(5/*-1*/, 8/*-1*/, met3) );
            assure("testRangeMetaMeta: overlap right allowed", false);
        } catch (com.sun.star.lang.IllegalArgumentException e) { /* ignore */ }
        root = new TreeNode()
            .appendChild( new TextNode("123") )
            .appendChild( met1.dup().appendChild( new TextNode("456") ) )
            .appendChild( new TextNode("789") );
        doTest(root, false);
        // around
        TreeNode met4 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(3, 7/*-1*/, met4) );
        root = new TreeNode()
            .appendChild( new TextNode("123") )
            .appendChild( met4.dup()
                .appendChild( met1.dup().appendChild( new TextNode("456") ) ) )
            .appendChild( new TextNode("789") );
        doTest(root, false);
        // inside
        TreeNode met5 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(6/*-2*/, 8/*-2*/, met5) );
        root = new TreeNode()
            .appendChild( new TextNode("123") )
            .appendChild( met4.dup()
                .appendChild( met1.dup()
                    .appendChild( new TextNode("4") )
                    .appendChild( met5.dup()
                        .appendChild( new TextNode("56") ) ) ) )
            .appendChild( new TextNode("789") );
        doTest(root, false);
    }

    public void testRange2() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode met1 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(1, 8, met1) );
        TreeNode met2 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(3/*-1*/, 8/*-1*/, met2) );
        TreeNode met3 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(5/*-2*/, 8/*-2*/, met3) );
        TreeNode root = new TreeNode()
            .appendChild( new TextNode("1") )
            .appendChild( met1.dup()
                .appendChild( new TextNode("2") )
                .appendChild( met2.dup()
                    .appendChild( new TextNode("3") )
                    .appendChild( met3.dup()
                        .appendChild( new TextNode("456") ) )
                    .appendChild( new TextNode("7") ) )
                .appendChild( new TextNode("8") ) )
            .appendChild( new TextNode("9") );
        doTest(root, false);
        // split ruby at every meta start!
        TreeNode rby4 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(0, 7/*-3*/, rby4) );
        root = new TreeNode()
            .appendChild( rby4.dup()
                .appendChild( new TextNode("1") ) )
            .appendChild( met1.dup()
                .appendChild( rby4.dup()
                    .appendChild( new TextNode("2") ) )
                .appendChild( met2.dup()
                    .appendChild( rby4.dup()
                        .appendChild( new TextNode("3") ) )
                    .appendChild( met3.dup()
                        .appendChild( rby4.dup()
                            .appendChild( new TextNode("4") ) )
                        .appendChild( new TextNode("56") ) )
                    .appendChild( new TextNode("7") ) )
                .appendChild( new TextNode("8") ) )
            .appendChild( new TextNode("9") );
        doTest(root, false);
        // split ruby at every meta end!
        TreeNode rby5 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(8/*-3*/, 12/*-3*/, rby5) );
        root = new TreeNode()
            .appendChild( rby4.dup()
                .appendChild( new TextNode("1") ) )
            .appendChild( met1.dup()
                .appendChild( rby4.dup()
                    .appendChild( new TextNode("2") ) )
                .appendChild( met2.dup()
                    .appendChild( rby4.dup()
                        .appendChild( new TextNode("3") ) )
                    .appendChild( met3.dup()
                        .appendChild( rby4.dup()
                            .appendChild( new TextNode("4") ) )
                        .appendChild( new TextNode("5") )
                        .appendChild( rby5.dup()
                            .appendChild( new TextNode("6") ) ) )
                    .appendChild( rby5.dup()
                        .appendChild( new TextNode("7") ) ) )
                .appendChild( rby5.dup()
                    .appendChild( new TextNode("8") ) ) )
            .appendChild( rby5.dup()
                .appendChild( new TextNode("9") ) );
        doTest(root, false);
    }

    public void testRange3() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode rby1 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(0, 9, rby1) );
        TreeNode met2 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(2, 7, met2) );
        TreeNode root = new TreeNode()
            .appendChild( rby1.dup()
                .appendChild( new TextNode("12") )
                .appendChild( met2.dup()
                    .appendChild( new TextNode("34567") ) )
                .appendChild( new TextNode("89") ) );
        doTest(root, false);
        // overwrite outer ruby, split remains at inner meta!
        TreeNode rby3 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(5/*-1*/, 6/*-1*/, rby3) );
        root = new TreeNode()
            .appendChild( rby1.dup()
                .appendChild( new TextNode("12") ) )
            .appendChild( met2.dup()
                .appendChild( rby1.dup()
                    .appendChild( new TextNode("34") ) )
                .appendChild( rby3.dup()
                    .appendChild( new TextNode("5") ) )
                .appendChild( rby1.dup()
                    .appendChild( new TextNode("67") ) ) )
            .appendChild( rby1.dup()
                .appendChild( new TextNode("89") ) );
        doTest(root, false);
    }

    public void testRange4() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode rby1 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(0, 9, rby1) );
        TreeNode met2 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(1, 8, met2) );
        TreeNode met3 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(3/*-1*/, 8/*-1*/, met3) );
        TreeNode met4 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(5/*-2*/, 8/*-2*/, met4) );
        TreeNode root = new TreeNode()
            .appendChild( rby1.dup()
                .appendChild( new TextNode("1") )
                .appendChild( met2.dup()
                    .appendChild( new TextNode("2") )
                    .appendChild( met3.dup()
                        .appendChild( new TextNode("3") )
                        .appendChild( met4.dup()
                            .appendChild( new TextNode("456") ) )
                        .appendChild( new TextNode("7") ) )
                    .appendChild( new TextNode("8") ) )
                .appendChild( new TextNode("9") ) );
        doTest(root, false);
        // overwrite outer ruby, split remains at every inner meta!
        TreeNode rby5 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(7/*-3*/, 8/*-3*/, rby5) );
        root = new TreeNode()
            .appendChild( rby1.dup()
                .appendChild( new TextNode("1") ) )
            .appendChild( met2.dup()
                .appendChild( rby1.dup()
                    .appendChild( new TextNode("2") ) )
                .appendChild( met3.dup()
                    .appendChild( rby1.dup()
                        .appendChild( new TextNode("3") ) )
                    .appendChild( met4.dup()
                        .appendChild( rby1.dup()
                            .appendChild( new TextNode("4") ) )
                        .appendChild( rby5.dup()
                            .appendChild( new TextNode("5") ) )
                        .appendChild( rby1.dup()
                            .appendChild( new TextNode("6") ) ) )
                    .appendChild( rby1.dup()
                        .appendChild( new TextNode("7") ) ) )
                .appendChild( rby1.dup()
                    .appendChild( new TextNode("8") ) ) )
            .appendChild( rby1.dup()
                .appendChild( new TextNode("9") ) );
        doTest(root, false);
    }

    public void testRange5() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode rby1 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(0, 9, rby1) );
        TreeNode met2 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(1, 3, met2) );
        TreeNode met3 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(5/*-1*/, 6/*-1*/, met3) );
        TreeNode met4 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(8/*-2*/, 10/*-2*/, met4) );
        TreeNode root = new TreeNode()
            .appendChild( rby1.dup()
                .appendChild( new TextNode("1") )
                .appendChild( met2.dup().appendChild( new TextNode("23") ) )
                .appendChild( new TextNode("4") )
                .appendChild( met3.dup().appendChild( new TextNode("5") ) )
                .appendChild( new TextNode("6") )
                .appendChild( met4.dup().appendChild( new TextNode("78") ) )
                .appendChild( new TextNode("9") ) );
        doTest(root, false);
        // overwrite outer ruby, but split at inner metas!
        TreeNode rby5 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(3/*-1*/, 10/*-3*/, rby5) );
        root = new TreeNode()
            .appendChild( rby1.dup()
                .appendChild( new TextNode("1") ) )
            .appendChild( met2.dup()
                .appendChild( rby1.dup()
                    .appendChild( new TextNode("2") ) )
                .appendChild( rby5.dup()
                    .appendChild( new TextNode("3") ) ) )
            .appendChild( rby5.dup()
                .appendChild( new TextNode("4") )
                .appendChild( met3.dup()
                    .appendChild( new TextNode("5") ) )
                .appendChild( new TextNode("6") ) )
            .appendChild( met4.dup()
                .appendChild( rby5.dup()
                    .appendChild( new TextNode("7") ) )
                .appendChild( rby1.dup()
                    .appendChild( new TextNode("8") ) ) )
            .appendChild( rby1.dup()
                .appendChild( new TextNode("9") ) );
        doTest(root, false);
    }

    public void testRange6() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode met1 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(1, 5, met1) );
        TreeNode met2 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(3/*-1*/, 6/*-1*/, met2) );
        TreeNode met3 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(5/*-2*/, 7/*-2*/, met3) );
        TreeNode root = new TreeNode()
            .appendChild( new TextNode("1") )
            .appendChild( met1.dup()
                .appendChild( new TextNode("2") )
                .appendChild( met2.dup()
                    .appendChild( new TextNode("3") )
                    .appendChild( met3.dup()
                        .appendChild( new TextNode("45") ) ) ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);
        // split at 3 metas, all at same position
        TreeNode rby4 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(7/*-3*/, 10/*-3*/, rby4) );
        root = new TreeNode()
            .appendChild( new TextNode("1") )
            .appendChild( met1.dup()
                .appendChild( new TextNode("2") )
                .appendChild( met2.dup()
                    .appendChild( new TextNode("3") )
                    .appendChild( met3.dup()
                        .appendChild( new TextNode("4") )
                        .appendChild( rby4.dup()
                            .appendChild( new TextNode("5") ) ) ) ) )
            .appendChild( rby4.dup()
                .appendChild( new TextNode("67") ) )
            .appendChild( new TextNode("89") );
        doTest(root, false);
    }

    public void testRange7() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123456789");
        inserter.insertRange( new Range(0, 0, text) );
        TreeNode url1 = new HyperlinkNode( mkName("url") );
        inserter.insertRange( new Range(1, 5, url1) );
        TreeNode met2 = new MetaNode( mkId("id") );
        inserter.insertRange( new Range(3, 5, met2) );
        TreeNode root = new TreeNode()
            .appendChild( new TextNode("1") )
            .appendChild( url1.dup()
                .appendChild( new TextNode("23") ) )
            .appendChild( met2.dup()
                .appendChild( url1.dup()
                    .appendChild( new TextNode("45") ) ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);
        // this should result in not splitting the hyperlink, but due to API
        // we can't tell :(
        TreeNode rby3 = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(5/*-1*/, 8/*-1*/, rby3) );
        root = new TreeNode()
            .appendChild( new TextNode("1") )
            .appendChild( url1.dup()
                .appendChild( new TextNode("23") ) )
            .appendChild( met2.dup()
                .appendChild( url1.dup()
                    .appendChild( new TextNode("4") ) )
                .appendChild( rby3.dup()
                    .appendChild( url1.dup()
                        .appendChild( new TextNode("5") ) ) ) )
            .appendChild( rby3.dup()
                .appendChild( new TextNode("67") ) )
            .appendChild( new TextNode("89") );
        doTest(root, false);
    }

    /* TODO: test partial selection, test UNDO/REDO */

    /** test SwXMeta XText interface */
    public void testMetaXText() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("12AB6789");
        inserter.insertRange( new Range(0, 0, text) );
        MetaNode meta = new MetaNode( mkId("id") );
//        inserter.insertRange( new Range(3, 5, met2) );
        XTextContent xMeta = inserter.makeMeta();

        XText xDocText = m_xDoc.getText();
        XTextCursor xDocTextCursor = xDocText.createTextCursor();
        xDocTextCursor.goRight((short)3, false);
        xDocTextCursor.goRight((short)2, true);
        xDocText.insertTextContent(xDocTextCursor, xMeta, true);
//        xMeta.attach(xDocTextCursor);

        XMetadatable xMetadatable = (XMetadatable)
            UnoRuntime.queryInterface(XMetadatable.class, xMeta);
        xMetadatable.setMetadataReference(meta.getXmlId());
        XText xText = (XText) UnoRuntime.queryInterface(XText.class, xMeta);

        XText xParentText = xText.getText();
        assure("getText(): no parent", xParentText != null);

        XTextRange xStart = xText.getStart();
        assure("getStart(): no start", xStart != null);

        XTextRange xEnd = xText.getEnd();
        assure("getEnd(): no end", xEnd != null);

        /*
        String string = xText.getString();
        assure("getString(): invalid string returned",
            string != null && "AB".equals(string) );
            */

        xText.setString("45");

        {
            String string = xText.getString();
            assure("getString(): invalid string returned: " + string,
                string != null && "45".equals(string) );
        }

        XTextCursor xTextCursor = xText.createTextCursor();
        assure("createTextCursor(): failed", xTextCursor != null);

        try {
            xText.createTextCursorByRange(null);
            assure("createTextCursorByRange(): null allowed?", false);
        } catch (RuntimeException e) { /* expected */ }

        XTextCursor xTextCursorStart = xText.createTextCursorByRange(xStart);
        assure("createTextCursorByRange(): failed for start",
                xTextCursorStart != null);

        XTextCursor xTextCursorEnd = xText.createTextCursorByRange(xEnd);
        assure("createTextCursorByRange(): failed for end",
                xTextCursorEnd != null);

        // move outside meta
        xDocTextCursor.gotoStart(false);

        try {
            xText.insertString(null, "foo", false);
            assure("insertString(): null allowed?", false);
        } catch (RuntimeException e) { /* expected */ }

        try {
            xText.insertString(xDocTextCursor, "foo", false);
            assure("insertString(): cursor outside allowed?", false);
        } catch (RuntimeException e) { /* expected */ }

        xStart = xText.getStart();
        xText.insertString(xStart, "A", false);
        {
            String string = xText.getString();
            assure("getString(): invalid string returned: " + string,
                string != null && "A45".equals(string) );
        }

        xText.insertString(xEnd, "B", false);
        {
            String string = xText.getString();
            assure("getString(): invalid string returned: " + string,
                string != null && "A45B".equals(string) );
        }

        try {
            xText.insertControlCharacter(null, HARD_HYPHEN, false);
            assure("insertControlCharacter(): null allowed?", false);
        } catch (com.sun.star.lang.IllegalArgumentException e) { /* ignore */ }

        xStart = xText.getStart();
        try {
            xText.insertControlCharacter(xDocTextCursor, HARD_HYPHEN, false);
            assure("insertControlCharacter(): cursor outside allowed?", false);
        } catch (com.sun.star.lang.IllegalArgumentException e) { /* ignore */ }

        xText.insertControlCharacter(xStart, HARD_HYPHEN, false);
        {
            String string = xText.getString();
            assure("getString(): invalid string returned: " + string,
                string != null && ('\u2011' + "A45B").equals(string) );
        }

        xText.insertControlCharacter(xEnd, HARD_HYPHEN, false);
        {
            String string = xText.getString();
            assure("getString(): invalid string returned: " + string,
                string != null &&
                ('\u2011' + "A45B" + '\u2011').equals(string) );
        }

        xText.setString("45");

        try {
            xText.insertTextContent(null, xMeta, false);
            assure("insertTextContent(): null range allowed?", false);
        } catch (com.sun.star.lang.IllegalArgumentException e) { /* ignore */ }

        try {
            xText.insertTextContent(xStart, null, false);
            assure("insertTextContent(): null content allowed?", false);
        } catch (com.sun.star.lang.IllegalArgumentException e) { /* ignore */ }

        try {
            xText.insertTextContent(xDocTextCursor, xMeta, false);
            assure("insertTextContent(): cursor outside allowed?", false);
        } catch (com.sun.star.lang.IllegalArgumentException e) { /* ignore */ }

        TextFieldNode field1 = new TextFieldNode( "f1" );
        TextFieldNode field2 = new TextFieldNode( "f2" );
        XTextContent xField1 = inserter.makeTextField(field1.getContent());
        XTextContent xField2 = inserter.makeTextField(field2.getContent());

        xStart = xText.getStart();
        xText.insertTextContent(xStart, xField1, false);

        TreeNode root = new TreeNode()
            .appendChild( new TextNode("12") )
            .appendChild( meta.dup()
                .appendChild( field1.dup() )
                .appendChild( new TextNode("45") ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);

        xText.insertTextContent(xEnd, xField2, false);

        root = new TreeNode()
            .appendChild( new TextNode("12") )
            .appendChild( meta.dup()
                .appendChild( field1.dup() )
                .appendChild( new TextNode("45") )
                .appendChild( field2.dup() ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);

        try {
            xText.removeTextContent(null);
            assure("removeTextContent(): null content allowed?", false);
        } catch (RuntimeException e) { /* expected */ }

        xText.removeTextContent(xField1);

        XTextRange xAnchor = xMeta.getAnchor();
        assure("getAnchor(): null", xAnchor != null);

        // evil test case: insert ruby around meta
        RubyNode ruby = new RubyNode( mkName("ruby") );
        inserter.insertRange( new Range(2, 6, ruby) );

        /* prevent caching...
        root = new TreeNode()
            .appendChild( new TextNode("12") )
            .appendChild( ruby.dup()
                .appendChild( meta.dup()
                    .appendChild( new TextNode("45") )
                    .appendChild( field2.dup() ) ) )
            .appendChild( new TextNode("6789") );
        doTest(root, false);
        */

        XEnumerationAccess xEA = (XEnumerationAccess)
            UnoRuntime.queryInterface(XEnumerationAccess.class, xMeta);
        XEnumeration xEnum = xEA.createEnumeration();
        assure("createEnumeration(): returns null", xEnum != null);
        {
            assure("hasNext(): first missing", xEnum.hasMoreElements());
            Object xElement = xEnum.nextElement();
            XTextRange xPortion = (XTextRange)
                UnoRuntime.queryInterface(XTextRange.class, xElement);
            XPropertySet xPropSet = (XPropertySet)
                UnoRuntime.queryInterface(XPropertySet.class, xPortion);
            String type = (String) xPropSet.getPropertyValue("TextPortionType");
            assure("first: not text", type.equals("Text"));
            String txt = xPortion.getString();
            assure("first: text differs: " + txt, "45".equals(txt));
        }
        {
            assure("hasNext(): second missing", xEnum.hasMoreElements());
            Object xElement = xEnum.nextElement();
            XTextRange xPortion = (XTextRange)
                UnoRuntime.queryInterface(XTextRange.class, xElement);
            XPropertySet xPropSet = (XPropertySet)
                UnoRuntime.queryInterface(XPropertySet.class, xPortion);
            String type = (String) xPropSet.getPropertyValue("TextPortionType");
            assure("second: not text", type.equals("TextField"));
        }
        // no ruby end here!!!
        assure("hasNext(): more elements?", !xEnum.hasMoreElements());

        XComponent xComponent = (XComponent)
            UnoRuntime.queryInterface(XComponent.class, xMeta);
        xComponent.dispose();

        try {
            XTextCursor xCursor = xText.createTextCursor();
            assure("createTextCursor(): succeeds on disposed object?",
                    xCursor == null);
        } catch (RuntimeException e) { /* expected */ }
    }

    /** check that cursor move methods move to positions in the meta,
        but do not move to positions outside the meta. */
    public void testMetaXTextCursor() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("Text. 12 More text here.");
        inserter.insertRange( new Range(0, 0, text) );
        MetaNode met1 = new MetaNode( mkId("id") );
        XTextContent xMeta = inserter.makeMeta();

        XText xDocText = m_xDoc.getText();
        XTextCursor xDocTextCursor = xDocText.createTextCursor();
        xDocTextCursor.goRight((short)7, false);
        xDocTextCursor.goRight((short)2, true);
        xDocText.insertTextContent(xDocTextCursor, xMeta, true);
        xDocTextCursor.gotoStart(true);

        XMetadatable xMetadatable = (XMetadatable)
            UnoRuntime.queryInterface(XMetadatable.class, xMeta);
        xMetadatable.setMetadataReference(met1.getXmlId());
        XText xText = (XText) UnoRuntime.queryInterface(XText.class, xMeta);

        XTextRange xStart = xText.getStart();
        assure("getStart(): no start", xStart != null);
        XTextRange xEnd = xText.getEnd();
        assure("getEnd(): no end", xEnd != null);

        XTextCursor xTextCursor = xText.createTextCursor();
        assure("createTextCursor(): no cursor", xTextCursor != null);

        // XTextCursor
        boolean bSuccess = false;
        xTextCursor.gotoStart(false);
        xTextCursor.gotoEnd(false);
        bSuccess = xTextCursor.goLeft((short)1, false);
        assure("goLeft(): failed", bSuccess);
        bSuccess = xTextCursor.goLeft((short)1000, false);
        assure("goLeft(): succeeded", !bSuccess);
        bSuccess = xTextCursor.goRight((short)1, false);
        assure("goRight(): failed", bSuccess);
        bSuccess = xTextCursor.goRight((short)1000, false);
        assure("goRight(): succeeded", !bSuccess);
        xTextCursor.gotoRange(xStart, false);
        xTextCursor.gotoRange(xEnd, false);
        try {
            xTextCursor.gotoRange(xDocTextCursor, false);
            assure("gotoRange(): succeeded", false);
        } catch (RuntimeException e) { /* expected */ }

        // XWordCursor
        xText.setString("Two words");
        xTextCursor.gotoStart(false);
        XWordCursor xWordCursor = (XWordCursor)
            UnoRuntime.queryInterface(XWordCursor.class, xTextCursor);

        bSuccess = xWordCursor.gotoNextWord(true);
        assure("gotoNextWord(): failed", bSuccess);
        {
            String string = xTextCursor.getString();
            assure("gotoNextWord(): wrong string: " + string,
                    "Two ".equals(string));
        }
        bSuccess = xWordCursor.gotoNextWord(false);
        assure("gotoNextWord(): succeeded", !bSuccess);
        xTextCursor.collapseToEnd();
        bSuccess = xWordCursor.gotoPreviousWord(true);
        assure("gotoPreviousWord(): failed", bSuccess);
        {
            String string = xTextCursor.getString();
            assure("gotoPreviousWord(): wrong string: " + string,
                    "words".equals(string));
        }
        bSuccess = xWordCursor.gotoPreviousWord(false);
        assure("gotoPreviousWord(): succeeded", !bSuccess);
        bSuccess = xWordCursor.gotoEndOfWord(true);
        assure("gotoEndOfWord(): failed", bSuccess);
        {
            String string = xTextCursor.getString();
            assure("gotoEndOfWord(): wrong string: " + string,
                    "Two".equals(string));
        }
        xTextCursor.gotoEnd(false);
        bSuccess = xWordCursor.gotoStartOfWord(true);
        assure("gotoStartOfWord(): failed", bSuccess);
        {
            String string = xTextCursor.getString();
            assure("gotoStartOfWord(): wrong string: " + string,
                    "words".equals(string));
        }
        xText.setString("");
        bSuccess = xWordCursor.gotoEndOfWord(false);
        assure("gotoEndOfWord(): succeeded", !bSuccess);
        bSuccess = xWordCursor.gotoStartOfWord(false);
        assure("gotoStartOfWord(): succeeded", !bSuccess);

        // XSentenceCursor
        xText.setString("This is a sentence. Another sentence.");
        xTextCursor.gotoStart(false);
        XSentenceCursor xSentenceCursor = (XSentenceCursor)
            UnoRuntime.queryInterface(XSentenceCursor.class, xTextCursor);

        bSuccess = xSentenceCursor.gotoNextSentence(true);
        assure("gotoNextSentence(): failed", bSuccess);
        {
            String string = xTextCursor.getString();
            assure("gotoNextSentence(): wrong string: " + string,
                    "This is a sentence. ".equals(string));
        }
        bSuccess = xSentenceCursor.gotoNextSentence(false);
        assure("gotoNextSentence(): succeeded", !bSuccess);
        // FIXME:
        // the sentence cursor seems to work differently than the word cursor
        xText.setString("This is a sentence. Another sentence. Sentence 3.");
        xTextCursor.gotoEnd(false);
        bSuccess = xSentenceCursor.gotoPreviousSentence(true);
        assure("gotoPreviousSentence(): failed", bSuccess);
        {
            String string = xTextCursor.getString();
            assure("gotoPreviousSentence(): wrong string: " + string,
                    "Another sentence. Sentence 3.".equals(string));
        }
        bSuccess = xSentenceCursor.gotoPreviousSentence(false);
        assure("gotoPreviousSentence(): succeeded", !bSuccess);
        bSuccess = xSentenceCursor.gotoEndOfSentence(true);
        assure("gotoEndOfSentence(): failed", bSuccess);
        {
            String string = xTextCursor.getString();
            assure("gotoEndOfSentence(): wrong string: " + string,
                    "This is a sentence.".equals(string));
        }
        xTextCursor.gotoEnd(false);
        bSuccess = xSentenceCursor.gotoStartOfSentence(true);
        assure("gotoStartOfSentence(): failed", bSuccess);
        {
            String string = xTextCursor.getString();
            assure("gotoStartOfSentence(): wrong string: " + string,
                    "Sentence 3.".equals(string));
        }
        xText.setString("");
        bSuccess = xSentenceCursor.gotoEndOfSentence(false);
        assure("gotoEndOfSentence(): succeeded", !bSuccess);
        bSuccess = xSentenceCursor.gotoStartOfSentence(false);
        assure("gotoStartOfSentence(): succeeded", !bSuccess);

        XParagraphCursor xParagraphCursor = (XParagraphCursor)
            UnoRuntime.queryInterface(XParagraphCursor.class, xTextCursor);

        // XParagraphCursor (does not make sense)
        bSuccess = xParagraphCursor.gotoNextParagraph(false);
        assure("gotoNextParagraph(): succeeded", !bSuccess);
        bSuccess = xParagraphCursor.gotoPreviousParagraph(false);
        assure("gotoPreviousParagraph(): succeeded", !bSuccess);
        bSuccess = xParagraphCursor.gotoStartOfParagraph(false);
        assure("gotoStartOfParagraph(): succeeded", !bSuccess);
        bSuccess = xParagraphCursor.gotoEndOfParagraph(false);
        assure("gotoEndOfParagraph(): succeeded", !bSuccess);
    }


    abstract class AttachHelper
    {
        abstract boolean isAttribute();
        abstract TreeNode mkTreeNode();
        abstract XTextContent mkTextContent(Inserter inserter, TreeNode node)
            throws Exception;
        void postInserted(TreeNode node, XTextContent xContent)
            throws Exception { }
    }

    public void testMetaXTextAttachToxMark() throws Exception
    {
        doMetaXTextAttach( new AttachHelper()
            {
                boolean isAttribute() { return true; }
                TreeNode mkTreeNode() {
                    return new DocumentIndexMarkNode( mkName("toxmark") );
                }
                XTextContent mkTextContent(Inserter inserter, TreeNode node)
                        throws Exception {
                    return inserter.makeDocumentIndexMark(
                        ((DocumentIndexMarkNode)node).getName());
                }
            });
    }

    public void testMetaXTextAttachRefMark() throws Exception
    {
        doMetaXTextAttach( new AttachHelper()
            {
                boolean isAttribute() { return true; }
                TreeNode mkTreeNode() {
                    return new ReferenceMarkNode( mkName("refmark") );
                }
                XTextContent mkTextContent(Inserter inserter, TreeNode node)
                        throws Exception {
                    return inserter.makeReferenceMark(
                        ((ReferenceMarkNode)node).getName());
                }
            });
    }

    public void testMetaXTextAttachTextField() throws Exception
    {
        doMetaXTextAttach( new AttachHelper()
            {
                boolean isAttribute() { return false; }
                TreeNode mkTreeNode() {
                    return new TextFieldNode( mkName("field") );
                }
                XTextContent mkTextContent(Inserter inserter, TreeNode node)
                        throws Exception {
                    return inserter.makeTextField(
                        ((TextFieldNode)node).getContent());
                }
            });
    }

    public void testMetaXTextAttachFootnote() throws Exception
    {
        doMetaXTextAttach( new AttachHelper()
            {
                boolean isAttribute() { return false; }
                TreeNode mkTreeNode() {
                    return new FootnoteNode( mkName("ftn") );
                }
                XTextContent mkTextContent(Inserter inserter, TreeNode node)
                        throws Exception {
                    return inserter.makeFootnote(
                        ((FootnoteNode)node).getLabel());
                }
            });
    }

    public void testMetaXTextAttachMeta() throws Exception
    {
        doMetaXTextAttach( new AttachHelper()
            {
                boolean isAttribute() { return true; }
                TreeNode mkTreeNode() {
                    return new MetaNode( mkId("id") );
                }
                XTextContent mkTextContent(Inserter inserter, TreeNode node)
                        throws Exception {
                    return inserter.makeMeta();
                }
                void postInserted(TreeNode node, XTextContent xContent)
                        throws Exception {
                    XMetadatable xMetadatable = (XMetadatable)
                        UnoRuntime.queryInterface(XMetadatable.class, xContent);
                    xMetadatable.setMetadataReference(
                            ((MetaNode)node).getXmlId());
                }
            });
    }

    void doMetaXTextAttach(AttachHelper helper) throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("12AB6789");
        inserter.insertRange( new Range(0, 0, text) );
        MetaNode met1 = new MetaNode( mkId("id") );
        XTextContent xMeta = inserter.makeMeta();

        XText xDocText = m_xDoc.getText();
        XTextCursor xDocTextCursor = xDocText.createTextCursor();
        xDocTextCursor.goRight((short)3, false);
        xDocTextCursor.goRight((short)2, true);
        xDocText.insertTextContent(xDocTextCursor, xMeta, true);

        XMetadatable xMetadatable = (XMetadatable)
            UnoRuntime.queryInterface(XMetadatable.class, xMeta);
        xMetadatable.setMetadataReference(met1.getXmlId());
        XText xText = (XText) UnoRuntime.queryInterface(XText.class, xMeta);
        XTextRange xStart = null;
        XTextRange xEnd = null;

        {
            xStart = xText.getStart();
            xEnd = xText.getEnd();

            TreeNode nod1 = helper.mkTreeNode();
            TreeNode nod2 = helper.mkTreeNode();
            XTextContent xContent1 = helper.mkTextContent(inserter, nod1);
            XTextContent xContent2 = helper.mkTextContent(inserter, nod2);

            // insertTextContent with meta getStart()/getEnd()
            xText.insertTextContent(xStart, xContent1, false);
            xText.insertTextContent(xEnd  , xContent2, false);

            helper.postInserted(nod1, xContent1);
            helper.postInserted(nod2, xContent2);

            TreeNode root = new TreeNode()
                .appendChild( new TextNode("12") )
                .appendChild( met1.dup()
                    .appendChild( nod1.dup() )
                    .appendChild( new TextNode("AB") )
                    .appendChild( nod2.dup() ) )
                .appendChild( new TextNode("6789") );
            doTest(root, false);
        }
        {
            xText.setString("AB");
            xStart = xText.getStart();
            xEnd = xText.getEnd();

            TreeNode nod1 = helper.mkTreeNode();
            TreeNode nod2 = helper.mkTreeNode();
            XTextContent xContent1 = helper.mkTextContent(inserter, nod1);
            XTextContent xContent2 = helper.mkTextContent(inserter, nod2);

            XTextCursor xTextCursor = xText.createTextCursor();
            xTextCursor.gotoStart(false);

            // insertTextContent with meta cursor
            xText.insertTextContent(xTextCursor, xContent1, false);
            xTextCursor.gotoEnd(false);
            xText.insertTextContent(xTextCursor, xContent2, false);

            helper.postInserted(nod1, xContent1);
            helper.postInserted(nod2, xContent2);

            TreeNode root = new TreeNode()
                .appendChild( new TextNode("12") )
                .appendChild( met1.dup()
                    .appendChild( nod1.dup() )
                    .appendChild( new TextNode("AB") )
                    .appendChild( nod2.dup() ) )
                .appendChild( new TextNode("6789") );
            doTest(root, false);
        }
        if (!helper.isAttribute())
        {
//            xText.setString("AB");
            xStart = xText.getStart();
            xEnd = xText.getEnd();

            TreeNode nod1 = helper.mkTreeNode();
            TreeNode nod2 = helper.mkTreeNode();
            XTextContent xContent1 = helper.mkTextContent(inserter, nod1);
            XTextContent xContent2 = helper.mkTextContent(inserter, nod2);

            XTextCursor xTextCursor = xText.createTextCursor();
            xTextCursor.gotoStart(false);
            xTextCursor.goRight((short)1, true);

            // insertTextContent with meta cursor and absorb
            xText.insertTextContent(xTextCursor, xContent1, true);
            xTextCursor.gotoEnd(false);
            xTextCursor.goLeft((short)1, true);
            xText.insertTextContent(xTextCursor, xContent2, true);

            helper.postInserted(nod1, xContent1);
            helper.postInserted(nod2, xContent2);

            TreeNode root = new TreeNode()
                .appendChild( new TextNode("12") )
                .appendChild( met1.dup()
                    .appendChild( nod1.dup() )
                    .appendChild( new TextNode("AB") )
                    .appendChild( nod2.dup() ) )
                .appendChild( new TextNode("6789") );
            doTest(root, false);
        }
        {
            xText.setString("AB");
            xStart = xText.getStart();
            xEnd = xText.getEnd();

            TreeNode nod1 = helper.mkTreeNode();
            TreeNode nod2 = helper.mkTreeNode();
            XTextContent xContent1 = helper.mkTextContent(inserter, nod1);
            XTextContent xContent2 = helper.mkTextContent(inserter, nod2);

            xDocTextCursor.gotoRange(xStart, false);

            // insertTextContent with document cursor
            xText.insertTextContent(xDocTextCursor, xContent1, false);
            xDocTextCursor.gotoRange(xEnd, false);
            xText.insertTextContent(xDocTextCursor, xContent2, false);

            helper.postInserted(nod1, xContent1);
            helper.postInserted(nod2, xContent2);

            TreeNode root = new TreeNode()
                .appendChild( new TextNode("12") )
                .appendChild( met1.dup()
                    .appendChild( nod1.dup() )
                    .appendChild( new TextNode("AB") )
                    .appendChild( nod2.dup() ) )
                .appendChild( new TextNode("6789") );
            doTest(root, false);
        }
        if (!helper.isAttribute())
        {
//            xText.setString("AB");
            xStart = xText.getStart();
            xEnd = xText.getEnd();

            TreeNode nod1 = helper.mkTreeNode();
            TreeNode nod2 = helper.mkTreeNode();
            XTextContent xContent1 = helper.mkTextContent(inserter, nod1);
            XTextContent xContent2 = helper.mkTextContent(inserter, nod2);

            xDocTextCursor.gotoRange(xStart, false);
            xDocTextCursor.goRight((short)1, true);

            // insertTextContent with document cursor and absorb
            xText.insertTextContent(xDocTextCursor, xContent1, true);
            xDocTextCursor.gotoRange(xEnd, false);
            xDocTextCursor.goLeft((short)1, true);
            xText.insertTextContent(xDocTextCursor, xContent2, true);

            helper.postInserted(nod1, xContent1);
            helper.postInserted(nod2, xContent2);

            TreeNode root = new TreeNode()
                .appendChild( new TextNode("12") )
                .appendChild( met1.dup()
                    .appendChild( nod1.dup() )
                    .appendChild( new TextNode("AB") )
                    .appendChild( nod2.dup() ) )
                .appendChild( new TextNode("6789") );
            doTest(root, false);
        }
        {
            xText.setString("AB");
            xStart = xText.getStart();
            xEnd = xText.getEnd();

            TreeNode nod1 = helper.mkTreeNode();
            TreeNode nod2 = helper.mkTreeNode();
            XTextContent xContent1 = helper.mkTextContent(inserter, nod1);
            XTextContent xContent2 = helper.mkTextContent(inserter, nod2);

            // attach to range from meta getStart()/getEnd()
            xContent1.attach(xStart);
            xContent2.attach(xEnd);

            helper.postInserted(nod1, xContent1);
            helper.postInserted(nod2, xContent2);

            TreeNode root = new TreeNode()
                .appendChild( new TextNode("12") )
                .appendChild( met1.dup()
                    .appendChild( nod1.dup() )
                    .appendChild( new TextNode("AB") )
                    .appendChild( nod2.dup() ) )
                .appendChild( new TextNode("6789") );
            doTest(root, false);
        }
        {
            xText.setString("AB");
            xStart = xText.getStart();
            xEnd = xText.getEnd();

            TreeNode nod1 = helper.mkTreeNode();
            TreeNode nod2 = helper.mkTreeNode();
            XTextContent xContent1 = helper.mkTextContent(inserter, nod1);
            XTextContent xContent2 = helper.mkTextContent(inserter, nod2);

            XTextCursor xTextCursor = xText.createTextCursor();
            xTextCursor.gotoStart(false);

            // attach to cursor from meta XText
            xContent1.attach(xTextCursor);
            xTextCursor.gotoEnd(false);
            xContent2.attach(xTextCursor);

            helper.postInserted(nod1, xContent1);
            helper.postInserted(nod2, xContent2);

            TreeNode root = new TreeNode()
                .appendChild( new TextNode("12") )
                .appendChild( met1.dup()
                    .appendChild( nod1.dup() )
                    .appendChild( new TextNode("AB") )
                    .appendChild( nod2.dup() ) )
                .appendChild( new TextNode("6789") );
            doTest(root, false);
        }
    }

    public void testMetaFieldXTextField() throws Exception
    {
        com.sun.star.rdf.XRepositorySupplier xModel =
            (com.sun.star.rdf.XRepositorySupplier) UnoRuntime.queryInterface(
                com.sun.star.rdf.XRepositorySupplier.class, m_xDoc);
        com.sun.star.rdf.XRepository xRepo = xModel.getRDFRepository();
        // for testing just add it to the first graph
        com.sun.star.rdf.XURI[] Graphs = xRepo.getGraphNames();
        com.sun.star.rdf.XNamedGraph xGraph = xRepo.getGraph(Graphs[0]);
        com.sun.star.rdf.XURI xOdfPrefix =
            com.sun.star.rdf.URI.createKnown(m_xContext,
                com.sun.star.rdf.URIs.ODF_PREFIX);
        com.sun.star.rdf.XURI xOdfSuffix =
            com.sun.star.rdf.URI.createKnown(m_xContext,
                com.sun.star.rdf.URIs.ODF_SUFFIX);
        com.sun.star.rdf.XNode xPrefix =
            com.sun.star.rdf.Literal.create(m_xContext, "foo");
        com.sun.star.rdf.XNode xSuffix =
            com.sun.star.rdf.Literal.create(m_xContext, "bar");

        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("abc");
        inserter.insertRange( new Range(0, 0, text) );
        XText xDocText = m_xDoc.getText();
        XTextCursor xDocTextCursor = xDocText.createTextCursor();
        xDocTextCursor.goRight((short)1, false);
        xDocTextCursor.goRight((short)3, true);

        XTextField xMetaField = inserter.makeMetaField();

        xDocText.insertTextContent(xDocTextCursor, xMetaField, true);

        XMetadatable xMetadatable = (XMetadatable)
            UnoRuntime.queryInterface(XMetadatable.class, xMetaField);
        xMetadatable.ensureMetadataReference();

        xGraph.addStatement(xMetadatable, xOdfPrefix, xPrefix);
        xGraph.addStatement(xMetadatable, xOdfSuffix, xSuffix);
        assure("getPresentation(): wrong",
            "fooabcbar".equals(xMetaField.getPresentation(false)));
        inserter.insertRange( new Range(0, 0, text) );
    }

    public void testMetaFieldXPropertySet() throws Exception
    {
        RangeInserter inserter = new RangeInserter(m_xDoc);
        TreeNode text = new TextNode("123");
        inserter.insertRange( new Range(0, 0, text) );
        XText xDocText = m_xDoc.getText();
        XTextCursor xDocTextCursor = xDocText.createTextCursor();
        xDocTextCursor.goRight((short)1, false);
        xDocTextCursor.goRight((short)3, true);

        XTextField xMetaField = inserter.makeMetaField();

        xDocText.insertTextContent(xDocTextCursor, xMetaField, true);

        XPropertySet xPropertySet = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, xMetaField);
        assure("PropertySet: not supported?", xPropertySet != null);
        XPropertySetInfo xPropertySetInfo = xPropertySet.getPropertySetInfo();
        assure("hasPropertyByName(\"NumberFormat\"):",
                xPropertySetInfo.hasPropertyByName("NumberFormat"));
        assure("hasPropertyByName(\"IsFixedLanguage\"):",
                xPropertySetInfo.hasPropertyByName("IsFixedLanguage"));

        int def = (Integer) xPropertySet.getPropertyValue("NumberFormat");
        log.println("NumberFormat: default is " + def);
        short INT = com.sun.star.i18n.NumberFormatIndex.NUMBER_INT;
        xPropertySet.setPropertyValue("NumberFormat", INT);
        xPropertySet.setPropertyValue("IsFixedLanguage", true);
        int format = (Integer) xPropertySet.getPropertyValue("NumberFormat");
        assure("NumberFormat: failed", format == INT);
        boolean isFixed = (Boolean)
            xPropertySet.getPropertyValue("IsFixedLanguage");
        assure("IsFixedLanguage: failed", isFixed);
    }

    public void testLoadStore() throws Exception
    {
        XComponent xComp = null;
        String filename = "TESTMETA.odt";
        String file;
        try {
            file = util.utils.getFullTestURL(filename);
            xComp = doLoad(file);
            if (xComp != null)
            {
                file = m_TmpDir + filename;
                doStore(xComp, file);
                close(xComp);
                xComp = doLoad(file);
            }
        } finally {
            close(xComp);
        }
    }

    private void doStore(XComponent xComp, String file) throws Exception
    {
        log.println("Storing test document...");

        XStorable xStor = (XStorable) UnoRuntime.queryInterface(
                    XStorable.class, xComp);

        xStor.storeToURL(file, new PropertyValue[0]);

        log.println("...done");
    }

    public XComponent doLoad(String file) throws Exception
    {
        XComponent xComp = null;

        log.println("Loading test document...");

        PropertyValue[] loadProps = new PropertyValue[1];
        loadProps[0] = new PropertyValue();
        loadProps[0].Name = "Hidden";
        loadProps[0].Value = new Boolean(true);

        xComp = util.DesktopTools.loadDoc(m_xMSF, file, loadProps);
//        xComp =  util.DesktopTools.getCLoader(m_xMSF).loadComponentFromURL(file, "_blank", 0, loadProps);


        XTextDocument xTextDoc = (XTextDocument)
            UnoRuntime.queryInterface(XTextDocument.class, xComp);

        XText xText = xTextDoc.getText();

        log.println("...done");

        log.println("Checking meta(-field)s in loaded test document...");

        TreeNode root = new TreeNode()
            .appendChild( new RubyNode("ruby1")
                .appendChild( new TextNode("1") ) )
            .appendChild( new MetaNode(mkId_("id1"))
                .appendChild( new TextNode("2") ) )
            .appendChild( new MetaFieldNode(mkId_("id2"))
                .appendChild( new TextNode("3") ) )
            .appendChild( new RubyNode("ruby2")
                .appendChild( new MetaNode(mkId_("id3"))
                    .appendChild( new TextNode("4") ) ) )
            .appendChild( new RubyNode("ruby3")
                .appendChild( new MetaFieldNode(mkId_("id4"))
                    .appendChild( new TextNode("5") ) ) )
            .appendChild( new MetaNode(mkId_("id5"))
                .appendChild( new RubyNode("ruby4")
                    .appendChild( new TextNode("6") ) ) )
            .appendChild( new MetaFieldNode(mkId_("id6"))
                .appendChild( new RubyNode("ruby5")
                    .appendChild( new TextNode("7") ) ) )
            .appendChild( new MetaNode(mkId_("id7"))
                .appendChild( new MetaNode(mkId_("id8"))
                    .appendChild( new TextNode("8") ) ) )
            .appendChild( new MetaNode(mkId_("id9"))
                .appendChild( new MetaFieldNode(mkId_("id10"))
                    .appendChild( new TextNode("9") ) ) )
            .appendChild( new MetaFieldNode(mkId_("id11"))
                .appendChild( new MetaNode(mkId_("id12"))
                    .appendChild( new TextNode("10") ) ) )
            .appendChild( new MetaFieldNode(mkId_("id13"))
                .appendChild( new MetaFieldNode(mkId_("id14"))
                    .appendChild( new TextNode("11") ) ) )
            .appendChild( new MetaNode(mkId_("id15"))
                .appendChild( new RubyNode("ruby6")
                    .appendChild( new MetaFieldNode(mkId_("id16"))
                        .appendChild( new TextNode("12") ) ) ) )
            .appendChild( new MetaNode(mkId_("")) {
                                public boolean equals(Object other) {
                                    return (other instanceof MetaNode);
                                } }
                .appendChild( new TextNode("13") ) )
            .appendChild( new TextNode(" X X ") );
        doTest(xTextDoc, root, false);

        log.println("...done");

        return xComp;
    }

    static void close(XComponent i_comp)
    {
        try {
            XCloseable xClos = (XCloseable) UnoRuntime.queryInterface(
                        XCloseable.class, i_comp);
            if (xClos != null) xClos.close(true);
        } catch (Exception e) {
        }
    }

    private void doTest(TreeNode intree) throws Exception
    {
        doTest(m_xDoc, intree, true);
    }

    private void doTest(TreeNode intree, boolean insert) throws Exception
    {
        doTest(m_xDoc, intree, insert);
    }

    private void doTest(XTextDocument xDoc, TreeNode intree,
            boolean insert) throws Exception
    {
        dumpTree(intree, "I: ");

        if (insert) {
            new TreeInserter(xDoc).insertTree(intree);
        }

//Thread.sleep(10000);

        XText xText = xDoc.getText();
        XEnumerationAccess xTextEA = (XEnumerationAccess)
            UnoRuntime.queryInterface(XEnumerationAccess.class, xText);
        XEnumeration xTextEnum = xTextEA.createEnumeration();
        // skip to right paragraph
        xTextEnum.nextElement(); // skip first -- always empty!
        Object xElement = xTextEnum.nextElement(); // second contains test case
        XEnumerationAccess xEA = (XEnumerationAccess)
            UnoRuntime.queryInterface(XEnumerationAccess.class, xElement);
        XEnumeration xEnum = xEA.createEnumeration();
        TreeNode outtree = new EnumConverter(this).convert(xEnum);

        dumpTree(outtree, "O: ");

        boolean success = new FuzzyTester(log).doTest(intree, outtree);
        assure("test failed", success);
    }

    private void dumpTree(TreeNode tree) { dumpTree(tree, "> "); }

    private void dumpTree(TreeNode tree, String prefix)
    {
        log.println(prefix + tree.toString());
        TreeNodeEnum children = tree.createEnumeration();
        while (children.hasNext()) {
            TreeNode node = children.next();
            dumpTree(node, prefix + "  ");
        }
    }

    private String mkName(String prefix)
    {
        return prefix + String.valueOf(m_Count++);
    }

    private StringPair mkId(String prefix)
    {
        return new StringPair("content.xml", mkName(prefix));
    }

    private StringPair mkId_(String id)
    {
        return new StringPair("content.xml", id);
    }

    public void assure(String str, boolean cond) { super.assure(str, cond); }
}

