# ifndef GL_WINDOW_HANDLER

	# define GL_WINDOW_HANDLER 1

	# include <string.h>

	# include <GL/glew.h>
	# include <GLFW/glfw3.h>
	# include <glm/glm.hpp>

	using namespace glm ;

	# include "logging_handler.cpp"

	GLFWwindow                                        * mainWindow ;
	short int                                           windowWidth ;
	short int                                           windowHeight ;
	float                                               aspectRatio ;

	class windowHandler {

		private:
			
			GLFWmonitor                               * primaryMonitor                ;
			GLuint                                      vertexArrayIDs                ;

			short int                                   errorCode                     ;

			loggingHandler                            * logger                        ;

		public:

			windowHandler ( const char * windowTitle , loggingHandler * logger = new loggingHandler ) {
				this -> logger = logger ;

				this -> logger -> logInfo ( "windowHandler : Initializing the main window and its handler" ) ;
				if ( ! glfwInit ( ) ) {
					this -> logger -> logInfo ( "windowHandler : Failed to initialize GLFW. Have to exit\n" ) ;
					exit ( - 1 ) ;
				}

				this -> logger -> logInfo ( "windowHandler : Reading game window related settings from file" ) ;
				char settingType [ 20 ] , settingValue [ 100 ] ;
				int isFullscreen = 0 , windowSamples = 4 , vSyncEnabled = 1 , debuggingEnabled = 0 ;
				FILE * settingsFile = fopen ( "config/settings.cf" , "r" ) ;

				while ( ! feof ( settingsFile ) ) {
					fscanf ( settingsFile , "%s : %s" , settingType , settingValue ) ;
					if ( ! strcmp ( "WINDOW_TYPE" , settingType ) ) {
						if ( ! strcmp ( "FULLSCREEN" , settingValue ) )
							isFullscreen = 1 ;
						else
							isFullscreen = 0 ;
					}
					else if ( ! strcmp ( "WINDOW_WIDTH" , settingType ) ) {
						windowWidth = atoi ( settingValue ) ;
					}
					else if ( ! strcmp ( "WINDOW_HEIGHT" , settingType ) ) {
						windowHeight = atoi ( settingValue ) ;
					}
					else if ( ! strcmp ( "WINDOW_SAMPLES" , settingType ) ) {
						windowSamples = atoi ( settingValue ) ;
					}
					else if ( ! strcmp ( "VSYNC" , settingType ) ) {
						vSyncEnabled = atoi ( settingValue ) ;
					}
					else if ( ! strcmp ( "DEBUG" , settingType ) ) {
						debuggingEnabled = atoi ( settingValue ) ;
					}
				}

				fclose ( settingsFile ) ;

				this -> logger -> logInfo ( "windowHandler : Setting number of samples to be used for multi-sampling and anti-aliasing" ) ;
				glfwWindowHint ( GLFW_SAMPLES , windowSamples ) ;
				this -> logger -> logInfo ( "windowHandler : Setting OpenGL version to be used to 3.3" ) ;
				glfwWindowHint ( GLFW_CONTEXT_VERSION_MAJOR , 3 ) ;
				glfwWindowHint ( GLFW_CONTEXT_VERSION_MINOR , 3 ) ;
				this -> logger -> logInfo ( "windowHandler : Enabling forward compatibility ( Required for Macs apparently )" ) ;
				glfwWindowHint ( GLFW_OPENGL_FORWARD_COMPAT , GL_TRUE ) ;
				this -> logger -> logInfo ( "windowHandler : Setting OpenGL to use the core/main profile" ) ;
				glfwWindowHint ( GLFW_OPENGL_PROFILE , GLFW_OPENGL_CORE_PROFILE ) ;
				glfwWindowHint ( GLFW_RESIZABLE , GL_FALSE ) ;
				glfwWindowHint ( GLFW_OPENGL_DEBUG_CONTEXT , 1 ) ;

				this -> logger -> logInfo ( "windowHandler : Retrieving the primary monitor to be used to show the window in" ) ;
				this -> primaryMonitor = glfwGetPrimaryMonitor ( ) ;
				if ( ( this -> primaryMonitor ) == NULL ) {
					this -> logger -> logInfo ( "windowHandler : Could not retrieve which is the primary monitor. Have to exit\n" ) ;
					exit ( - 1 ) ;
				}

				this -> logger -> logInfo ( "windowHandler : Retrieve the video mode the primary monitor is using" ) ;
				const GLFWvidmode * videoMode = glfwGetVideoMode ( this -> primaryMonitor ) ;
				if ( videoMode == NULL ) {
					this -> logger -> logInfo ( "windowHandler : Could not retrieve the video mode of the primary monitor. Have to exit\n" ) ;
					exit ( - 1 ) ;
				}

				this -> logger -> logInfo ( "windowHandler : Setting the gamma value for the primary monitor" ) ;
				glfwSetGamma ( ( this -> primaryMonitor ) , 1.0 ) ;

				this -> logger -> logInfo ( "windowHandler : Creating a window to use the primary monitor in it's currently set video mode resolution" ) ;
				if ( isFullscreen ) {
					mainWindow = glfwCreateWindow ( ( videoMode -> width ) , ( videoMode -> height ) , windowTitle , ( this -> primaryMonitor ) , NULL ) ;
					windowWidth = videoMode -> width ;
					windowHeight = videoMode -> height ;
				}
				else
					mainWindow = glfwCreateWindow ( windowWidth , windowHeight , windowTitle , NULL , NULL ) ;
				aspectRatio = ( ( float ) windowWidth ) / ( ( float ) windowHeight ) ;
				if ( mainWindow == NULL ) {
					this -> logger -> logInfo ( "windowHandler : Could not create a window. Have to exit\n" ) ;
					exit ( - 1 ) ;
				}

				this -> logger -> logInfo ( "windowHandler : Set the newly created window as the one to be used in context" ) ;
				glfwMakeContextCurrent ( mainWindow ) ;
				glfwSwapInterval ( vSyncEnabled ) ;

				this -> logger -> logInfo ( "windowHandler : Setting GLEW to use the experimental profile ( required )" ) ;
				glewExperimental = true ;

				this -> logger -> logInfo ( "windowHandler : Initializing GLEW" ) ;
				if ( glewInit ( ) != GLEW_OK ) {
					this -> logger -> logInfo ( "windowHandler : Could not initialize GLEW. Have to exit\n" ) ;
					exit ( - 1 ) ;
				}

				if ( glDebugMessageCallback && debuggingEnabled ) {
					this -> logger -> logInfo ( "windowHandler : Enabling OpenGL debugging feature\n" ) ;
					glEnable ( GL_DEBUG_OUTPUT ) ;
					glDebugMessageCallback ( & ( this -> debugOutputCallback ) , NULL ) ;
					GLuint unusedIds = 0 ;
        			glDebugMessageControl ( GL_DONT_CARE , GL_DONT_CARE , GL_DONT_CARE , 0 , & unusedIds , true ) ;
				}

				this -> logger -> logInfo ( "windowHandler : Saving the primary monitor's current video mode's resolution" ) ;

				glGenVertexArrays ( 1 , & ( this -> vertexArrayIDs ) ) ;
				glBindVertexArray ( this -> vertexArrayIDs ) ;

				glViewport ( 0 , 0 , windowWidth , windowHeight ) ;

				this -> logger -> logInfo ( "windowHandler : Initialized the main window and its handler\n" ) ;
			}

			~windowHandler ( ) {
				glDeleteVertexArrays ( 1 , & ( this -> vertexArrayIDs ) ) ;
				glfwDestroyWindow ( mainWindow ) ;
				glfwTerminate ( ) ;
			}

			void setClearScreenColor ( vec4 color ) {
				glClearColor ( color . x , color . y , color . z , color . w ) ;
			}

			void clearScreen ( GLbitfield mask ) {
				glClear ( mask ) ;
			}

			void enableMSAA ( ) {
				glEnable ( GL_MULTISAMPLE ) ; 
			}

			void enableDepthTest ( ) {
				glEnable ( GL_DEPTH_TEST ) ;
				glDepthFunc ( GL_LESS ) ;
			}

			void disableDepthTest ( ) {
				glDisable ( GL_DEPTH_TEST ) ;
			}

			void enableFaceCulling ( ) {
				glEnable ( GL_CULL_FACE ) ;
			}

			void disableFaceCulling ( ) {
				glDisable ( GL_CULL_FACE ) ;
			}

			void enableAlphaBlending ( ) {
				glEnable ( GL_BLEND ) ;
				glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA ) ;
			}

			void disableAlphaBlending ( ) {
				glDisable ( GL_BLEND ) ;
			}

			int getWindowWidth ( ) {
				return windowWidth ;
			}

			int getWindowHeight ( ) {
				return windowHeight ;
			}

			static void APIENTRY debugOutputCallback ( GLenum source , GLenum type , GLuint id , GLenum severity , GLsizei length , const GLchar* message , const void* userParam ) {

				:: logger -> logInfo ( "OpenGL Debug Output message :" ) ;

				if ( source == GL_DEBUG_SOURCE_API_ARB )
					:: logger -> logInfo ( "\tSource : API" ) ;
				else if ( source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB )
					:: logger -> logInfo ( "\tSource : WINDOW_SYSTEM" ) ;
				else if ( source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB )
					:: logger -> logInfo ( "\tSource : SHADER_COMPILER" ) ;
				else if ( source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB )
					:: logger -> logInfo ( "\tSource : THIRD_PARTY" ) ;
				else if ( source == GL_DEBUG_SOURCE_APPLICATION_ARB )
					:: logger -> logInfo ( "\tSource : APPLICATION" ) ;
				else if ( source == GL_DEBUG_SOURCE_OTHER_ARB )
					:: logger -> logInfo ( "\tSource : OTHER" ) ;

				if ( type == GL_DEBUG_TYPE_ERROR_ARB )
					:: logger -> logInfo ( "\tType : ERROR" ) ;
				else if ( type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB )
					:: logger -> logInfo ( "\tType : DEPRECATED_BEHAVIOR" ) ;
				else if ( type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB )
					:: logger -> logInfo ( "\tType : UNDEFINED_BEHAVIOR" ) ;
				else if ( type == GL_DEBUG_TYPE_PORTABILITY_ARB )
					:: logger -> logInfo ( "\tType : PORTABILITY" ) ;
				else if ( type == GL_DEBUG_TYPE_PERFORMANCE_ARB )
					:: logger -> logInfo ( "\tType : PERFORMANCE" ) ;
				else if ( type == GL_DEBUG_TYPE_OTHER_ARB )
					:: logger -> logInfo ( "\tType : OTHER" ) ;

				if ( severity == GL_DEBUG_SEVERITY_HIGH_ARB )
					:: logger -> logInfo ( "\tSeverity : HIGH" ) ;
				else if ( severity == GL_DEBUG_SEVERITY_MEDIUM_ARB )
					:: logger -> logInfo ( "\tSeverity : MEDIUM" ) ;
				else if ( severity == GL_DEBUG_SEVERITY_LOW_ARB )
					:: logger -> logInfo ( "\tSeverity : LOW" ) ;

				char msg [ 10000 ] ;
				snprintf ( msg , 1000 , "\tMessage : %s\n" , message ) ;
				:: logger -> logInfo ( msg ) ;
			}

	} ;

#endif