/*****************************************************************************
 * Filename    : MBeanInfoRegistry.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.management.jmx;

import org.openamq.AMQException;
import org.openamq.schema.cml.CmlDocument;
import org.openamq.schema.cml.FieldDocument;
import org.openamq.schema.cml.SchemaReplyDocument;

import javax.management.openmbean.*;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Stores all OpenMBeanInfo instances.
 * <p/>
 * Builds MBeanInfo instances from the CML schema (which is parsed by XMLBeans) and
 * stores these indexed by CML class name.
 * <p/>
 * When constructing a DynamicMBean this registry is consulted for the MBeanInfo.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class MBeanInfoRegistry
{
    private Map<String, OpenMBeanInfoSupport> _cmlClass2OpenMBeanInfoMap = new HashMap<String, OpenMBeanInfoSupport>();

    public MBeanInfoRegistry(CmlDocument cmlDocument) throws AMQException
    {
        initialise(cmlDocument);
    }

    private void initialise(CmlDocument cmlDocument) throws AMQException
    {
        SchemaReplyDocument.SchemaReply schema = cmlDocument.getCml().getSchemaReply();
        for (org.openamq.schema.cml.ClassDocument.Class c : schema.getClass1List())
        {
            OpenMBeanAttributeInfo[] attributes = createAttributeInfos(c.getFieldList());
            String className = c.getName();
            OpenMBeanInfoSupport support = new OpenMBeanInfoSupport(className, null, attributes,
                                                                    null, null, null, null);
            _cmlClass2OpenMBeanInfoMap.put(className, support);
        }
    }

    public OpenMBeanInfoSupport getOpenMBeanInfo(String cmlType)
    {
        return _cmlClass2OpenMBeanInfoMap.get(cmlType);
    }

    private OpenMBeanAttributeInfo[] createAttributeInfos(List<FieldDocument.Field> fields)
            throws AMQException
    {
        OpenMBeanAttributeInfo[] attributes = new OpenMBeanAttributeInfo[fields.size()];
        for (int i = 0; i < attributes.length; i++)
        {
            FieldDocument.Field field = fields.get(i);
            OpenType openType = getOpenType(field.getType(), field.getModify());
            attributes[i] = new OpenMBeanAttributeInfoSupport(field.getName(),
                                                              field.getLabel(),
                                                              openType,
                                                              true,
                                                              field.getModify(),
                                                              openType == SimpleType.BOOLEAN);
        }
        return attributes;
    }

    private static OpenType getOpenType(FieldDocument.Field.Type.Enum type, boolean isArray)
            throws UnsupportedCMLTypeException, AMQException
    {
        SimpleType simpleType;
        boolean primitive;
        switch (type.intValue())
        {
            // the constants are not public (bug in xmlbeans) so we cannot use
            // the constants that are defined
            // TODO: raise defect with xmlbeans projects
            case 1:
                simpleType = SimpleType.BOOLEAN;
                primitive = true;
                break;
            case 2:
                simpleType = SimpleType.STRING;
                primitive = false;
                break;
            case 3:
                simpleType = SimpleType.INTEGER;
                primitive = true;
                break;
            case 4:
                simpleType = SimpleType.OBJECTNAME;
                primitive = false;
                break;
            default:
                throw new UnsupportedCMLTypeException(type.toString());
        }
        if (isArray)
        {
            try
            {
                return new ArrayType(simpleType, primitive);
            }
            catch (OpenDataException e)
            {
                throw new AMQException("Error constructing array type: " + e, e);
            }
        }
        else
        {
            return simpleType;
        }
    }
}

