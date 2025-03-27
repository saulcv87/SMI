/*
  Visualitzant audio3D: OpenAL + OpenGL

  MindCode's OpenAL Lesson 3: Multiple Sources
  http://forum.devmaster.net/t/openal-lesson-3-multiple-sources/2890
  + 
  PIGE-OpenAL
  Created by Chad Armstrong on Mon Jul 29 2002.
  http://www.edenwaith.com/products/pige/tutorials/openal.php

  Atres referències:
  * The freeglut Project: API Documentation
  http://freeglut.sourceforge.net/docs/api.php#EventProcessing

  * The OpenAL Utility Toolkit (ALUT)
  http://distro.ibiblio.org/rootlinux/rootlinux-ports/more/freealut/freealut-1.1.0/doc/alut.html

  * OpenAL Lesson 7: The Doppler Effect
  http://devmaster.net/p/2894/openal-lesson-7-the-doppler-effect

*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

// Modo text
//#include "keyboard.h"

//Modo gràfic
#include <GL/glut.h>
#include <GL/freeglut.h> // FullScreen*


// Maximum data buffers we will need.
#define NUM_BUFFERS 10

// Maximum emissions we will need.
#define NUM_SOURCES 10

// These index the buffers and sources.
#define BATTLE 0
#define VEHICLE 1
#define GUN1   2
#define GUN2   3
#define HELLO 4
#define SINE 5
#define SQUARE 6
#define SAWTOOH 7
#define WHITE_NOISE 8
#define IMPULSE 9

// Buffers hold sound data.
ALuint Buffers[NUM_BUFFERS];

// Sources are points of emitting sound.
ALuint Sources[NUM_SOURCES];

// Position of the source sounds.
ALfloat SourcesPos[NUM_SOURCES][3];

// Velocity of the source sounds.
ALfloat SourcesVel[NUM_SOURCES][3];

// Position of the listener.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

// Velocity of the listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
ALfloat ListenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Estava 0,0, 0,0, -1.0 ...
// però per aliner-ho en la càmera d'OpenGL canvie el signe de la Z de "at".

ALboolean mute[NUM_SOURCES];
int 	GLwin;
#define DISTANCIA 10
ALfloat velocitatDelMovil = 80;
/*
  https://es.wikipedia.org/wiki/Velocidad_del_sonido
  La velocidad del sonido es la dinámica de propagación de las ondas sonoras. En la atmósfera terrestre es de 343,2 m/s (a 20 °C de temperatura, con 50 % de humedad y a nivel del mar). La velocidad del sonido varía en función del medio en el que se trasmite.

  343 m/s --> 1234.799012 Km/h
*/

ALfloat factorPerAlDoppler = 0.0;
ALfloat velocitatPerAlDoppler = 0.0;
/*

 OpenAL Lesson 7: The Doppler Effect

 'alDopplerFactor'== 0 -> Doing this will disable the Doppler effect and may in fact help overall performance (but won't be as realistic). 
  The effect of the Doppler factor will directly change the magnitude of the equation. 
                  == 1.0 will not change the effect at all. 
                  Passing anything between 0.0 and 1.0 will minimize the Doppler effect, 
                  and anything greater than 1.0 will maximize the effect.

For 'alDopplerVelocity' any non-negative non-zero value will suffice. Passing either a negative or a zero will raise an error of 'AL_INVALID_VALUE', and the whole command will be ignored. The Doppler velocity is essentially the speed of sound. Setting this will be like setting how fast sound can move through the medium. OpenAL has no sense of medium, but setting the velocity will give the effect of a medium. OpenAL also has no sense of units (kilometer, miles, parsecs), so keep that in mind when you set this value so it is consistent with all other notions of units that you have defined.
/*

/*
 * ALboolean LoadALData()
 *
 *	This function will load our sample data from the disk using the alut
 *	utility and send the data into OpenAL as a buffer. A source is then
 *	also created to play that buffer.
 */
ALboolean LoadALData()
{
  /* en alutCreateBufferFromFile no fan falta!!
  // Variables to load into.
  ALenum format;
  ALsizei size;
  ALvoid* data;
  ALsizei freq;
  ALboolean loop;
  */
  
  // Load wav data into buffers.
  alGenBuffers(NUM_BUFFERS, Buffers);

  if(alGetError() != AL_NO_ERROR)
    return AL_FALSE;

  /* Estas funciones ya no están en OpenAL 1.1
    alutLoadWAVFile((ALbyte*)"wavdata/Battle.wav", &format, &data, &size, &freq, &loop);
    alBufferData(Buffers[BATTLE], format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);
  */
  Buffers[BATTLE] = alutCreateBufferFromFile( "openal_escena_data/Battle.wav" );
  mute[BATTLE] = 0;

  /*
    alutLoadWAVFile((ALbyte*)"wavdata/Gun1.wav", &format, &data, &size, &freq, &loop);
    alBufferData(Buffers[GUN1], format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);
  */
  Buffers[GUN1] = alutCreateBufferFromFile( "openal_escena_data/Gun1.wav" );
  mute[GUN1] = 0;

  /*
    alutLoadWAVFile((ALbyte*)"wavdata/Gun2.wav", &format, &data, &size, &freq, &loop);
    alBufferData(Buffers[GUN2], format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);
  */
  Buffers[GUN2] = alutCreateBufferFromFile( "openal_escena_data/Gun2.wav" );
  mute[GUN2] = 0;
	
  Buffers[VEHICLE] = alutCreateBufferFromFile( "openal_escena_data/ambulancia.wav" );
  //Buffers[VEHICLE] = alutCreateBufferFromFile( "openal_doppler_data/car_idle.wav" );
  //Buffers[VEHICLE] = alutCreateBufferFromFile( "openal_doppler_data/EinsteinIntro.wav" );
  mute[VEHICLE] = 0;
  
  Buffers[HELLO] = alutCreateBufferHelloWorld ();
  mute[HELLO] = 1;
  
  Buffers[SINE] = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, 440.0, 0.0, 1.0);
  mute[SINE] = 1;
  
  Buffers[SQUARE] = alutCreateBufferWaveform(ALUT_WAVEFORM_SQUARE,  440.0, 0.0, 1.0);
  mute[SQUARE] = 1;
  
  Buffers[SAWTOOH] = alutCreateBufferWaveform(ALUT_WAVEFORM_SAWTOOTH,  440.0, 0.0, 1.0);
  mute[SAWTOOH] = 1;
  
  Buffers[WHITE_NOISE] = alutCreateBufferWaveform(ALUT_WAVEFORM_WHITENOISE,   440.0, 0.0, 1.0);
  mute[WHITE_NOISE] = 1;
  
  Buffers[IMPULSE] = alutCreateBufferWaveform(ALUT_WAVEFORM_IMPULSE, 440.0, 0.0, 1.0);
  mute[IMPULSE] = 1;
  
  


  // Bind buffers into audio sources.
  alGenSources(NUM_SOURCES, Sources);

  if(alGetError() != AL_NO_ERROR)
    return AL_FALSE;

  alSourcei (Sources[VEHICLE], AL_BUFFER,   Buffers[VEHICLE]   );
  alSourcef (Sources[VEHICLE], AL_PITCH,    1.0f              );
  alSourcef (Sources[VEHICLE], AL_GAIN,     1.0f              );
  alSourcefv(Sources[VEHICLE], AL_POSITION, SourcesPos[VEHICLE]);
  alSourcefv(Sources[VEHICLE], AL_VELOCITY, SourcesVel[VEHICLE]);
  alSourcei (Sources[VEHICLE], AL_LOOPING,  AL_TRUE           );

  alSourcei (Sources[BATTLE], AL_BUFFER,   Buffers[BATTLE]   );
  alSourcef (Sources[BATTLE], AL_PITCH,    1.0f              );
  alSourcef (Sources[BATTLE], AL_GAIN,     1.0f              );
  alSourcefv(Sources[BATTLE], AL_POSITION, SourcesPos[BATTLE]);
  alSourcefv(Sources[BATTLE], AL_VELOCITY, SourcesVel[BATTLE]);
  alSourcei (Sources[BATTLE], AL_LOOPING,  AL_TRUE           );

  alSourcei (Sources[GUN1], AL_BUFFER,   Buffers[GUN1]   );
  alSourcef (Sources[GUN1], AL_PITCH,    1.0f            );
  alSourcef (Sources[GUN1], AL_GAIN,     1.0f            );
  alSourcefv(Sources[GUN1], AL_POSITION, SourcesPos[GUN1]);
  alSourcefv(Sources[GUN1], AL_VELOCITY, SourcesVel[GUN1]);
  alSourcei (Sources[GUN1], AL_LOOPING,  AL_FALSE        );

  alSourcei (Sources[GUN2], AL_BUFFER,   Buffers[GUN2]   );
  alSourcef (Sources[GUN2], AL_PITCH,    1.0f            );
  alSourcef (Sources[GUN2], AL_GAIN,     1.0f            );
  alSourcefv(Sources[GUN2], AL_POSITION, SourcesPos[GUN2]);
  alSourcefv(Sources[GUN2], AL_VELOCITY, SourcesVel[GUN2]);
  alSourcei (Sources[GUN2], AL_LOOPING,  AL_FALSE        );
  
  alSourcei (Sources[HELLO], AL_BUFFER,   Buffers[HELLO]   );
  alSourcef (Sources[HELLO], AL_PITCH,    1.0f            );
  alSourcef (Sources[HELLO], AL_GAIN,     1.0f            );
  alSourcefv(Sources[HELLO], AL_POSITION, SourcesPos[HELLO]);
  alSourcefv(Sources[HELLO], AL_VELOCITY, SourcesVel[HELLO]);
  alSourcei (Sources[HELLO], AL_LOOPING,  AL_FALSE        );
  
  alSourcei (Sources[SINE], AL_BUFFER,   Buffers[SINE]   );
  alSourcef (Sources[SINE], AL_PITCH,    1.0f            );
  alSourcef (Sources[SINE], AL_GAIN,     1.0f            );
  alSourcefv(Sources[SINE], AL_POSITION, SourcesPos[SINE]);
  alSourcefv(Sources[SINE], AL_VELOCITY, SourcesVel[SINE]);
  alSourcei (Sources[SINE], AL_LOOPING,  AL_FALSE        );
  
  alSourcei (Sources[SQUARE], AL_BUFFER,   Buffers[SQUARE]   );
  alSourcef (Sources[SQUARE], AL_PITCH,    1.0f            );
  alSourcef (Sources[SQUARE], AL_GAIN,     1.0f            );
  alSourcefv(Sources[SQUARE], AL_POSITION, SourcesPos[SQUARE]);
  alSourcefv(Sources[SQUARE], AL_VELOCITY, SourcesVel[SQUARE]);
  alSourcei (Sources[SQUARE], AL_LOOPING,  AL_FALSE        );
  
  alSourcei (Sources[SAWTOOH], AL_BUFFER,   Buffers[SAWTOOH]   );
  alSourcef (Sources[SAWTOOH], AL_PITCH,    1.0f            );
  alSourcef (Sources[SAWTOOH], AL_GAIN,     1.0f            );
  alSourcefv(Sources[SAWTOOH], AL_POSITION, SourcesPos[SAWTOOH]);
  alSourcefv(Sources[SAWTOOH], AL_VELOCITY, SourcesVel[SAWTOOH]);
  alSourcei (Sources[SAWTOOH], AL_LOOPING,  AL_FALSE        );
  
  alSourcei (Sources[WHITE_NOISE], AL_BUFFER,   Buffers[WHITE_NOISE]   );
  alSourcef (Sources[WHITE_NOISE], AL_PITCH,    1.0f            );
  alSourcef (Sources[WHITE_NOISE], AL_GAIN,     1.0f            );
  alSourcefv(Sources[WHITE_NOISE], AL_POSITION, SourcesPos[WHITE_NOISE]);
  alSourcefv(Sources[WHITE_NOISE], AL_VELOCITY, SourcesVel[WHITE_NOISE]);
  alSourcei (Sources[WHITE_NOISE], AL_LOOPING,  AL_FALSE        );
  
  alSourcei (Sources[IMPULSE], AL_BUFFER,   Buffers[IMPULSE]   );
  alSourcef (Sources[IMPULSE], AL_PITCH,    1.0f            );
  alSourcef (Sources[IMPULSE], AL_GAIN,     1.0f            );
  alSourcefv(Sources[IMPULSE], AL_POSITION, SourcesPos[IMPULSE]);
  alSourcefv(Sources[IMPULSE], AL_VELOCITY, SourcesVel[IMPULSE]);
  alSourcei (Sources[IMPULSE], AL_LOOPING,  AL_FALSE        );

  // Do another error check and return.

  if(alGetError() != AL_NO_ERROR)
    return AL_FALSE;

  return AL_TRUE;
}



/*
 * void SetListenerValues()
 *
 *	We already defined certain values for the listener, but we need
 *	to tell OpenAL to use that data. This function does just that.
 */
void SetListenerValues()
{
  alListenerfv(AL_POSITION,    ListenerPos);
  alListenerfv(AL_VELOCITY,    ListenerVel);
  alListenerfv(AL_ORIENTATION, ListenerOri);
}



/*
 * void KillALData()
 *
 *	We have allocated memory for our buffers and sources which needs
 *	to be returned to the system. This function frees that memory.
 */
void KillALData()
{
  alDeleteBuffers(NUM_BUFFERS, Buffers);
  alDeleteSources(NUM_SOURCES, Sources);
  alutExit();
}



void ajuda() {
  printf(" '1', '2' i '3'  on/off les fonts 1, 2 i 3. El '4' on/off el vehicle.\n\
La posició del oient es canvia en les tecles del cursor o en 'asqz'\n\
Doppler: 'D'/'d' per al factor i 'v'/'V' per a la velocitat\n\n");
}


// ===================================================================
// void display()
// ===================================================================
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;
  glPushMatrix() ;
  glRotatef(20.0,1.0,1.0,0.0) ;

  glPushMatrix() ;
  glTranslatef(SourcesPos[BATTLE][0], SourcesPos[BATTLE][1], SourcesPos[BATTLE][2]) ;
  glColor3f(1.0,0.0,0.0) ; //RED
  if (mute[BATTLE] == 1)
    glutWireCube(0.5);
  else
    glutSolidCube(0.5);
  glPopMatrix() ;

  glPushMatrix() ;
  glTranslatef(SourcesPos[GUN1][0], SourcesPos[GUN1][1], SourcesPos[GUN1][2]) ;
  glColor3f(0.0,1.0,0.0) ; //GREEN
  if (mute[GUN1] == 1)
    glutWireCube(0.5);
  else
    glutSolidCube(0.5); 
  glPopMatrix() ;

  glPushMatrix() ;
  glTranslatef(SourcesPos[GUN2][0], SourcesPos[GUN2][1], SourcesPos[GUN2][2]) ;
  glColor3f(0.0,0.0,1.0) ; //BLUE
  if (mute[GUN2] == 1)
    glutWireCube(0.5);
  else
    glutSolidCube(0.5); 
  glPopMatrix() ;

  glPushMatrix() ;
  glTranslatef(SourcesPos[VEHICLE][0], SourcesPos[VEHICLE][1], SourcesPos[VEHICLE][2]) ;
  glColor3f(0.0,1.0,1.0) ; //DARK BLUE
  if (mute[VEHICLE] == 1)
    glutWireSphere(0.5, 10, 10);
  else
    glutSolidSphere(0.5, 10, 10);
  glPopMatrix() ;
  
  //////nuevas formas geometricas/////////////////////////////////////////
  glPushMatrix();
  glTranslatef(SourcesPos[SINE][0], SourcesPos[SINE][1], SourcesPos[SINE][2]);
  glColor3f(0.1,0.0,0.0); //BROWN
  if(mute[SINE] == 1)
    glutWireIcosahedron();
  else
    glutSolidIcosahedron();
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(SourcesPos[SQUARE][0], SourcesPos[SQUARE][1], SourcesPos[SQUARE][2]);
  glColor3f(1.0,0.5,0.0); //ORANGE
  if(mute[SQUARE] == 1)
    glutWireCone(0.5,1.0,10,10);
  else
    glutSolidCone(0.5,1.0,10,10);
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(SourcesPos[HELLO][0], SourcesPos[HELLO][1], SourcesPos[HELLO][2]);
  glColor3f(1.0,0.0,1.0); //PURPLE 
  if(mute[HELLO] == 1)
    glutWireDodecahedron();
  else
    glutSolidDodecahedron();
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(SourcesPos[SAWTOOH][0], SourcesPos[SAWTOOH][1], SourcesPos[SAWTOOH][2]);
  glColor3f(0.1,0.1,0.0); //BRONZE
  if(mute[SAWTOOH] == 1)
    glutWireOctahedron();
  else
    glutSolidOctahedron();
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(SourcesPos[WHITE_NOISE][0], SourcesPos[WHITE_NOISE][1], SourcesPos[WHITE_NOISE][2]);
  glColor4f(1.0,1.0,1.0,0.0); //WHITE
  if(mute[WHITE_NOISE] == 1)
    glutWireTorus(0.2,0.5,10,10);
  else
    glutSolidTorus(0.2,0.5,10,10);
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(SourcesPos[IMPULSE][0], SourcesPos[IMPULSE][1], SourcesPos[IMPULSE][2]);
  glColor3f(0.0,0.1,0.0); //FOREST GREEN
  if(mute[IMPULSE] == 1)
    glutWireTetrahedron();
  else
    glutSolidTetrahedron();
  glPopMatrix();

  
  //the listener
  glPushMatrix() ;
  glTranslatef(ListenerPos[0], ListenerPos[1], ListenerPos[2]) ;
  glColor3f(1.0,1.0,1.0) ;
  glutWireCube(0.5) ;
  glPopMatrix() ;

  glPopMatrix() ;
  glutSwapBuffers() ;
}

// ===================================================================
// void reshape(int w, int h)
// ===================================================================
void reshape(int w, int h) // the reshape function
{
  glViewport(0,0,(GLsizei)w,(GLsizei)h) ;
  glMatrixMode(GL_PROJECTION) ;
  glLoadIdentity() ;
  gluPerspective(60.0,(GLfloat)w/(GLfloat)h,1.0,30.0) ;
  glMatrixMode(GL_MODELVIEW) ;
  glLoadIdentity() ;
  glTranslatef(0.0,0.0,-6.6) ;
}


// ===================================================================
// void keyboard(int key, int x, int y)
// ===================================================================
void keyboard(unsigned char key, int x, int y) 
{
  ALint state;

  switch(key)
    {
    case 'h':
    case 'H': // Menú: opciones
      ajuda();
      break;

    case '1':
      alGetSourcei(Sources[BATTLE], AL_SOURCE_STATE, &state);
      if(state != AL_PLAYING) {
	alSourcePlay(Sources[BATTLE]);
	mute[BATTLE] = 0;
      }
      else{
	alSourceStop(Sources[BATTLE]);
	mute[BATTLE] = 1;
      }
      break;
      
    case '2':
      alGetSourcei(Sources[GUN1], AL_SOURCE_STATE, &state);
      if(state != AL_PLAYING){
	alSourcePlay(Sources[GUN1]);
	mute[GUN1] = 0;
      }
      else{
	alSourceStop(Sources[GUN1]);
	mute[GUN1] = 1;
      }
      break;
      
    case '3':
      alGetSourcei(Sources[GUN2], AL_SOURCE_STATE, &state);
      if(state != AL_PLAYING){
	alSourcePlay(Sources[GUN2]);
	mute[GUN2] = 0;
      }
      else{
	alSourceStop(Sources[GUN2]);
	mute[GUN2] = 1;
      }
      break;

    case '4':
      alGetSourcei(Sources[VEHICLE], AL_SOURCE_STATE, &state);
      if(state != AL_PLAYING){
	alSourcePlay(Sources[VEHICLE]);
	mute[VEHICLE] = 0;
      }
      else{
	alSourceStop(Sources[VEHICLE]);
	mute[VEHICLE] = 1;
      }
      break;
      
    case '5':
      alGetSourcei(Sources[HELLO], AL_SOURCE_STATE, &state);
      if(state != AL_PLAYING){
	alSourcePlay(Sources[HELLO]);
	mute[HELLO] = 0;
      }
      else{
	alSourceStop(Sources[HELLO]);
	mute[HELLO] = 1;
      }
      break;
      
    case '6':
      alGetSourcei(Sources[SINE], AL_SOURCE_STATE, &state);
      if(state != AL_PLAYING){
	alSourcePlay(Sources[SINE]);
	mute[SINE] = 0;
      }
      else{
	alSourceStop(Sources[SINE]);
	mute[SINE] = 1;
      }
      break;
      
    case '7':
      alGetSourcei(Sources[SQUARE], AL_SOURCE_STATE, &state);
      if(state != AL_PLAYING){
	alSourcePlay(Sources[SQUARE]);
	mute[SQUARE] = 0;
      }
      else{
	alSourceStop(Sources[SQUARE]);
	mute[SQUARE] = 1;
      }
      break;
      
    case '8':
      alGetSourcei(Sources[SAWTOOH], AL_SOURCE_STATE, &state);
      if(state != AL_PLAYING){
	alSourcePlay(Sources[SAWTOOH]);
	mute[SAWTOOH] = 0;
      }
      else{
	alSourceStop(Sources[VEHICLE]);
	mute[SAWTOOH] = 1;
      }
      break;
      
    case '9':
      alGetSourcei(Sources[WHITE_NOISE], AL_SOURCE_STATE, &state);
      if(state != AL_PLAYING){
	alSourcePlay(Sources[WHITE_NOISE]);
	mute[WHITE_NOISE] = 0;
      }
      else{
	alSourceStop(Sources[WHITE_NOISE]);
	mute[WHITE_NOISE] = 1;
      }
      break;
      
    case '0':
      alGetSourcei(Sources[IMPULSE], AL_SOURCE_STATE, &state);
      if(state != AL_PLAYING){
	alSourcePlay(Sources[IMPULSE]);
	mute[IMPULSE] = 0;
      }
      else{
	alSourceStop(Sources[IMPULSE]);
	mute[IMPULSE] = 1;
      }
      break;
    

    case 'f':
    case 'F':
      glutFullScreenToggle();
      break;
      
    case 'a':
    case 'A':
      ListenerPos[0] -= 0.1 ;
      alListenerfv(AL_POSITION, ListenerPos);
      break ;
      
    case 's':
    case 'S':
      ListenerPos[0] += 0.1 ;
      alListenerfv(AL_POSITION, ListenerPos);
      break ;
      
    case 'q':
    case 'Q':
      ListenerPos[2] -= 0.1 ;
      alListenerfv(AL_POSITION, ListenerPos);
      break ;
      
    case 'z':
    case 'Z':
      ListenerPos[2] += 0.1 ;
      alListenerfv(AL_POSITION, ListenerPos);
      break ;

    case 'v':
      velocitatDelMovil -= 10.0;
      SourcesVel[VEHICLE][0] = velocitatDelMovil;
      SourcesVel[VEHICLE][1] = velocitatDelMovil;
      SourcesVel[VEHICLE][2] = velocitatDelMovil;
      alSourcefv(Sources[VEHICLE], AL_VELOCITY, SourcesVel[VEHICLE]);
      break;
      
    case 'V':
      velocitatDelMovil += 10.0;
      SourcesVel[VEHICLE][0] = velocitatDelMovil;
      SourcesVel[VEHICLE][1] = velocitatDelMovil;
      SourcesVel[VEHICLE][2] = velocitatDelMovil;
      alSourcefv(Sources[VEHICLE], AL_VELOCITY, SourcesVel[VEHICLE]);
      break;

    case 'd':
      factorPerAlDoppler -= 0.1;
      alDopplerFactor( factorPerAlDoppler );
      break;

    case 'D':
      factorPerAlDoppler += 0.1;
      alDopplerFactor( factorPerAlDoppler );
      break;

    case 'e':
      velocitatPerAlDoppler -= 1.0;
      alSpeedOfSound( velocitatPerAlDoppler );
      break;

    case 'E':
      velocitatPerAlDoppler += 1.0;
      alSpeedOfSound( velocitatPerAlDoppler );
      break;
   
      
    case 27:
      alSourceStop(Sources[GUN2]);
      alSourceStop(Sources[GUN1]);
      alSourceStop(Sources[BATTLE]);

      alutExit();

      glutLeaveMainLoop(); // End the process	
      glutDestroyWindow(GLwin) ;
      exit(0) ;
      break ;
      
    default: break;
    }
  glutPostRedisplay() ;
}


// ===================================================================
// void specialKeys(int key, int x, int y)
// =================================================================== 
void specialKeys(int key, int x, int y)
{
  switch(key)
    {
    case GLUT_KEY_RIGHT:
      ListenerPos[0] += 0.1 ;
      alListenerfv(AL_POSITION, ListenerPos);
      glutPostRedisplay() ;
      break;
    case GLUT_KEY_LEFT:
      ListenerPos[0] -= 0.1 ;
      alListenerfv(AL_POSITION, ListenerPos);
      glutPostRedisplay() ;
      break;
    case GLUT_KEY_UP:
      ListenerPos[2] -= 0.1 ;
      alListenerfv(AL_POSITION, ListenerPos);
      glutPostRedisplay() ;
      break;
    case GLUT_KEY_DOWN:
      ListenerPos[2] += 0.1 ;
      alListenerfv(AL_POSITION, ListenerPos);
      glutPostRedisplay() ;
      break;
    }
}


int main(int argc, char *argv[])
{
  ALint state, nIteracio;
  double theta;

  printf("Visualizar fuentes de sonido en movimiento.\n\
Basado en:\n\
MindCode's OpenAL Lesson 3: Multiple Sources \n\
y \n\
PIGE-OpenAL. Chad Armstrong on Mon Jul 29 2002.\n\n");
  //    printf("(Press any key to quit.) \n");
  printf("Para ver la asignación de teclas: 'h') \n");


  // Initialize OpenAL and clear the error bit.
  alutInit(NULL, 0);
  alGetError();
  printf("Versió OpenAL: %s\n", alGetString( AL_VERSION ) );
  printf("Versió ALUT: %d.%d\n", alutGetMajorVersion(), alutGetMinorVersion() );
  
  // Load the wav data.
  if(LoadALData() == AL_FALSE)
    return 0;

  SetListenerValues();

  // Setup an exit procedure.
  atexit(KillALData);


  //Inicializar OpenGL y lanzar interfaz gráfico
  //initialise glut
  glutInit(&argc, argv) ;
  //  printf( "Versió GLUT %d\n", glutGet(GLUT_VERSION) );
  printf( "Versió GLUT %d\n", GLUT_API_VERSION ); 

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH) ;   
  glutInitWindowSize(1024,468) ;
  GLwin = glutCreateWindow("Fuentes en movimiento - ejemplo sobre OpenAL + OpenGL") ;
  //init(argv[1], argv[2], argv[3]) ;
  glutDisplayFunc(display) ;
  glutKeyboardFunc(keyboard) ;
  glutSpecialFunc(specialKeys);
  glutReshapeFunc(reshape) ;

  //  glutMainLoop() ;

  nIteracio = 0;

  // Begin the battle sample to play.
  alSourcePlay(Sources[BATTLE]);


  // Go through all the sources and check that they are playing.
  // Skip the first
  // i tammé el segon, que serà el que es meneja en una velocitat variable
  // pegant voltes respecte a l'oritge)
  // source because it is looping anyway (will always be playing).
  while( 1 ) //En mode text, tornar a deixar !kbhit())
    {
      glutMainLoopEvent(); // http://freeglut.sourceforge.net/docs/api.php#EventProcessing

      // factor Doppler == 0 no efecte; >0 el va accentuant; 0.5 ja es nota i a partir d'ell deteriora
      // La relació velocitat Doppler i velocitat del mòbil
      // Valors interesants:      factorPerAlDoppler 0.500000, velocitatPerAlDoppler 212.000000 velocitatDelMovil 10.000000 
      // Valors interesants:      factorPerAlDoppler 0.900000, velocitatPerAlDoppler 80.000000 velocitatDelMovil 10.000000 
      // Menejar l'ambulància en cerles
      // Es el mateix còdic i nIteració en conter de rand
      //nIteracio = (nIteracio + 5 ) % 360;
      nIteracio = (nIteracio + (int)round(SourcesVel[VEHICLE][0])) % 360;      
      printf("nIteracio %d,factorPerAlDoppler %f, velocitatPerAlDoppler %f velocitatDelMovil %f \n",
	     nIteracio, factorPerAlDoppler, velocitatPerAlDoppler, velocitatDelMovil);
      
      
      for(int i = 1; i < NUM_SOURCES; i++)
	{
	  alGetSourcei(Sources[i], AL_SOURCE_STATE, &state);
	  //printf( "Font  %d %d %d\n", i, state, VEHICLE);

	  if ((i == VEHICLE) && (state == AL_PLAYING)) {
		    
		theta = (double) (nIteracio) * 3.14 / 180.0;

	      // L'aparte un poc: DISTANCIA
		SourcesPos[i][0] = -(float)(cos(theta)) * 4.0; // DISTANCIA;
		SourcesPos[i][1] = -0.1 * 5.0; // DISTANCIA;
		SourcesPos[i][2] = -(float)(sin(theta)) * 5.0; // DISTANCIA;
		//printf( " %f, %f, %f\n", SourcesPos[i][0], SourcesPos[i][1], SourcesPos[i][2] );
          }
		  
	  if((i> VEHICLE) && (state != AL_PLAYING)){
		// Pick a random position around the listener to play the source.
		theta = (double) (rand() % 360) * 3.14 / 180.0;

	      // L'aparte un poc: DISTANCIA
		SourcesPos[i][0] = -(float)(cos(theta)) * DISTANCIA;
		SourcesPos[i][1] = -(float)(rand()%2) * DISTANCIA;
 	        SourcesPos[i][2] = -(float)(sin(theta)) * DISTANCIA;
	      }

	      alSourcefv(Sources[i], AL_POSITION, SourcesPos[i]);

	      if ((i> VEHICLE) && (mute[i] == 0))
		alSourcePlay(Sources[i]);
	      else
		alutSleep( 0.1 ); //http://distro.ibiblio.org/rootlinux/rootlinux-ports/more/freealut/freealut-1.1.0/doc/alut.html		      	    

	}
      display();
	    
    }

  
  return 0;
} // Fi de main
