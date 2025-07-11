/*
 *  @author Jose Simo. 
 *  (c) ai2-UPV Creative Commons.
 *  Rev: 2022
 */
package comm;

import java.io.Serializable;

import java.util.Properties;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.ObjectMessage;
import javax.jms.Topic;
import javax.jms.TopicConnection;
import javax.jms.TopicConnectionFactory;
import javax.jms.TopicPublisher;
import javax.jms.TopicSession;
import javax.jms.TopicSubscriber;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;




public class DifusionJMS implements javax.jms.MessageListener, Difusion {

    Context context = null;
    TopicConnectionFactory factory = null;
    TopicConnection connection = null;
    String factoryName = "ConnectionFactory";
    String destName = "";
    Topic dest = null;
    TopicSession sessionSuscriber = null;
    TopicSession sessionPublisher = null;
    TopicSubscriber subscriber = null;
    TopicPublisher publisher = null;
    //
    String username ="EsteEsMiNombre";
    int count =0;
    
    // Observe que esta variable es la que usamos para implementar 
    // la cola de contencion de mensajes de tamanyo uno.
    Object currentItem = null;
    //
	public DifusionJMS(String ip, int port) {
		
		// Observe que el nombre del "Topic" dinamico corresponde al formato 
		// "IP_Port" (xxx.xxx.xxx.xxx_pppp). Con esto se consige compatibilidad 
		// entre Multicast y JMS en la identificacion del canal de difusion.
		destName = "dynamicTopics/"+ip+"_"+port;
		//
        try {
        	
            // create the JNDI initial context
            Properties env = new Properties( );
            env.put(Context.INITIAL_CONTEXT_FACTORY, "org.apache.activemq.jndi.ActiveMQInitialContextFactory");
            env.put(Context.PROVIDER_URL, "tcp://localhost:61616");
            //
            context = new InitialContext(env);
            //  
            /*********************************************************/
            /**** EJERCICIO 01: Preparar la infraestructura JMS ******/
            // EJ01.a: obtener la factoria a partir del contexto.
            factory = (TopicConnectionFactory) context.lookup(factoryName);
            
            // EJ01.b: obtener el "Topic" destino a partir de su nombre.
            dest = (Topic) context.lookup(destName);
            
            // EJ01.c: crear una conexion usando la factoria
            connection = factory.createTopicConnection();
            
            // Establece un identificador de cliente �nico para la conexion.
            connection.setClientID(username+Math.random()); 
            
            // EJ01.d: crear las sesiones de publicacion y de suscripcion.
            sessionSuscriber = connection.createTopicSession(false, javax.jms.Session.AUTO_ACKNOWLEDGE);
            sessionPublisher = connection.createTopicSession(false, javax.jms.Session.AUTO_ACKNOWLEDGE);
            
            // EJ01.e: crear el suscriptor a partir de la sesion de suscripcion. 
            subscriber = sessionSuscriber.createSubscriber(dest);
            
            // Define este objeto (this) como "listener" del suscriptor.
            subscriber.setMessageListener(this);     
            
            // EJ01.f: crear el publicador a partir de la sesion de publicacion.
            publisher = sessionPublisher.createPublisher(dest);
            
            // Arranca la conexion para poder recibir mensajes.
            connection.start();
            /*********************************************************/
            
        } catch (JMSException exception) {
            exception.printStackTrace();
        } catch (NamingException exception) {
            exception.printStackTrace();
        }
	}
	
	@Override
	public synchronized void onMessage(Message msg) {
		
        /***********************************************************/
        /**** EJERCICIO 02: recibir el mensaje JMS y almacenarlo ***/
		// Cast del mensaje
		
		// EJ02.a: extraer el objeto del mensaje y almacenarlo en "currentItem"
		ObjectMessage objMsg = (ObjectMessage) msg;
		try {
			
			currentItem = objMsg.getObject();
			notifyAll();
		} catch (JMSException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// EJ02.b: despertar a todos los hilos suspendidos a la espera 
		// de la llegada de un mensaje.
		
	}
	
	public synchronized Object receiveObject() {
		
        /***********************************************************/
        /**** EJERCICIO 03: implementar la recepcion bloqueante ***/
		Object obj = null;
		// EJ03.a mientras "currentItem" sea "null" suspender el hilo.
		// 
		while(currentItem == null) {
			try {
				wait();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		// Ya tenemos un mensaje, lo copiamos para retornarlo y ponemos "currentItem" 
		// a "null" para indicar que el mensaje ya ha sido consumido.
		obj = currentItem;
		currentItem = null;
		return obj;
	}
	
	
	public synchronized void sendObject(Object obj) {	
		/////////////////////
		// El envio de mensajes esta totalmente implementado.
		try {
			ObjectMessage m=(ObjectMessage)sessionPublisher.createObjectMessage((Serializable)obj);
			publisher.publish(m,
					javax.jms.DeliveryMode.PERSISTENT, 
					javax.jms.Message.DEFAULT_PRIORITY, 
					2000);
		} catch (JMSException e) {
			e.printStackTrace();
		}
	}    
}
