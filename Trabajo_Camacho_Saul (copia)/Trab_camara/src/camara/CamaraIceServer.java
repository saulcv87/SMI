/*
 *  @author Jose Simo. 
 *  (c) ai2-UPV Creative Commons.
 *  Rev: 2022
 */
package camara;

import java.util.Properties;

import agencia.datos.IPYPort;
/**
 *
 */
public class CamaraIceServer {
	
	private static IPYPort ipyport;
	/**
	 * 
	 */
	public static void main(String[] args) {
		Properties sys_props = System.getProperties(); 
		sys_props.setProperty("org.apache.activemq.SERIALIZABLE_PACKAGES", "*");

		//
	    if (args.length >= 2)
		    ipyport = new IPYPort( args[0], Integer.parseInt(args[1]) );
		else
		    ipyport = new IPYPort( "228.7.7.7", 7010);
	    //
		System.out.println("Difusion por " + ipyport.ip + "_" + ipyport.port);
		///////////////////////////////////////////////////////////////////////////////////////////////
		// Get the initialized property set.
    	com.zeroc.Ice.Properties props = com.zeroc.Ice.Util.createProperties(args);
    	// Set props
    	props.setProperty("Ice.Default.Locator", "IceGrid/Locator:tcp -h localhost -p 12000");
    	props.setProperty("CamaraAdapter.Endpoints", "tcp");
    	props.setProperty("CamaraAdapter.AdapterId", "CamaraAdapter");
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
        	//Create adapter.
        	com.zeroc.Ice.ObjectAdapter adapter = ic.createObjectAdapter("CamaraAdapter");
        	// Create servant and register it into the adapter.
            com.zeroc.Ice.Object iceObject = new CamaraI(ic, ipyport);
            adapter.add(iceObject, com.zeroc.Ice.Util.stringToIdentity("Camara"));            
            //
            //Generate a proxy to get its string form.
            com.zeroc.Ice.ObjectPrx proxy = adapter.createDirectProxy(com.zeroc.Ice.Util.stringToIdentity("Camara"));
            String s1 = ic.proxyToString(proxy);
            System.out.println("IOR de la Camara: " + s1);
            //
            System.out.println("Arrancando la difusion de la Camara: ");
            ((CamaraI)iceObject).start();
            //
            // Activate adapter
            adapter.activate();
            // Block main thread.
            ic.waitForShutdown();
        }
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
