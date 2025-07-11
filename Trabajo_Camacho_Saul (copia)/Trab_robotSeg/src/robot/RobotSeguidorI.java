package robot;

import agencia.datos.Escenario;
import agencia.datos.EstadoRobot;
import agencia.datos.Instantanea;
import agencia.datos.Posicion;
import agencia.datos.PuntosRobot;
import agencia.datos.suscripcion;
import agencia.objetos.CamaraPrx;

import comm.Difusion;
import comm.DifusionJMS;

import com.zeroc.Ice.Current;
import khepera.control.*;
import khepera.escenario.*;
import khepera.robot.*;

public class RobotSeguidorI implements agencia.objetos.RobotSeguidor {
   CamaraPrx camaraProxy = null;
   private String minombre = "Mazinger";
   private String miIOR = null;
   private int miid;
   private PuntosRobot mipuntos;
   private Polares mipos = new Polares();
   private Posicion miobj = new Posicion(500.0F, 500.0F);
   private int miLider = -1;
   private Instantanea instantanea;
   private int indexLider = -1;
   private EscenarioKhepera escenario;
   private RobotKhepera robotkhepera;
   private Trayectoria tra;
   private Destino dst = new Destino();
   private Braitenberg bra = new Braitenberg();

   public void setNombre(String nom) {
      this.minombre = nom;
   }

   public void setRobotIOR(String ior) {
      this.miIOR = ior;
   }

   public RobotSeguidorI(String name,CamaraPrx camPrx) {
      this.camaraProxy = camPrx;
      
      Posicion centro = new Posicion(20.0F, 20.0F);
      Posicion[] sens = new Posicion[9];
      Posicion[] finsens = new Posicion[9];
      Posicion[] inter = new Posicion[8];

      int i;
      for(i = 0; i < 9; ++i) {
         sens[i] = new Posicion(0.0F, 0.0F);
      }

      for(i = 0; i < 9; ++i) {
         finsens[i] = new Posicion(0.0F, 0.0F);
      }

      for(i = 0; i < 8; ++i) {
         inter[i] = new Posicion(0.0F, 0.0F);
      }

      this.mipuntos = new PuntosRobot(centro, sens, finsens, inter);
   }

   public void start() {
      new RobotDifusion().start();
   }

   void avanzar() {
      new IzqDer();
      new IzqDer();
      this.robotkhepera.avanzar();
      this.mipos = this.robotkhepera.posicionPolares();
      this.mipuntos = this.robotkhepera.posicionRobot();
      
      System.out.println("Posicion: " + mipos.x + ", " + mipos.y + " posCentro: " + mipuntos.centro.x + ", " + mipuntos.centro.y);
      
      this.tra = new Trayectoria(this.mipos, this.miobj);
      float[] ls = this.robotkhepera.leerSensores();
      IzqDer nv = this.dst.calcularVelocidad(this.tra);
      IzqDer nv2 = this.bra.calcularVelocidad(ls);
      nv.izq += nv2.izq / 90;
      nv.der += nv2.der / 90;
      this.robotkhepera.fijarVelocidad(nv.izq, nv.der);
   }

   public EstadoRobot ObtenerEstado(Current current) {
	   EstadoRobot er = new EstadoRobot();
	   er.nombre = minombre;
	   er.id = miid;
	   er.IORrob = miIOR;
	   er.puntrob = mipuntos;
	   er.posObj = miobj;
	   er.idLider = miLider;
	   
	   System.out.println(er.nombre + ", " + er.id +  ", " + er.puntrob.centro.x +  ", " + er.puntrob.centro.y + ", " + er.posObj.x + ", " + er.posObj.y + ", " + er.idLider);
	   return er;
   }

   public void ModificarEscenario(Escenario esc, Current current) {
      this.escenario = new EscenarioKhepera(esc);
      this.robotkhepera = new RobotKhepera(new Posicion(20.0F, 20.0F), this.escenario, 0);
      System.out.println("Nuevo Escenario......");
   }

   public void ModificarObjetivo(Posicion NuevoObj, Current current) {
	// TODO Auto-generated method stub
		this.miobj.x = NuevoObj.x;
		this.miobj.y = NuevoObj.y;
		
		this.miLider = -1;
		this.indexLider = -1;
		
		System.out.println("Nueva posicion Objetivo: X= " + NuevoObj.x + ", Y= " + NuevoObj.y);
   }

   public void ModificarPosicion(Posicion npos, Current current) {
      this.robotkhepera.fijarPosicion(npos);
      System.out.println("Nueva Posicion: " + npos.x + "," + npos.y);
   }

   public void ModificarLider(int idLider, Current current) {
	   boolean encontrado = false; 
	   if (this.instantanea != null) { 
		   for (int i = 0; i < instantanea.estadorobs.length; ++i) { 
			   EstadoRobot sr = instantanea.estadorobs[i]; 
			   if (sr.id == idLider && sr.id != miid) { 
				   this.miLider = idLider; 
				   this.indexLider = i;
				   miobj = instantanea.estadorobs[indexLider].puntrob.centro;
				   encontrado = true;
				   System.out.println("Nuevo Lider -> " + idLider);
				   break;
			   }
		   } 
		   if (!encontrado) {
			   System.out.println("Lider Invalido: " + idLider);
			   for (int i = 0; i < instantanea.estadorobs.length; ++i) { 
				   EstadoRobot sr = instantanea.estadorobs[i];
				   if (sr.id != miid) {
					   
					   this.miLider = sr.id; 
					   this.indexLider = i;
					   miobj = sr.puntrob.centro; 
					   encontrado = true;
					   System.out.println("Promovido nuevo Líder -> " + sr.id); break; 
				   } 
			   } 
		   }
	   }

   }
   class RobotDifusion extends Thread {
	   private Difusion difusion;
	   private EstadoRobot sr;
	   
	   private suscripcion sus;
      private Instantanea inst1;
      private Instantanea inst2;
      
      
	   
	   private boolean encontrado = false;

	   public void run() {
	      sus = camaraProxy.SuscribirRobot(miIOR);
	      try {
	         this.difusion = new DifusionJMS(sus.iport.ip, sus.iport.port);
	      } catch (Exception e) {
	         e.printStackTrace();
	      }
	      miid= sus.id;
	      escenario = new EscenarioKhepera(this.sus.esc);
	      robotkhepera = new RobotKhepera(new Posicion(20.0F, 20.0F), escenario, 0);
	      
	      inst1 = (Instantanea)this.difusion.receiveObject();
          inst2 = inst1;
          
          

          if (miid > 0 && inst1.estadorobs.length > 1) {
        	    // Buscar el último robot disponible 
        	    for (int i = inst1.estadorobs.length - 1; i >= 0; i--) {
        	        EstadoRobot posibleLider = inst1.estadorobs[i];
        	        if (posibleLider.id != miid) { 
        	            miLider = posibleLider.id;
        	            indexLider = i;
        	            miobj = posibleLider.puntrob.centro; 
        	            System.out.println("Nuevo líder asignado: " + miLider);
        	            break;
        	        }
        	    }
        	} else {
        	    // Si no hay robots vivos o no hay más de uno, ser el propio líder
        	    miLider = -1;
        	    indexLider = -1;
        	    miobj = new Posicion(500.0F, 500.0F); 
        	    System.out.println("Este robot es ahora el líder.");
        	}
          
	      while(true) {
	    	  encontrado = false;
	    	  inst1 = (Instantanea)this.difusion.receiveObject();
	          
	    	  for(int i = 0; i < inst1.estadorobs.length; ++i) {
	              sr = inst1.estadorobs[i];
	              if (sr.id == indexLider && sr.id != miid) {
	            	  encontrado = true;
	                  indexLider = sr.id;
	                  miobj = sr.posObj;
	                  System.out.println("Lider actual :" + indexLider);
	                  
	                  break;
                  } 
	          }
	          
	    	  if(!encontrado && sr.idLider != -1 && inst1.estadorobs.length > 1){
	            for (int i= 0; i < inst1.estadorobs.length; i++) {
	                this.sr = inst1.estadorobs[i];
	                for(int j= 0; j < inst2.estadorobs.length; j++) {
	            	    EstadoRobot sr2 = inst2.estadorobs[j];
	                   
	                     if(sr2.id == indexLider){
	                        indexLider = sr2.idLider;
	                        System.out.println("lider  sr2 " + indexLider);
	                     }
	                }
	            }
	          }
	          try {
	    	     if (indexLider >= 0) {
	    		    System.out.println("este es el lider: " + indexLider);
	                 miobj = inst1.estadorobs[indexLider].puntrob.centro;
	             } 
	          } catch (Exception e) {
	                   miLider = -1;
	                   System.out.println("Error al asignar el líder");
	                   e.printStackTrace(); 
	                   
	                   
	       	    }
	      		  inst2 = inst1; 
	           
	            	  avanzar();
	           
	      		      
		    }
      }
	}
}