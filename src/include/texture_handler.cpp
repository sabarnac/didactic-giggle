# ifndef GL_MODEL_TEXTURE_HANDLER

	# define GL_MODEL_TEXTURE_HANDLER 1

	# include <string>
	# include <string.h>
	# include <map>
	# include <GL/glew.h>

	# include "logging_handler.cpp"

	using namespace std ;

	struct activeTextureDetails {
		GLuint                                          activeTextureID               ;
		GLuint                                          activeTextureNo               ;
		GLuint                                          activeTextureHandler          ;
	} ;

	class modelTextureHandler ;

	map < string , modelTextureHandler * > modelTexturesList ;
	map < string , int > modelTexturesListCount ;

	class modelTextureHandler {

		protected:
			
			static struct activeTextureDetails          activeTexture                 ;
			GLuint                                      modelTextureID                ;
			string                                      modelName                     ;
			
			loggingHandler                            * logger                        ;

			int loadBMP ( const char * filePath ) {
				unsigned char header [ 54 ] , * data ;
				unsigned int dataPos , imageSize , width , height ;

				FILE *fp ;
				fp = fopen ( filePath , "rb" ) ;
				if ( fp == NULL )
					return ( - 1 ) ;

				if ( fread ( header , 1 , 54 , fp ) != 54 )
					return ( - 1 ) ;
				if ( ( header [ 0 ] != 'B' ) || ( header [ 1 ] != 'M' ) )
					return ( - 1 ) ;
				if ( * ( int * ) & ( header [ 0x1E ] ) != 0 )
					return ( - 1 ) ;
				if ( * ( int * ) & ( header [ 0x1C ] ) != 24 )
					return ( - 1 ) ;

				dataPos = * ( int * ) & ( header [ 0x0A ] ) ;
				imageSize  = * ( int * ) & ( header [ 0x22 ] ) ;
				width = * ( int * ) & ( header [ 0x12 ] ) ;
				height = * ( int * ) & ( header [ 0x16 ] ) ;

				if ( imageSize == 0 ) imageSize = width * height * 3 ;
				if ( dataPos == 0 ) dataPos = 54 ;

				data = new unsigned char [ imageSize ] ;

				fread ( data , 1 , imageSize , fp ) ;

				fclose ( fp ) ;

				glGenTextures ( 1 , & ( this -> modelTextureID ) ) ;
				glBindTexture ( GL_TEXTURE_2D , ( this -> modelTextureID ) ) ;
				glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

				delete data ;

				glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT ) ;
				glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT ) ;
				glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR ) ;
				glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR ) ;
				glGenerateMipmap ( GL_TEXTURE_2D ) ;

				return 0 ;
			}

		public:

			modelTextureHandler ( const char * modelName , loggingHandler * logger = new loggingHandler ) {
				this -> logger = logger ;

				char logMsg [ 200 ] = "modelTextureHandler : Initializing the model texture handler \'" ;
				strcat ( logMsg , modelName ) ;
				strcat ( logMsg , "\'" ) ;
				this -> logger -> logInfo ( logMsg ) ;

				this -> logger -> logInfo ( "modelTextureHandler : Loading texture from file" ) ;
				char textureFilePath [ 500 ] = "assets/models/" ;
				strcat ( textureFilePath , modelName ) ;
				strcat ( textureFilePath , "/texture" ) ;
				this -> logger -> logInfo ( "modelTextureHandler : Loading texture using BMP loader" ) ;
				if ( this -> loadBMP ( textureFilePath ) ) {
					this -> logger -> logInfo ( "modelTextureHandler : Could not load texture. Have to exit\n" ) ;
					exit ( - 1 ) ;
				}

				this -> logger -> logInfo ( "modelTextureHandler : Adding current texture to the list of available textures to use directly" ) ;
				modelTexturesList [ modelName ] = this ;
				modelTexturesListCount [ modelName ] = 1 ;
				this -> modelName = modelName ;

				this -> logger -> logInfo ( "modelTextureHandler : Initialized the model texture handler\n" ) ;
			}

			~modelTextureHandler ( ) {
				glDeleteTextures ( 1 , & ( this -> modelTextureID ) ) ;
				modelTexturesList . erase ( this -> modelName ) ;
				modelTexturesListCount . erase ( this -> modelName ) ;
			}

			void bindTexture ( GLuint textureHandler , unsigned short int textureNo = 0 ) {
				if ( ( ( this -> modelTextureID ) != ( ( modelTextureHandler :: activeTexture ) . activeTextureID ) ) ||
				     ( ( textureNo ) != ( ( modelTextureHandler :: activeTexture ) . activeTextureNo ) ) ||
				     ( ( textureHandler ) != ( ( modelTextureHandler :: activeTexture ) . activeTextureHandler ) )
				) {
					unsigned short int textureBlock = GL_TEXTURE0 + textureNo ;
					glActiveTexture ( textureBlock ) ;
					glBindTexture ( GL_TEXTURE_2D , ( this -> modelTextureID ) ) ;
					glUniform1i ( textureHandler , textureNo ) ;
					modelTextureHandler :: activeTexture = {
						( this -> modelTextureID ) ,
						textureNo ,
						textureHandler
					} ;
				}
			}

			static modelTextureHandler * checkTexture ( const char * modelName ) {
				if ( modelTexturesList . count ( modelName ) == 0 )
					return NULL ;
				else
					return modelTexturesList [ modelName ] ;
			} 

	} ;

	struct activeTextureDetails modelTextureHandler :: activeTexture = { 0 , 0 , 0 } ;

#endif