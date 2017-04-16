# ifndef TTT3D_LEVEL_MASTER_CHARACTER_OBSTACLE

	# define TTT3D_LEVEL_MASTER_CHARACTER_OBSTACLE 1

	# include "../../include/logging_handler.cpp"

	# include "character_base.cpp"

	struct obstacleCharacterProperties {
		vec2                                            startPoint                   ;
		vec2                                            endPoint                     ;
		float                                           speed                        ;
		float                                           growthScale                  ;
	} ;

	class levelMasterCharacterObstacle : public levelMasterCharacterBase {

		public:

			struct obstacleCharacterProperties          characterProperties           ;
			levelMasterCharacterObstacle              * nextCharacter                 ;
			bool                                        destroyCharacter              ;

			levelMasterCharacterObstacle ( struct obstacleCharacterProperties setProperties , inputHandler * inputObject , mat4 modelMatrix = mat4 ( 1.0f ) , loggingHandler * logger = new loggingHandler ) : levelMasterCharacterBase ( "obstacle" , 1 , logger ) {
				this -> inputObject = inputObject ;
				
				this -> logger -> logInfo ( "levelMasterCharacterObstacle : Initializing obstacle character" ) ;
				this -> matrixID = this -> modelObject -> modelShaders -> setShaderVarHandle ( "MVP" ) ;
				this -> textureID  = this -> modelObject -> modelShaders -> setShaderVarHandle ( "textureSampler" ) ;
				this -> alphaID  = this -> modelObject -> modelShaders -> setShaderVarHandle ( "alphaValue" ) ;

				this -> baseModelMatrix = this -> modelMatrix = modelMatrix ;
				this -> characterProperties = setProperties ;
				this -> modelScale = vec3 ( 0.0f , 0.0f , 0.0f ) ;
				this -> modelPosition = vec3 ( ( setProperties . startPoint . x ) , ( setProperties . startPoint . y ) , 0.0f ) ;
				this -> modelOrientation = vec3 ( 0.0f , 0.0f , 0.0f ) ;
				this -> alphaVal = 1.0f ;
				this -> destroyCharacter = false ;
				this -> nextCharacter = NULL ;

				this -> logger -> logInfo ( "levelMasterCharacterObstacle : Initialized obstacle character\n" ) ;
			}

			void translateModel ( vec3 translationFactor ) {
				this -> modelPosition = ( this -> modelPosition ) + translationFactor ;
				this -> modelPosition = clamp ( ( this -> modelPosition ) , vec3 ( ( ( - ( aspectRatio * 1.0f ) ) - ( ( this -> modelScale ) . x ) ) , ( ( - 1.0f ) - ( ( this -> modelScale ) . y ) ) , ( ( - 1.0f ) - ( ( this -> modelScale ) . z ) ) ) , vec3 ( ( ( aspectRatio * 1.0f ) + ( ( this -> modelScale ) . x ) ) , ( 1.0f + ( ( this -> modelScale ) . y ) ) , ( 1.0f + ( ( this -> modelScale ) . z ) ) ) ) ;
			}

			int getType ( ) {
				return 3 ;
			}

	} ;

#endif