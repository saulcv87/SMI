/*
 *  v 2.1 Ajustes a los cambios de OpencV 4.2
  M. Agustí (Març 2021) - SMII

  v 2.0 Ajustes de la instalación de laboratorio.
  Generación procedural del posicionamiento de las fuentes de audio y revisión índices
  M. Agustí (Març 2019) - SMII

  Para compilar en linux con g++:
  $ gcc discoCam.c -o discoCam `pkg-config openal opencv --cflags` -lalut `pkg-config openal opencv --libs` -lm

  * v 1.0 Añadir generación de sonido con OpenAL. A. Ivars (Mordrekai)
  * Sobre el trabajo de detección de movimiento de D. Millán
  */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// C++ o C?
#ifdef __cplusplus
// Les capsaleres canvien en la V4, ho comprobe'm
#if CV_MAJOR_VERSION == 4
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#else
// Versió 2 i 3 : #elif CV_MAJOR_VERSION == 3 ...
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#endif
using namespace cv;  // The new C++ interface API is inside this namespace. Import it.
using namespace std; // para que "string" sea un tipo

#else
//__STDC__
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#endif

//#ifdef __cplusplus
// Les capsaleres canvien en la V4 d'OpencV, ho comprovem
#if CV_VERSION_MAJOR == 4
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
//#else
//// En la V3 d'OpencV
#include <opencv2/opencv.hpp>
using namespace cv;  // The new C++ interface API is inside this namespace. Import it.
using namespace std; // para que "string" sea un tipo

#else
//__STDC__
// En la V2 d'OpencV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#endif

/*
//#ifdef __cplusplus
// Les capsaleres canvien en la V4 d'OpencV, ho comprovem
#if CV_VERSION_MAJOR == 4
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;  // The new C++ interface API is inside this namespace. Import it.
using namespace std; // para que "string" sea un tipo

#elif CV_MAJOR_VERSION == 3 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;  // The new C++ interface API is inside this namespace. Import it.
using namespace std; // para que "string" sea un tipo

#else
//__STDC__
// En la V2 d'OpencV
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#endif
*/

#include <AL/alut.h>
#include <math.h>

#define NUM_ENVIRONMENTS 1
#define NUM_COL 4
#define NUM_FIL 4
#define NUM_BUFFERS NUM_COL*NUM_FIL
#define NUM_SOURCES NUM_COL*NUM_FIL

#define FPPAL "DISCOCAM"
#define FDEBUG "DISCOCAM_DEBUG"

Rect rect[NUM_COL][NUM_FIL];
ALuint	buffer[NUM_BUFFERS];
ALuint	fontsAudio[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];

//Posición, velocidad y orientación relativas del oyente (respectivamente)
ALfloat listenerPos[]={0.0,0.0,0.0}; //origen de coordenadas
ALfloat listenerVel[]={0.0,0.0,0.0}; //en reposo
ALfloat	listenerOri[]={0.0,0.0,-1.0, 0.0,1.0,0.0}; //mirando hacia el frente (hacia -z) y con la cabeza en posición vertical (UP= eje y)

//Posición y velocidad relativas de las fuentes
ALfloat fontsAudioPos[3];
ALfloat fontsAudioVel[]={ 0.0, 0.0, 0.0};

#define AND &&
#define OR ||
#define TRUE 1
#define FALSE 0
#define ESC 27


void inicializarAudio(){
  int i, y, x;
  int error;
  
  alutInit (0,NULL);
  alGetError();
  alListenerfv(AL_POSITION,listenerPos);
  alListenerfv(AL_VELOCITY,listenerVel);
  alListenerfv(AL_ORIENTATION,listenerOri);

  error = alGetError();
  if (error) {
    printf("%s\n", alutGetErrorString(error));
  }
  else {
    printf("Oyente Creado\n");
  }
  // Generate buffers, or else no sound will happen!
  alGenBuffers(NUM_BUFFERS, buffer);
  error=alGetError();
  if(error){
    printf("%s\n",alutGetErrorString(error));
  }

  alGenSources(NUM_SOURCES, fontsAudio);

  error = alGetError();
  if (error) {
    printf("%s\n", alutGetErrorString(error));
  }
  else {
    printf("%d - Fuentes creadas.\n", NUM_SOURCES );
  }



  i = 0;
  for(y=NUM_FIL; y >= -NUM_FIL; y-= 2)
    for(x=-NUM_COL; x <= NUM_COL; x+= 2)
      {
	if ( (y != 0) AND (x != 0) ) {
	  //	  	  buffer[i] = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, 440.0*pow(2.0,((i-5.0)/12.0)), 0.0, 1.0);
	  // https://www.palermo.edu/ingenieria/downloads/CyT6/6CyT%2003.pdf
	  buffer[i] = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, (261 + (((522-261)/NUM_COL)*x)*y), 0.0, 1.0);	  
	  error = alGetError();
	  if (error) {
	    printf("- Buffer para tono %02d. Error: %s\n",
		   (i+1), alutGetErrorString(error));
	  }
	  else {
	    printf("- Buffer para tono %02d creado.\n",(i+1));
	    alSourcef(fontsAudio[i],AL_PITCH,1.0f);
	    alSourcef(fontsAudio[i],AL_GAIN,1.0f);
	    fontsAudioPos[0] = x*10.0;
	    fontsAudioPos[1] = y*10.0;
	    fontsAudioPos[2] = 0.0;
	    alSourcefv(fontsAudio[i],AL_POSITION, fontsAudioPos ); //fontsAudioPos[i]);
	    alSourcefv(fontsAudio[i],AL_VELOCITY, fontsAudioVel); //{0.0, 0.0, 0.0} ); //fontsAudioVel);
	    alSourcei(fontsAudio[i],AL_BUFFER,buffer[i]);
	    alSourcei(fontsAudio[i],AL_LOOPING,AL_FALSE);
	    i++;
	  }

	} // if ( (y != 0) AND (x != 0) ) {
      }// for(x=-4; x <= 4; x+= 2)
  

} // Fi de inicializarAudio


int main( int argc, char** argv )
{
  int i,j, salir = FALSE, tecla;
  //IplImage* frame = 0;
  Mat frame, image, lastImage, diffImage, bitImage;
  Mat roiFrame, roiBitImage;
  int width, height;
  Rect rectGeneral;
  //  CvCapture* capture = 0;
  VideoCapture capture = 0;
  int fullScreenMode = FALSE, 
      umbralMoviment, quantitatMoviment,
      nPuntsCanviats;
  char mostrarImatge = 'b';


  if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
    capture.open( argc == 2 ? argv[1][0] - '0' : 0 );
  else if( argc == 2 )
    capture.open( argv[1] );

  if( !capture.isOpened() )
    {
      fprintf(stderr,"No se pudo iniciar la captura...\n");
      return -1;
    }

  inicializarAudio();

  printf( "Hot keys: \n"
	  "\tESC - salir: quit the program.\n"
	  "\t'f' - modo de ventana: flip to/from fullscreen mode. \n"
	  "\t'd' - mostrar diffImage en ventana de depurarción.\n"
  	  "\t'b' - mostrar bitImage en ventana de depuración.\n"	  
	  "\n");	  

  umbralMoviment = 80;
  quantitatMoviment = 10;
  
  capture.read( frame ); //Need to get correct data
  width = capture.get( CAP_PROP_FRAME_WIDTH );
  height = capture.get( CAP_PROP_FRAME_HEIGHT );
  printf("Dispositivo abierto de %dx%d píxeles\n", width, height);
  printf("frame de  %dx%d píxeles\n", frame.cols, frame.rows);
  printf("Máxima cantidad de movimiento %dx%d = %d\n",
	 (width/NUM_COL), (height/NUM_FIL), (width/NUM_COL)*(height/NUM_FIL) );

  
 #if CV_MAJOR_VERSION == 4
  namedWindow( FPPAL, WINDOW_NORMAL ); 
  namedWindow( FDEBUG, WINDOW_NORMAL );
#else
  namedWindow( FPPAL, CV_WINDOW_NORMAL | CV_GUI_EXPANDED); // CV_GUI_NORMAL
  namedWindow( FDEBUG, CV_WINDOW_NORMAL );
 #endif

  createTrackbar("Umbral de movimiento: ", FDEBUG, &umbralMoviment, 255, NULL);
  createTrackbar("Cantidad de movimiento: ", FDEBUG, &quantitatMoviment,
		   (width/NUM_COL) * (height/NUM_FIL), NULL);
		   
  //Creamos las areas de interés (ROI) que coincidirán con la posición de las teclas más el ROI general
  for(i = 0; i < NUM_FIL; i++){
    for(j = 0; j < NUM_COL; j++){  // Rect( x,y, width,height )
      rect[i][j] = Rect( (width/NUM_COL)*(i),   (height/NUM_FIL)*(j),
			 (width/NUM_COL), (height/NUM_FIL) );
    }
  }
  rectGeneral = Rect(0,0,width,height);


  // Crear les imatges temporals a partir de la obtessa de la càmera
  image.create( frame.rows, frame.cols, frame.depth() ); 
  diffImage.create( frame.rows, frame.cols, frame.depth() ); 
  bitImage.create( frame.rows, frame.cols, frame.depth() ); 
  lastImage.create( frame.rows, frame.cols, frame.depth() ); 
      
#if CV_MAJOR_VERSION == 4
  cvtColor(frame, lastImage, COLOR_BGR2GRAY);
#else
  cvtColor(frame, lastImage, CV_BGR2GRAY);
#endif

  
  //for(;;)
  while ( !salir )
  {
      //capture.read( frame );
      if ( !capture.grab() ) { // Congela la imatge a la càmera
	salir = true; 
	break;
      }
      else {
        capture.retrieve(frame, 0 ); // Decodes and returns the grabbed video frame.
      }
      
      //Convert frame to gray and store in image
#if CV_MAJOR_VERSION == 4
      cvtColor(frame, image, COLOR_BGR2GRAY);
#else
      cvtColor(frame, image, CV_BGR2GRAY);
#endif

        
      //Differences with actual and last image
      absdiff( image, lastImage, diffImage );
      //threshold image
      threshold(diffImage, bitImage, umbralMoviment, 255, THRESH_BINARY);
      // THRESH_BINARY: if src(x,y) > umbral; dst(x,y) = 255; else 0

      for(i=0;i<NUM_FIL;i++){
    	for(j=0;j<NUM_COL;j++){
#if CV_MAJOR_VERSION == 4
            rectangle( frame,Point( (width/NUM_COL*(i)),   (height/NUM_FIL*(j))), 
		           Point( (width/NUM_COL*(i+1)), (height/NUM_FIL*(j+1))),
                      CV_RGB(0,0,0), 2, 2, 0 );
#else
            rectangle( frame,cvPoint( (width/NUM_COL*(i)),   (height/NUM_FIL*(j))), 
		           cvPoint( (width/NUM_COL*(i+1)), (height/NUM_FIL*(j+1))),
                      CV_RGB(0,0,0), 2, 2, 0 );
#endif
          

	  roiFrame = frame( rect[i][j] );
	  roiBitImage = bitImage( rect[i][j] );
	  nPuntsCanviats = countNonZero( roiBitImage ); //cvSum(bitImage );
	  //	  printf("En rect[%d][%d]: nPuntsCanviats %d\n", i, j, nPuntsCanviats );

    	  if( nPuntsCanviats > quantitatMoviment )
	  {
	    alSourcePlay(fontsAudio[(i*NUM_COL)+j]);
#if CV_MAJOR_VERSION == 4
	    cvtColor(roiFrame, roiFrame, COLOR_RGB2HSV );
#else
	    cvtColor(roiFrame, roiFrame, CV_RGB2HSV );
#endif
	  } // if ( nPuntsCanviats > quantitatMoviment )
	} // for(j=0;j<NUM_COL;j++)
      } //for(i=0;i<NUM_FIL;i++){

      //      frame.roi( rectGeneral );
      imshow( FPPAL , frame );

      // guardar el cuadro que acaba de processar-se
      lastImage = image.clone();

      if (mostrarImatge == 'b')	  
        imshow( FDEBUG, bitImage);
      else
        imshow( FDEBUG, diffImage);
          

      tecla = waitKey(25) & 255;  // Espera una tecla los milisegundos que haga falta
      switch ( tecla )
	{
	case ESC:
	case 'q':
	case 'Q':    // Si 'ESC', q ó Q, ¡acabar!
	  salir = TRUE;
	  break;

 	case 'b':    
 	  mostrarImatge = 'b';
 	  break;
	  
 	case 'd':    
 	  mostrarImatge = 'd';
 	  break;
  
	case 'f':
	  fullScreenMode = !fullScreenMode;
	  if ( fullScreenMode )
#if CV_MAJOR_VERSION == 4
        setWindowProperty( FPPAL, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
#else
        cvSetWindowProperty( FPPAL, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
#endif
	  else
#if CV_MAJOR_VERSION == 4
        setWindowProperty( FPPAL, WND_PROP_FULLSCREEN, WINDOW_NORMAL);
#else
        cvSetWindowProperty( FPPAL, CV_WND_PROP_FULLSCREEN, CV_WINDOW_NORMAL);
#endif           
	  break;
	default: ;    
	} // Fin de "switch ( tecla )"  

  } // while ( !salir )

  alDeleteSources(NUM_SOURCES, fontsAudio );
  alDeleteBuffers(NUM_BUFFERS, buffer );
  alutExit ();
  
  return 0;
} // main
