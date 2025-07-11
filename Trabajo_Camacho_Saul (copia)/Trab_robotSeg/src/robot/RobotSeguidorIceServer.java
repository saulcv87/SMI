/*
 *  @author Jose Simo. 
 *  (c) ai2-UPV Creative Commons.
 *  Rev: 2022
 */
package robot;

import java.util.Properties;



import agencia.objetos.CamaraPrx;



public class RobotSeguidorIceServer {
	public RobotSeguidorIceServer() {
		
	}


	public static void main(String[] args) {
		

		String robotName = "MinervaX";
		// Get robot name
    	if (args.length > 0) {
    		robotName = args[0];
    	}
    	Properties sys_props = System.getProperties(); 
		sys_props.setProperty("org.apache.activemq.SERIALIZABLE_PACKAGES", "*");
		
	
		
		//
		// Get the initialized property set.
    	com.zeroc.Ice.Properties props = com.zeroc.Ice.Util.createProperties(args);
    	// Set props
    	props.setProperty("Ice.Default.Locator", "IceGrid/Locator:tcp -h localhost -p 12000");
    	//
    	// Initialize a communicator with these properties.
    	com.zeroc.Ice.InitializationData initData = new com.zeroc.Ice.InitializationData();
    	initData.properties = props;
        //
        // Try with resources block.
    	// Communicator "ic" is automatically destroyed
        // at the end of this try block
        //
        try (com.zeroc.Ice.Communicator ic = com.zeroc.Ice.Util.initialize(initData)) 
        {
            // Install shutdown hook to (also) destroy communicator during JVM shutdown.
            Runtime.getRuntime().addShutdownHook(new Thread(() -> ic.destroy()));
            //            
            ///////////////////////////////////////////////////////////////////////////////
            // Get Camara proxy
            boolean cameraConnected = false;
            CamaraPrx camProxy = null;
            while (!cameraConnected) {
            	try {
            		///////////////////////////////////////////////////////////////////////////////
            		//EJERCICIO: Conectar con el servidor de nombres 
            		//           y obtener una referencia a la Camara (camProxy). 
            		///////////////////////////////////////////////////////////////////////////////
            		com.zeroc.Ice.ObjectPrx conex = ic.stringToProxy("Camara@CamaraAdapter");
            		System.out.println("Proxy obtenido: " + conex);
            		camProxy = CamaraPrx.checkedCast(conex);
            		
                    cameraConnected = true;
            	
            		

            	} catch (com.zeroc.Ice.NotRegisteredException ncEx) {
            		System.out.println("Objeto distribuido ICE \"Camara\" no disponible, nuevo intento de conexion en 2 segundos.");
            		ncEx.printStackTrace();
            		try {Thread.sleep(2000);} catch (InterruptedException e) {}
            	}
            }
            if (camProxy == null) { throw new Error("Invalid proxy");}
            
            ///////////////////////////////////////////////////////////////////////////////
            //
            ///////////////////////////////////////////////////////////////////////////////
        	// Create robot ICE object.
        	com.zeroc.Ice.ObjectAdapter adapter = ic.createObjectAdapterWithEndpoints("RobotAdapter","tcp");
        	// Create servant and register it into the adapter.
            com.zeroc.Ice.Object robotIceObject = new RobotSeguidorI(robotName, camProxy);
            adapter.add(robotIceObject, com.zeroc.Ice.Util.stringToIdentity("RobotSeguidor"));   
            // Activate adapter
            adapter.activate();
            //
            String robotIOR = "";
            ///////////////////////////////////////////////////////////////////////////////
        	//EJERCICIO: convertir la referencia al robot en un IOR en formato String
            //           y almacenarlo en la variable robotIOR
            ///////////////////////////////////////////////////////////////////////////////
            com.zeroc.Ice.ObjectPrx proxy = adapter.createDirectProxy(com.zeroc.Ice.Util.stringToIdentity("RobotSeguidor")); 
            robotIOR = ic.proxyToString(proxy);
    
            
            System.out.println("IOR (robot): " + robotIOR);
            ///////////////////////////////////////////////////////////////////////////////
            //
            //Establecer dependencias
            RobotSeguidorI robotI = (RobotSeguidorI)robotIceObject;
            robotI.setRobotIOR(robotIOR);
            if (args.length > 0) robotI.setNombre(args[0]);
       
            //Arrancar robot
            robotI.start();
            //
            // Block main thread.
            ic.waitForShutdown();
            //
        }
		
	}
}
