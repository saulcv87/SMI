/*
 *  @author Jose Simo. 
 *  (c) ai2-UPV Creative Commons.
 *  Rev: 2022
 */
package camara;

import java.util.LinkedList;

import java.util.Iterator;
import java.util.Timer;
import java.util.TimerTask;

import com.zeroc.Ice.Current;
import com.zeroc.Ice.ObjectPrx;

import agencia.datos.Escenario;
import agencia.datos.EstadoRobot;
import agencia.datos.IPYPort;
import agencia.datos.Instantanea;
import agencia.datos.ListaSuscripcion;
import agencia.datos.suscripcion;
import agencia.objetos.ConsolaPrx;
import agencia.objetos.RobotSeguidorPrx;
import comm.Difusion;
//import comm.DifusionMulticast;
import comm.DifusionJMS;
import khepera.escenario.EscenarioKhepera;


/**
 * 
 */
public class CamaraI implements agencia.objetos.Camara
{
	
   private LinkedList<String> listaRobots = new LinkedList<String>();
   private LinkedList<EstadoRobot> listaEstados = new LinkedList<EstadoRobot>();
   
   private LinkedList<String> listaConsolas = new LinkedList<String>();

   Instantanea instantanea;
   private int nrobots, nconsolas;

   private IPYPort ipyport;
   
   private com.zeroc.Ice.Communicator iceComunicator;
   
   private Escenario escKhepera = (new EscenarioKhepera()).toEscenario();
   
   boolean modificarEsce = false; 

///////////////////////////////////////////////////////////////////////////////////////////////
   
   public CamaraI(com.zeroc.Ice.Communicator ic, IPYPort iport) {
	   
	   iceComunicator = ic;
     ipyport = new IPYPort(iport.ip, iport.port);
     nrobots = 0; 
     nconsolas = 0;
   }
   
   
   public void start(){

	      new CamaraDifusion(ipyport);
   }
   
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
   
	@Override
	public suscripcion SuscribirRobot(String IORrob, Current current) {
		
		//////////////////////////////////////////////////////
	  //EJERCICIO: Implementar la suscripcion del robot
		//EJERCICIO: Retornar un objeto "suscripcion".
		//////////////////////////////////////////////////////
		ipyport = new IPYPort("228.7.7.7", 7010);
	    suscripcion s = new suscripcion(nrobots++, ipyport, escKhepera);
	    
	    listaRobots.add(IORrob);  
	    System.out.println("Robot suscrito: " + IORrob);
        return s;
        
	}
	
	private void mEsc() {
	      LinkedList<String> listaFallos = new LinkedList();
	      LinkedList<String> listaRobotsCopia = (LinkedList)listaRobots.clone();
	      Iterator<String> i = listaRobotsCopia.iterator();

	      while(i.hasNext()) {
	         try {
	            ObjectPrx base = iceComunicator.stringToProxy((String)i.next());
	            RobotSeguidorPrx robotProxy = RobotSeguidorPrx.checkedCast(base);
	            if (robotProxy == null) {
	               throw new Error("Invalid proxy");
	            }
	            robotProxy.ModificarEscenario(escKhepera);
	         } catch (Exception e) {
	            System.out.println("Detectado fallo Robot: " + (String)i.next());
	            listaFallos.add((String)i.next());
	         }
	      }

	      i = listaFallos.iterator();

	      while(i.hasNext()) {
	         this.BajaRobot((String)i.next(), (Current)null);
	      }

	      listaFallos.clear();
	      LinkedList<String> listaConsolasCopia = (LinkedList)listaConsolas.clone();
	      

	      while(i.hasNext()) {
	         try {
	            ObjectPrx base = iceComunicator.stringToProxy((String)i.next());
	            ConsolaPrx consolaProxy = ConsolaPrx.checkedCast(base);
	            if (consolaProxy == null) {
	               throw new Error("Invalid proxy");
	            }

	            consolaProxy.ModificarEscenario(escKhepera);
	         } catch (Exception e) {
	            String consolaFallo = (String)i.next();
	            System.out.println("Detectado fallo Consola: " + consolaFallo);
	            listaFallos.add(consolaFallo);
	         }
	      }

	      i = listaFallos.iterator();

	      while(i.hasNext()) {
	         this.BajaConsola((String)i.next(), (Current)null);
	      }

	      modificarEsce = false;
	   }

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

    //------------------------------------------------------------------------------
    // La clase anidada CamaraDifusion
    //------------------------------------------------------------------------------
    class CamaraDifusion {
      private Timer timer;
      private Difusor df;
      private int periodMS = 500;

      public CamaraDifusion(IPYPort iport){
         timer = new Timer();
         df= new Difusor(iport);
         timer.schedule(df,0,periodMS);
      }
    //------------------------------------------------------------------------------
      class Difusor extends TimerTask {
    	  
          private Difusion difusion;
          //
          public Difusor(IPYPort iport){
           //
            try {
				//difusion = new DifusionMulticast(iport.ip, ipyport.port);
            	difusion = new DifusionJMS(iport.ip, ipyport.port);
			} catch (Exception e) {
				e.printStackTrace();
			}
          }
          public void run() {
             //System.out.print("Voy a difundir...");
             EstadoRobot st = new EstadoRobot();
             LinkedList<String> listaFallos = new LinkedList<String>();
              //
             if(modificarEsce) {
            	 mEsc();
             }
             listaEstados.clear();
             listaFallos.clear();
             LinkedList<String> listaRobotsCopia = (LinkedList<String>) listaRobots.clone();
             
             
             for (Iterator<String> i = listaRobotsCopia.iterator(); i.hasNext(); ) {
               String ior=null;
               try {
                    ior = (String) i.next();
                    
                    try (com.zeroc.Ice.Communicator communicator = com.zeroc.Ice.Util.initialize(_iceIds)) {
                    	com.zeroc.Ice.ObjectPrx robotIn = communicator.stringToProxy(ior);
                    	
                    	RobotSeguidorPrx robotProx = RobotSeguidorPrx.checkedCast(robotIn);
                        st = robotProx.ObtenerEstado();
                        
                        listaEstados.add(st);
                    }
      
               } catch (Exception e){
                  System.out.println("Detectado fallo x en Robot: " + ior );
                  ////////////////////////////////////////////////////////////////
                  //EJERCICIO: anyadir el robot caido a la lista de fallos 
                  ////////////////////////////////////////////////////////////////
                  listaRobots.remove(ior);
                  listaFallos.add(ior);
                 
               }
             }
             //
             for (Iterator<String> i = listaFallos.iterator(); i.hasNext(); ) {
            	 listaRobots.remove(i.next());
             }
                        
             ////////////////////////////////////////////////////////////////
             //EJERCICIO: crear una instantanea a partir de la lista de estados de los robots. 
             instantanea = new Instantanea((EstadoRobot[]) (listaEstados.toArray(new EstadoRobot[0])));
             
           //EJERCICIO: difundir la instantanea 
             ////////////////////////////////////////////////////////////////////////
             try {
            	 difusion.sendObject(instantanea);
            	 
             } catch(Exception e) {
            	 e.printStackTrace();
             }
             
             
             //////
        } // de run
      } // de clase difusor
    } // de clase CamaraDifusion
//////////////////////////////////////CamaraDifusion//////////////////////////////////////////////

	@Override
	public suscripcion SuscribirConsola(String IORcons, Current current) {
		// TODO Auto-generated method stub
		suscripcion sc = new suscripcion(nconsolas++, ipyport, escKhepera);
		listaConsolas.add(IORcons);
		System.out.println("Consola suscrita: " + IORcons);
		return sc;
	}


	@Override
	public  void BajaRobot(String IORrob, Current current) {
		// TODO Auto-generated method stub
		listaRobots.remove(IORrob);
		System.out.println("Procediendo a dar de baja al robot: " + IORrob);
		
		
	}


	@Override
	public  void BajaConsola(String IORcons, Current current) {
		// TODO Auto-generated method stub
		listaConsolas.remove(IORcons);
		System.out.println("Procediendo a dar de baja a la consola: " + IORcons);
		
	}


	@Override
	public ListaSuscripcion ObtenerLista(Current current) {
		// TODO Auto-generated method stub
		return new ListaSuscripcion((String[]) listaRobots.toArray(new String[0]), (String[]) listaConsolas.toArray(new String[0]));
	}


	@Override
	public IPYPort ObtenerIPYPortDifusion(Current current) {
		// TODO Auto-generated method stub
		return this.ipyport;
	}


	@Override
	public Instantanea ObtenerInstantanea(Current current) {
		// TODO Auto-generated method stub
		Instantanea instanCamara = new Instantanea((EstadoRobot[])listaEstados.toArray(new EstadoRobot[0]));
		return instanCamara;
	}


	@Override
	public void ModificarEscenario(Escenario esc, Current current) {
		// TODO Auto-generated method stub
		escKhepera = esc;
		modificarEsce = true;
	}


	@Override
	public Escenario ObtenerEscenario(Current current) {
		// TODO Auto-generated method stub
		if(escKhepera == null) {
			throw new IllegalStateException("No hay escenario configurado");
		}
		
		return escKhepera;
	}
    
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

