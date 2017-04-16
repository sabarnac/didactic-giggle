# ifndef GL_MODEL_SHADERS_HANDLER

	# define GL_MODEL_SHADERS_HANDLER 1

	# include <fstream>
	# include <string>
	# include <string.h>
	# include <stdio.h>
	# include <stdlib.h>
	# include <map>
	# include <GL/glew.h>

	# include "logging_handler.cpp"

	using namespace std ;

	struct shaderDetailsInfo {
		char                                         shaderVertexNo [ 100 ]             ;
		char                                         shaderFragmentNo [ 100 ]           ;
	} ;

	class modelShadersHandler ;

	map < string , modelShadersHandler * > modelShadersList ;
	map < string , int > modelShadersListCount ;
	map < string , GLuint > modelShaderUniformHandle ;

	class modelShadersHandler {

		protected:
			
			static GLuint                               activeShadersID               ;
			GLuint                                      modelShadersID                ;
			string                                      shaderName                    ;
			
			loggingHandler                            * logger                        ;

			int loadShaders ( const char * shaderName ) {
				char vertexFilePath [ 500 ] = "assets/shaders/" ;
				strcat ( vertexFilePath , shaderName ) ;
				strcat ( vertexFilePath , "/vShader" ) ;
				char fragmentFilePath [ 500 ] = "assets/shaders/" ;
				strcat ( fragmentFilePath , shaderName ) ;
				strcat ( fragmentFilePath , "/fShader" ) ;

				GLuint vertexShaderID = glCreateShader ( GL_VERTEX_SHADER ) ;
				GLuint fragmentShaderID = glCreateShader ( GL_FRAGMENT_SHADER ) ;

				string vertexShaderCode ;
				ifstream vertexShaderStream ( vertexFilePath , ios :: in ) ;
				if ( vertexShaderStream . is_open ( ) ) {
					string line = "" ;
					while ( getline ( vertexShaderStream , line ) )
						vertexShaderCode += "\n" + line ;
					vertexShaderStream . close ( ) ;
				}
				else
					return ( - 1 ) ;

				string fragmentShaderCode ;
				ifstream fragmentShaderStream ( fragmentFilePath , ios :: in ) ;
				if ( fragmentShaderStream . is_open ( ) ) {
					string line = "" ;
					while ( getline ( fragmentShaderStream , line ) )
						fragmentShaderCode += "\n" + line ;
					fragmentShaderStream . close ( ) ;
				}
				else
					return ( - 1 ) ;

				GLint result = GL_FALSE ;
				int infoLogLength ;

				char const * vertexSourcePointer = vertexShaderCode . c_str ( ) ;
				glShaderSource ( vertexShaderID , 1 , & vertexSourcePointer , NULL ) ;
				glCompileShader ( vertexShaderID ) ;

				glGetShaderiv ( vertexShaderID , GL_COMPILE_STATUS , & result ) ;
				glGetShaderiv ( vertexShaderID , GL_INFO_LOG_LENGTH , & infoLogLength ) ;
				if ( infoLogLength > 1 )
					return ( - 1 ) ;

				char const * fragmentSourcePointer = fragmentShaderCode . c_str ( ) ;
				glShaderSource ( fragmentShaderID , 1 , & fragmentSourcePointer , NULL ) ;
				glCompileShader ( fragmentShaderID ) ;

				glGetShaderiv ( fragmentShaderID , GL_COMPILE_STATUS , & result ) ;
				glGetShaderiv ( fragmentShaderID , GL_INFO_LOG_LENGTH , & infoLogLength ) ;
				if ( infoLogLength > 1 )
					return ( - 1 ) ;

				this -> modelShadersID = glCreateProgram ( ) ;
				glAttachShader ( ( this -> modelShadersID ) , vertexShaderID ) ;
				glAttachShader ( ( this -> modelShadersID ) , fragmentShaderID ) ;
				glLinkProgram ( this -> modelShadersID ) ;

				glGetProgramiv ( ( this -> modelShadersID ) , GL_LINK_STATUS , & result ) ;
				glGetProgramiv ( ( this -> modelShadersID ) , GL_INFO_LOG_LENGTH , & infoLogLength ) ;
				if ( infoLogLength > 1 )
					return ( - 1 ) ;

				glDetachShader ( ( this -> modelShadersID ) , vertexShaderID ) ;
				glDetachShader ( ( this -> modelShadersID ) , fragmentShaderID ) ;
				
				glDeleteShader ( vertexShaderID ) ;
				glDeleteShader ( fragmentShaderID ) ;

				return 0 ;
			}

		public:

			modelShadersHandler ( const char * shaderName , loggingHandler * logger = new loggingHandler ) {
				this -> logger = logger ;

				char logMsg [ 200 ] = "modelShadersHandler : Initializing the model shaders handler \'" ;
				strcat ( logMsg , shaderName ) ;
				strcat ( logMsg , "\'" ) ;
				this -> logger -> logInfo ( logMsg ) ;

				this -> logger -> logInfo ( "modelShadersHandler : Loading shaders from files" ) ;
				if ( this -> loadShaders ( shaderName ) ) {
					this -> logger -> logInfo ( "modelShadersHandler : Could not load shaders. Have to exit\n" ) ;
					exit ( - 1 ) ;
				}

				this -> logger -> logInfo ( "modelShadersHandler : Adding current shaders to the list of available shaders to use directly" ) ;
				modelShadersList [ shaderName ] = this ;
				modelShadersListCount [ shaderName ] = 1 ;
				this -> shaderName = shaderName ;

				this -> logger -> logInfo ( "modelShadersHandler : Initialized the model shaders handler\n" ) ;
			}

			~modelShadersHandler ( ) {
				glDeleteProgram ( this -> modelShadersID ) ;
				modelShadersList . erase ( this -> shaderName ) ;
				modelShadersListCount . erase ( this -> shaderName ) ;
			}

			void useShader ( ) {
				if ( ( this -> modelShadersID ) != ( modelShadersHandler :: activeShadersID ) ) {
					glUseProgram ( this -> modelShadersID ) ;
					modelShadersHandler :: activeShadersID = this -> modelShadersID ; 
				}
			}

			GLuint setShaderVarHandle ( const char * varName ) {
				if ( modelShaderUniformHandle . count ( varName ) == 0 ) {
					GLuint tmp = glGetUniformLocation ( ( this -> modelShadersID ) , varName ) ;
					modelShaderUniformHandle [ varName ] = tmp ;
					return tmp ;
				}
				else
					return modelShaderUniformHandle [ varName ] ;
			}

			static modelShadersHandler * checkShaders ( const char * shaderName ) {
				if ( modelShadersList . count ( shaderName ) == 0 )
					return NULL ;
				else
					return modelShadersList [ shaderName ] ;
			} 

	} ;

	GLuint modelShadersHandler :: activeShadersID = 0 ;

#endif