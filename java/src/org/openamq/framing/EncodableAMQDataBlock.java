package org.openamq.framing;

/**
 * Marker interface to indicate to MINA that a data block should be encoded with the
 * single encoder/decoder that we have defined.
 *
 * Note that due to a bug in MINA all classes must directly implement this interface, even if
 * a superclass implements it.
 * TODO: fix MINA so that this is not necessary
 * 
 */
public interface EncodableAMQDataBlock
{

}
