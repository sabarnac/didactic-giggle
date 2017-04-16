# ifndef TTT3D_LEVEL_MASTER_CHARACTER_BASE

	# define TTT3D_LEVEL_MASTER_CHARACTER_BASE 1

	# include <glm/glm.hpp>
	# include <glm/gtc/matrix_transform.hpp>

	# include "../../include/models_handler.cpp"

	class levelMasterCharacterBase {

		protected:
			
			inputHandler                              * inputObject                   ;
			modelObjectsHandler                       * modelObject                   ;
			GLuint                                      matrixID                      ;
			GLuint                                      textureID                     ;
			GLuint                                      alphaID                       ;
			mat4                                        baseModelMatrix               ;
			mat4                                        modelMatrix                   ;
			string                                      fileName                      ;
			float                                       alphaVal                      ;
			vec3                                        modelScale                    ;
			vec3                                        modelPosition                 ;
			vec3                                        modelOrientation              ;

			loggingHandler                            * logger                        ;

		public:

			levelMasterCharacterBase ( const char * fileName , short int modelType = 1 , loggingHandler * logger = new loggingHandler ) {
				this -> logger = logger ;

				this -> logger -> logInfo ( "levelMasterCharacterBase : Initializing character base" ) ;
				this -> fileName = fileName ;
				this -> logger -> logInfo ( "levelMasterCharacterBase : Setting character object" ) ;
				this -> modelObject = modelObjectsHandler :: checkObjects ( fileName ) ;
				if ( this -> modelObject == NULL )
					this -> modelObject = new modelObjectsHandler ( fileName , "basic" , modelType , logger ) ;
				else
					modelObjectsListCount [ fileName ] ++ ;
				
				this -> logger -> logInfo ( "levelMasterCharacterBase : Initialized character base\n" ) ;
			}

			~levelMasterCharacterBase ( ) {
				if ( ( -- modelObjectsListCount [ this -> fileName ] ) == 0 )
					delete modelObjectsList [ this -> fileName ] ;
			}

			virtual void resetModelMatrix ( ) {
				this -> modelMatrix = this -> baseModelMatrix ;
			}

			virtual void setModelMatrix ( mat4 newModelMatrix ) {
				this -> modelMatrix = newModelMatrix ;
			}

			virtual void scaleModel ( vec3 scaleFactor ) {
				this -> modelScale = ( this -> modelScale ) + scaleFactor ;
				this -> modelScale = clamp ( ( this -> modelScale ) , vec3 ( ( - 100.0f ) , ( - 100.0 ) , ( - 100.0f ) ) , vec3 ( 100.0f , 100.0f , 100.0f ) ) ;
				this -> translateModel ( vec3 ( 0.0f , 0.0f , 0.0f ) ) ;
			}

			virtual vec3 getModelScale ( ) {
				return this -> modelScale ;
			}

			virtual void rotateModel ( vec3 rotateFactor ) {
				this -> modelOrientation = ( this -> modelOrientation ) + rotateFactor ;
				if ( ( this -> modelOrientation ) . x >= 360.0f )
					( this -> modelOrientation ) . x = ( ( this -> modelOrientation ) . x ) - 360 ;

				if ( ( this -> modelOrientation ) . y >= 360.0f )
					( this -> modelOrientation ) . y = ( ( this -> modelOrientation ) . y ) - 360 ;

				if ( ( this -> modelOrientation ) . z >= 360.0f )
					( this -> modelOrientation ) . z = ( ( this -> modelOrientation ) . z ) - 360 ;
			}

			virtual vec3 getModelOrientation ( ) {
				return this -> modelOrientation ;
			}

			virtual void translateModel ( vec3 translationFactor ) {
				this -> modelPosition = ( this -> modelPosition ) + translationFactor ;
				this -> modelPosition = clamp ( ( this -> modelPosition ) , vec3 ( ( ( - ( aspectRatio * 0.95f ) ) + ( ( this -> modelScale ) . x ) ) , ( ( - 0.95f ) + ( ( this -> modelScale ) . y ) ) , ( ( - 0.95f ) + ( ( this -> modelScale ) . z ) ) ) , vec3 ( ( ( aspectRatio * 0.95f ) - ( ( this -> modelScale ) . x ) ) , ( 0.95f - ( ( this -> modelScale ) . y ) ) , ( 0.95f - ( ( this -> modelScale ) . z ) ) ) ) ;
			}

			virtual vec3 getModelPosition ( ) {
				return this -> modelPosition ;
			}

			virtual void setColorAlpha ( float alphaValue ) {
				this -> alphaVal = alphaValue ;
			}

			virtual float getColorAlpha ( ) {
				return this -> alphaVal ;
			}

			virtual void updateAndDraw ( ) {
				this -> modelObject -> modelShaders -> useShader ( ) ;

				this -> modelObject -> modelTexture -> bindTexture ( ( this -> textureID ) , 0 ) ;
				
				glUniform1f ( ( this -> alphaID ) , ( this -> alphaVal ) ) ;
				mat4 mvpMatrix = translate ( ( this -> modelMatrix ) , ( this -> modelPosition ) ) ;
				mvpMatrix = rotate ( mvpMatrix , radians ( ( this -> modelOrientation ) . x ) , vec3 ( 1 , 0 , 0 ) ) ;
				mvpMatrix = rotate ( mvpMatrix , radians ( ( this -> modelOrientation ) . y ) , vec3 ( 0 , 1 , 0 ) ) ;
				mvpMatrix = rotate ( mvpMatrix , radians ( ( this -> modelOrientation ) . z ) , vec3 ( 0 , 0 , 1 ) ) ;
				mvpMatrix = scale ( mvpMatrix , ( this -> modelScale ) ) ;
				mvpMatrix = ( this -> inputObject -> getProjectionMatrix ( ) ) * mvpMatrix ;
				glUniformMatrix4fv ( ( this -> matrixID ) , 1 , GL_FALSE , & ( mvpMatrix [ 0 ] [ 0 ] ) ) ;

				this -> modelObject -> bindBuffers ( ) ;

				this -> modelObject -> drawObject ( ) ;
			}

			virtual int getType ( ) {
				return 0 ;
			}

	} ;

#endif