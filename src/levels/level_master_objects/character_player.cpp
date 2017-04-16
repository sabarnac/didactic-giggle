# ifndef TTT3D_LEVEL_MASTER_CHARACTER_PLAYER

	# define TTT3D_LEVEL_MASTER_CHARACTER_PLAYER 1

	# include <glm/glm.hpp>

	# include "../../include/logging_handler.cpp"

	# include "character_base.cpp"

	class levelMasterCharacterPlayer : public levelMasterCharacterBase {

		public:

			levelMasterCharacterPlayer ( inputHandler * inputObject , mat4 modelMatrix = mat4 ( 1.0f ) , loggingHandler * logger = new loggingHandler ) : levelMasterCharacterBase ( "player" , 1 , logger ) {
				this -> inputObject = inputObject ;
				
				this -> logger -> logInfo ( "levelMasterCharacterPlayer : Initializing player character" ) ;
				this -> matrixID = this -> modelObject -> modelShaders -> setShaderVarHandle ( "MVP" ) ;
				this -> textureID  = this -> modelObject -> modelShaders -> setShaderVarHandle ( "textureSampler" ) ;
				this -> alphaID  = this -> modelObject -> modelShaders -> setShaderVarHandle ( "alphaValue" ) ;

				this -> baseModelMatrix = this -> modelMatrix = modelMatrix ;
				this -> modelScale = vec3 ( 0.0f , 0.0f , 0.0f ) ;
				this -> modelPosition = vec3 ( 0.0f , 0.0f , 0.0f ) ;
				this -> modelOrientation = vec3 ( 0.0f , 0.0f , 0.0f ) ;
				this -> alphaVal = 1.0f ;

				this -> logger -> logInfo ( "levelMasterCharacterPlayer : Initialized player character\n" ) ;
			}

			int getType ( ) {
				return 1 ;
			}

			void changeState ( short int stateNo ) {
				if ( ( -- modelObjectsListCount [ this -> fileName ] ) == 0 )
					delete modelObjectsList [ this -> fileName ] ;

				char fileName [ 50 ] ;
				snprintf ( fileName , 50 , "player-%d" , stateNo ) ;

				this -> fileName = fileName ;
				this -> modelObject = modelObjectsHandler :: checkObjects ( fileName ) ;
				if ( this -> modelObject == NULL )
					this -> modelObject = new modelObjectsHandler ( fileName , "basic" , 1 , this -> logger ) ;
				else
					modelObjectsListCount [ fileName ] ++ ;

				this -> setColorAlpha ( 1.0f / ( ( float ) ( ( 2 * stateNo ) - 1 ) ) ) ;
			}

	} ;

#endif