/*************************************************************************
 *
 *  $RCSfile: CheckIndeterminateState.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Date: 2007-07-24 13:10:56 $
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
package complex.indeterminateState;

import complexlib.ComplexTestCase;
import helper.OfficeProvider;
import util.SOfficeFactory;
import util.AccessibilityTools;
import com.sun.star.awt.XWindow;
import com.sun.star.awt.XExtendedToolkit;
import com.sun.star.frame.XDispatch;
import com.sun.star.frame.XController;
import com.sun.star.frame.XDesktop;
import com.sun.star.frame.XModel;
import com.sun.star.text.XTextDocument;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.accessibility.AccessibleRole;
import com.sun.star.accessibility.XAccessible;
import com.sun.star.accessibility.XAccessibleValue;
import com.sun.star.accessibility.XAccessibleContext;
import com.sun.star.accessibility.XAccessibleStateSet;
import com.sun.star.accessibility.AccessibleStateType;
import com.sun.star.accessibility.XAccessibleAction;
import com.sun.star.awt.XTopWindow;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.frame.XDispatchProvider;
import com.sun.star.util.URL;
import com.sun.star.util.XURLTransformer;

import java.io.PrintWriter;

/**
 */
public class CheckIndeterminateState extends ComplexTestCase {

    /**
     * Return all test methods.
     * @return The test methods.
     */
     public String[] getTestMethodNames() {
        return new String[]{"checkToolBoxItem"};
    }

    /*
     * Test the indeterminate state of AccessibleToolBarItem
     * The used tools are in project qadevOOo/runner
     */
    public void checkToolBoxItem() {
        log.println( "creating a test environment" );
        XTextDocument xTextDoc = null;
        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory((XMultiServiceFactory) param.getMSF());

        try {
            log.println( "creating a text document" );
            xTextDoc = SOF.createTextDoc(null);
        } catch ( com.sun.star.uno.Exception e ) {
            // Some exception occures.FAILED
            e.printStackTrace( (java.io.PrintWriter)log );
            failed (e.getMessage());
        }

        XModel aModel = (XModel)
                    UnoRuntime.queryInterface(XModel.class, xTextDoc);

        XController xController = aModel.getCurrentController();

        XInterface oObj = null;

        System.out.println("Press any key after making 'Bold' indeterminate.");
        try{
            byte[]b = new byte[16];
            System.in.read(b);
        } catch (Exception e) {
            e.printStackTrace();
        }

        AccessibilityTools at = new AccessibilityTools();
        XWindow xWindow = at.getCurrentContainerWindow((XMultiServiceFactory)param.getMSF(), aModel);
        XAccessible xRoot = at.getAccessibleObject(xWindow);

        // uncomment to print the whole accessible tree.
//        at.printAccessibleTree((java.io.PrintWriter)log,xRoot);

        oObj = at.getAccessibleObjectForRole(xRoot,
            AccessibleRole.PUSH_BUTTON, "Bold");
        System.out.println("Found a PUSH_BUTTON: " + (oObj != null));

        oObj = at.getAccessibleObjectForRole(xRoot,
            AccessibleRole.TOGGLE_BUTTON, "Bold");
        System.out.println("Found a TOGGLE_BUTTON: " + (oObj != null));

        log.println("ImplementationName: "+ util.utils.getImplName(oObj));

        XAccessibleContext oContext = (XAccessibleContext)
            UnoRuntime.queryInterface(XAccessibleContext.class, oObj);

        XAccessibleStateSet oSet = oContext.getAccessibleStateSet();

        short[]states = oSet.getStates();
        for(int i=0; i<states.length; i++)
            System.out.println("State "+i+": "+states[i]);

        assure("The 'INDETERMINATE' state is not set.",oSet.contains(AccessibleStateType.INDETERMINATE));
    }

}


