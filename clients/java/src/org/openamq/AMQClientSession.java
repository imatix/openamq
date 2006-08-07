/*---------------------------------------------------------------------------
    AMQClientSession.java

    Copyright (c) 2005-2006 iMatix Corporation
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.
    
    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
    
    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 *---------------------------------------------------------------------------*/

package org.openamq;

import org.openamq.protocol.AMQClientSessionI;
import org.openamq.framing.*;

/**
 * An AMQSession object allows sending and receiving messages.
 *
 * By sending AMQP methods to the broker, a Session object allows to configure
 * common or tailored routing topologies. For each method supported, a pair of
 * functions exist; one allows for full method configuration, the other uses
 * defaults convenient for simple publish/subscribe and request/reply scenarios.
 *  
 * Immediately after the AMQConnection object returns a new session, the channel
 * methods and the getMessage function can be called.
 */
public class AMQClientSession extends AMQClientSessionI {

    /**
     * Stops or allows message flow on the channel.
     *
     * @param active when true allows message flow on the channel.
     */
    public void channelFlow(boolean active) throws Exception {
        writeFrame(ChannelFlowBody.createAMQFrame(getSessionId(), active));
    }

    /**
     * Requests access on a given realm with default parameters.
     *
     * @param realm realm to request access to.
     */
    public void accessRequest(String realm) throws Exception {
        accessRequest(realm, false, false, true, true, true);
    }

    /**
     * Requests access on a given realm with the given parameters.
     *
     * @param realm realm to request access to.
     * @param exclusive request exclusive access.
     * @param passive do nothing if already granted.
     * @param active when true allows message flow on the channel.
     * @param write request sending to the realm.
     * @param read request receiving from the realm.
     */
    public void accessRequest(String realm, boolean exclusive, boolean passive, boolean active,
        boolean write, boolean read) throws Exception {

        writeFrame(AccessRequestBody.createAMQFrame(getSessionId(), realm, exclusive, passive,
            active, write, read));
    }

    /**
     * Requests exchange creation with default parameters.
     *
     * @param exchange exchange name.
     * @param type exchange routing type.
     */
    public void exchangeDeclare(String exchange, String type) throws Exception {
        exchangeDeclare(exchange, type, false, true, false, false, false, null);
    }

    /**
     * Requests exchange creation with the given parameters.
     *
     * @param exchange exchange name.
     * @param type exchange routing type.
     * @param passive do nothing if already granted.
     * @param durable make best effort to preserve messages on broker failure.
     * @param autoDelete delete when no longer in use.
     * @param internal exchange created for administration purposes.
     * @param nowait do not wait for confirmation.
     * @param arguments arguments for exchange type.
     */
    public void exchangeDeclare(String exchange, String type, boolean passive, boolean durable, boolean autoDelete,
        boolean internal, boolean nowait, FieldTable arguments) throws Exception {

        writeFrame(ExchangeDeclareBody.createAMQFrame(getSessionId(), 0, exchange, type, passive, durable, autoDelete,
            internal, nowait, arguments));
    }

    /**
     * Requests exchange destruction with default parameters.
     *
     * @param exchange exchange name.
     */
    public void exchangeDelete(String exchange) throws Exception {
        exchangeDelete(exchange, false, false);
    }

    /**
     * Requests exchange destruction with the given parameters.
     *
     * @param exchange exchange name.
     * @param ifUnused delete only if unused.
     * @param nowait do not wait for confirmation.
     */
    public void exchangeDelete(String exchange, boolean ifUnused, boolean nowait) throws Exception {
        writeFrame(ExchangeDeleteBody.createAMQFrame(getSessionId(), 0, exchange, ifUnused, nowait));
    }

    /**
     * Requests queue creation with default parameters.
     *
     * @param queue queue name.
     */
    public void queueDeclare(String queue) throws Exception {
        queueDeclare(queue, false, true, false, false, false);
    }

    /**
     * Requests queue creation with the given parameters.
     *
     * @param queue queue name.
     * @param passive do nothing if already granted.
     * @param durable make best effort to preserve messages on broker failure.
     * @param exclusive request exclusive access.
     * @param autoDelete delete when no longer in use.
     * @param nowait do not wait for confirmation.
     */
    public void queueDeclare(String queue, boolean passive, boolean durable, boolean exclusive, boolean autoDelete,
        boolean nowait) throws Exception {

        writeFrame(QueueDeclareBody.createAMQFrame(getSessionId(), 0, queue, passive, durable,
            exclusive, autoDelete, nowait, null));
    }

    /**
     * Requests queue to exchange binding with default parameters.
     *
     * @param queue queue name.
     * @param routingKey receive messages published with this routing key.
     */
    public void queueBind(String queue, String routingKey) throws Exception {
        queueBind(queue, "", routingKey, false, null);
    }

    /**
     * Requests queue to exchange binding with the given parameters.
     *
     * @param queue queue name.
     * @param exchange exchange name.
     * @param routingKey receive messages published with this routing key.
     */
    public void queueBind(String queue, String exchange, String routingKey) throws Exception {
        queueBind(queue, exchange, routingKey, false, null);
    }

    /**
     * Requests queue to exchange binding with the given parameters.
     *
     * @param queue queue name.
     * @param exchange exchange name.
     * @param routingKey receive messages published with this routing key.
     * @param nowait do not wait for confirmation.
     * @param arguments binding arguments for the exchange.
     */
    public void queueBind(String queue, String exchange, String routingKey, boolean nowait, FieldTable arguments) throws Exception {
        writeFrame(QueueBindBody.createAMQFrame(getSessionId(), 0, queue, exchange, routingKey, nowait, arguments));
    }

    /**
     * Requests queue message purge with default parameters.
     *
     * @param queue queue name.
     */
    public void queuePurge(String queue) throws Exception {
        queuePurge(queue, false);
    }

    /**
     * Requests queue message purge with the given parameters.
     *
     * @param queue queue name.
     * @param nowait do not wait for confirmation.
     */
    public void queuePurge(String queue, boolean nowait) throws Exception {
        writeFrame(QueuePurgeBody.createAMQFrame(getSessionId(), 0, queue, nowait));
    }

    /**
     * Requests queue destruction with default parameters.
     *
     * @param queue queue name.
     */
    public void queueDelete(String queue) throws Exception {
        queueDelete(queue, false, false, false);
    }

    /**
     * Requests queue destruction with the given parameters.
     *
     * @param queue queue name.
     * @param ifUnused delete only if unused.
     * @param ifEmpty delete only if empty.
     * @param nowait do not wait for confirmation.
     */
    public void queueDelete(String queue, boolean ifUnused, boolean ifEmpty, boolean nowait) throws Exception {
        writeFrame(QueueDeleteBody.createAMQFrame(getSessionId(), 0, queue, ifUnused, ifEmpty, nowait));
    }

    /**
     * Requests start consuming messages with default parameters.
     *
     * @param queue queue name.
     */
    public void basicConsume(String queue) throws Exception {
        basicConsume(queue, queue, false, true, false, false);
    }

    /**
     * Requests start consuming messages with the given parameters.
     *
     * @param queue queue name.
     * @param consumerTag tag for matching incoming messages to a specific consume request.
     * @param noLocal do not receive matching messages which originated from this client.
     * @param noAck the client will not acknowledge messages, the server will not redeliver messages.
     * @param exclusive request exclusive access.
     * @param nowait do not wait for confirmation.
     */
    public void basicConsume(String queue, String consumerTag, boolean noLocal, boolean noAck, boolean exclusive,
        boolean nowait) throws Exception {

        writeFrame(BasicConsumeBody.createAMQFrame(getSessionId(), 0, queue, consumerTag, noLocal, noAck, exclusive, nowait));
    }

    /**
     * Cancel a specific consume request with default parameters.
     *
     * @param consumerTag tag used on consume request.
     */
    public void basicCancel(String consumerTag) throws Exception {
        basicCancel(consumerTag, false);
    }

    /**
     * Cancel a specific consume request with the given parameters.
     *
     * @param consumerTag tag used on consume request.
     * @param nowait do not wait for confirmation.
     */
    public void basicCancel(String consumerTag, boolean nowait) throws Exception {
        writeFrame(BasicCancelBody.createAMQFrame(getSessionId(), consumerTag, nowait));
    }

    /**
     * Peek queue messages with default parameters.
     *
     * @param queue queue name.
     */
    public void basicGet(String queue) throws Exception {
        basicGet(queue, true);
    }

    /**
     * Peek queue messages with the given parameters.
     *
     * @param queue queue name.
     * @param noAck the client will not acknowledge messages, the server will not redeliver messages.
     */
    public void basicGet(String queue, boolean noAck) throws Exception {
        writeFrame(BasicGetBody.createAMQFrame(getSessionId(), 0, queue, noAck));
    }

    /**
     * Acknowledge messages with default parameters.
     *
     * @param deliveryTag tag of received message.
     */
    public void basicAck(long deliveryTag) throws Exception {
        basicAck(deliveryTag, true);
    }

    /**
     * Acknowledge messages with the given parameters.
     *
     * @param deliveryTag tag of received message.
     * @param multiple acknowledge all messages up to deliveryTag, inclusive.
     */
    public void basicAck(long deliveryTag, boolean multiple) throws Exception {
        writeFrame(BasicAckBody.createAMQFrame(getSessionId(), deliveryTag, multiple));
    }

    /**
     * Reject message with default parameters.
     *
     * @param deliveryTag tag of received message.
     */
    public void basicRejectBody(long deliveryTag) throws Exception {
        basicAck(deliveryTag, false);
    }

    /**
     * Reject message with the given parameters.
     *
     * @param deliveryTag tag of received message.
     * @param requeue requeue message.
     */
    public void basicRejectBody(long deliveryTag, boolean requeue) throws Exception {
        writeFrame(BasicRejectBody.createAMQFrame(getSessionId(), deliveryTag, requeue));
    }

    public void basicPublish(AMQMessage message, String routingKey) throws Exception {
        basicPublish(message, "", routingKey, false, false);
    }

    public void basicPublish(AMQMessage message, String exchange, String routingKey) throws Exception {
        basicPublish(message, exchange, routingKey, false, false);
    }

    /**
     * Publish message with the given parameters.
     *
     * @param message message to publish.
     * @param exchange exchange to publish to.
     * @param routingKey routing key to be matched by consumers.
     * @param mandatory keep message if no consumers received.
     * @param immediate return message if no consumers received.
     */
    public void basicPublish(AMQMessage message, String exchange, String routingKey,
        boolean mandatory, boolean immediate) throws Exception {

        writeFrame(super.preparePublish(message, exchange, routingKey, mandatory, immediate));
    }

   /**
    * Gets a message.
    *
    * @return received message.
    */
    public AMQMessage getMessage() throws Exception {
        return super.getMessage();
    }

   /**
    * Creates a message.
    *
    * @return A newly created message.
    */
    public AMQMessage createMessage() {
        return super.createMessage();
    }

    /**
     * Close this session.
     *
     * After a session is closed with this method (or by the server), further
     * attempts to use methods or get messages will result in exceptions thrown.
     * @exception Exception if an error occured while closing the session.
     */
    public void close() throws Exception {
        super.close(AMQConstant.REPLY_SUCCESS, "Client closing session.", 0, 0);
    }
}
 
