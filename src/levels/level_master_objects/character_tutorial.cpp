# ifndef TTT3D_LEVEL_MASTER_CHARACTER_TUTORIAL

	# define TTT3D_LEVEL_MASTER_CHARACTER_TUTORIAL 1

	# include <glm/glm.hpp>

	# include "../../include/logging_handler.cpp"

	# include "character_base.cpp"

	class levelMasterCharacterTutorial : public levelMasterCharacterBase {

		public:

			levelMasterCharacterTutorial ( inputHandler * inputObject , mat4 modelMatrix = mat4 ( 1.0f ) , loggingHandler * logger = new loggingHandler ) : levelMasterCharacterBase ( "tutorial" , 0 , logger ) {
				this -> inputObject = inputObject ;

				this -> logger -> logInfo ( "levelMasterCharacterTutorial : Initializing tutorial character" ) ;
				this -> matrixID = this -> modelObject -> modelShaders -> setShaderVarHandle ( "MVP" ) ;
				this -> textureID  = this -> modelObject -> modelShaders -> setShaderVarHandle ( "textureSampler" ) ;
				this -> alphaID  = this -> modelObject -> modelShaders -> setShaderVarHandle ( "alphaValue" ) ;

				this -> baseModelMatrix = this -> modelMatrix = modelMatrix ;
				this -> modelScale = vec3 ( 0.1f , 0.1f , 0.0f ) ;
				this -> modelPosition = vec3 ( ( - 0.85f ) , ( - 0.75f ) , 0.0f ) ;
				this -> modelOrientation = vec3 ( 0.0f , 0.0f , 0.0f ) ;
				this -> alphaVal = 0.0f ;

				this -> logger -> logInfo ( "levelMasterCharacterTutorial : Initialized tutorial character\n" ) ;
			}

			int getType ( ) {
				return 4 ;
			}

	} ;

	class levelMasterCharacterTutorial2 : public levelMasterCharacterBase {

		public:

			levelMasterCharacterTutorial2 ( inputHandler * inputObject , mat4 modelMatrix = mat4 ( 1.0f ) , loggingHandler * logger = new loggingHandler ) : levelMasterCharacterBase ( "tutorial2" , 0 , logger ) {
				this -> inputObject = inputObject ;

				this -> logger -> logInfo ( "levelMasterCharacterTutorial2 : Initializing tutorial character" ) ;
				this -> matrixID = this -> modelObject -> modelShaders -> setShaderVarHandle ( "MVP" ) ;
				this -> textureID  = this -> modelObject -> modelShaders -> setShaderVarHandle ( "textureSampler" ) ;
				this -> alphaID  = this -> modelObject -> modelShaders -> setShaderVarHandle ( "alphaValue" ) ;

				this -> baseModelMatrix = this -> modelMatrix = modelMatrix ;
				this -> modelScale = vec3 ( 0.1f , 0.1f , 0.0f ) ;
				this -> modelPosition = vec3 ( 0.85f , ( - 0.75f ) , 0.0f ) ;
				this -> modelOrientation = vec3 ( 0.0f , 0.0f , 0.0f ) ;
				this -> alphaVal = 0.0f ;

				this -> logger -> logInfo ( "levelMasterCharacterTutorial2 : Initialized tutorial character\n" ) ;
			}

			int getType ( ) {
				return 4 ;
			}

	} ;

#endif