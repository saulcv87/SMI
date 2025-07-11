/*
 *  @author Jose Simo. 
 *  (c) ai2-UPV Creative Commons.
 *  Rev: 2022
 */

package comm;

import java.io.*;

import java.net.*;



public class DifusionMulticast implements Difusion{

  MulticastSocket socket;
  String m_ip;
  int m_port;
  public InetSocketAddress group;
  
  public static final int BUFSIZE = 65535;

//------------------------------------------------------------------------------
  public DifusionMulticast(String ip, int port) {
	  
	m_ip = ip;
	m_port = port;
 
	group = new InetSocketAddress(m_ip, m_port); //Obtener la direccion del grupo
	 
	try {
		socket = new MulticastSocket(m_port);  //Crear el socket multicast 
		socket.joinGroup(group, null);     //Unirse al grupo 
	} catch (IOException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return;
	}
	
  }

//------------------------------------------------------------------------------
  public Object receiveObject(){

    Object object = null;  
    byte[] buffer = new byte[BUFSIZE];
    DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
    ByteArrayInputStream bis = null;
    ObjectInputStream ois = null;
    ///////////////////////////////////////////////////////////
    //EJERCICIO: recibir el paquete y deserializarlo 
    ///////////////////////////////////////////////////////////
    	try {
    		
    	    socket.receive(packet);
    	    
    	    buffer = packet.getData();
    	    int length = packet.getLength();
    	
    	    bis = new ByteArrayInputStream(buffer,0, length);
    	    ois = new ObjectInputStream(bis);
    	    object = ois.readObject();
    	    
    	} catch (IOException | ClassNotFoundException e1) {
    		// TODO Auto-generated catch block
    		        e1.printStackTrace();
    	} finally {
    		try {
    			if (ois != null) ois.close();
                if (bis != null) bis.close();
    			
    		} catch(IOException e) {
    		   e.printStackTrace();    			
    		}
    	}
     return object;
  }

//------------------------------------------------------------------------------
  public void sendObject(Object object){
    byte[] buffer;
    DatagramPacket packet;
    ByteArrayOutputStream bos = null;
    ObjectOutputStream oos = null;
    ///////////////////////////////////////////////////////////
    //EJERCICIO: serializar el paquete y difundirlo   
    ///////////////////////////////////////////////////////////
    try {
        bos = new ByteArrayOutputStream();
        oos = new ObjectOutputStream(bos);
    
        oos.writeObject(object);
        oos.flush();
     
        buffer = bos.toByteArray() ;
    
    	packet = new DatagramPacket(buffer, buffer.length,
    			InetAddress.getByName(m_ip), m_port);
    	
    	socket.send(packet);
    } catch(IOException e) {
    	e.printStackTrace();
    	
    } finally {
    	try {
    		if (oos != null) oos.close();
            if (bos != null) bos.close();
    		
    	} catch(IOException e) {
    		e.printStackTrace();
    	}
    }
    
  }
}