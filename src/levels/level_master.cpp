# ifndef TTT3D_LEVEL_MASTER

	# define TTT3D_LEVEL_MASTER 1

	# include <random>
	# include <time.h>
	# include <unistd.h>
	# include <pthread.h>
	# include <sys/types.h>
	# include <sys/stat.h>
	# include <stdlib.h>
	# include <AL/al.h>
	# include <AL/alc.h>
	# include <glm/glm.hpp>
	# include <glm/gtx/string_cast.hpp>

	# include "../include/logging_handler.cpp"
	# include "../include/fps_counter.cpp"

	# include "level_base.cpp"

	# include "level_master_objects/character_base.cpp"
	# include "level_master_objects/character_player.cpp"
	# include "level_master_objects/character_achievement.cpp"
	# include "level_master_objects/character_obstacle.cpp"
	# include "level_master_objects/character_tutorial.cpp"

	using namespace std ;
	using namespace glm ;

	struct playerProperties {
		bool                                               showTutorial , tutorialEnabled , switchAlpha ;
		unsigned short int                                 playerStrength ;
		float                                              secondIncrement , secondIncrement2 ;
		float                                              secondIncrementGrowth , secondIncrement2Growth ;
		vec3                                               characterScale ;
		float                                              growthScale , scaleIncrement ;
		vec3                                               characterSpeed , constSpeed ;
		float                                              growthSpeed , speedIncrement ;
	} ;

	struct achievementProperties {
		float                                              secondIncrement , secondIncrement2 ;
		float                                              secondIncrementGrowth , secondIncrement2Growth ;
		float                                              speedGrowth , scaleGrowth ;
		struct achievementCharacterProperties              characterProperties ;
	} ;

	struct obstacleProperties {
		float                                              secondIncrement , secondIncrement2 ;
		float                                              secondIncrementGrowth , secondIncrement2Growth ;
		float                                              speedGrowth , scaleGrowth ;
		struct obstacleCharacterProperties                 characterProperties ;
	} ;

	class levelMaster : public levelBase {

		protected:

			fpsCounter                                   * fpsObject                     ;
			levelMasterCharacterPlayer                   * playerCharacter               ;
			levelMasterCharacterObstacle                 * obstacleCharacter             ;
			levelMasterCharacterAchievement              * achievementCharacter          ;

			levelMasterCharacterTutorial                 * tutorialCharacter             ;
			levelMasterCharacterTutorial2                * tutorialCharacter2            ;

			pthread_t                                      playerThread                  ;
			pthread_t                                      obstacleThread                ;
			pthread_t                                      achievementThread             ;

			short int                                      endThreads                    ;
			bool                                           pauseThreads                  ;
			bool                                           pauseNonPlayers               ;
			bool                                           pausePlayer                   ;
			bool                                           endgameMode                   ;

			struct playerProperties                        playerValues                  ;
			struct achievementProperties                   achievementValues             ;
			struct obstacleProperties                      obstacleValues                ;

			short int                                      playerAchieved                ;
			short int                                      playerObstacled               ;

			short int                                      gameRounds                    ;

			mt19937                                      * rndGenerator                  ;

			void initializeCharacters ( float gameRoundNumber = 1.0f ) {
				this -> logger -> logInfo ( "levelMaster : Initializing characters" ) ;
				
				this -> tutorialCharacter = new levelMasterCharacterTutorial ( ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
				this -> tutorialCharacter -> setColorAlpha ( 0.0f ) ;
				this -> tutorialCharacter2 = new levelMasterCharacterTutorial2 ( ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
				this -> tutorialCharacter2 -> setColorAlpha ( 0.0f ) ;

				this -> obstacleValues = {
					10.0f , 30.0f ,
					( 3.0f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) , ( 7.5f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) ,
					( 0.4f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) , ( 1.25f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) ,
					{ vec2 ( 0.0f , 0.2f ) , vec2 ( 0.0f , 0.0f ) , 0.0f , 0.0001f }
				} ;
				this -> obstacleCharacter = new levelMasterCharacterObstacle ( ( this -> obstacleValues ) . characterProperties , ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
				this -> obstacleCharacter -> setColorAlpha ( 1.0f ) ;
				this -> obstacleValues = {
					10.0f , 30.0f ,
					( 3.0f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) , ( 7.5f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) ,
					( 0.4f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) , ( 1.25f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) ,
					{ vec2 ( 0.0f , 0.0f ) , vec2 ( 0.0f , 0.0f ) , 0.25f , 0.0001f }
				} ;
				this -> playerObstacled = 0 ;

				this -> achievementValues = {
					10.0f , 30.0f ,
					( 3.0f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) , ( 7.5f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) ,
					( 0.4f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) , ( 1.25f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) ,
					{ vec2 ( 0.0f , 0.1f ) , vec2 ( 0.0f , 0.0f ) , 0.0f , 0.0001f }
				} ;
				this -> achievementCharacter = new levelMasterCharacterAchievement ( ( this -> achievementValues ) . characterProperties , ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
				this -> achievementCharacter -> setColorAlpha ( 1.0f ) ;
				this -> achievementValues = {
					10.0f , 30.0f ,
					( 3.0f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) , ( 7.5f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) ,
					( 0.4f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) , ( 1.25f * ( ( 26.0f - gameRoundNumber ) / 25.0f ) ) ,
					{ vec2 ( 0.0f , 0.0f ) , vec2 ( 0.0f , 0.0f ) , 0.225f , 0.0001f }
				} ;

				this -> playerCharacter = new levelMasterCharacterPlayer ( ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
				this -> playerCharacter -> setColorAlpha ( 0.0f ) ;
				this -> playerCharacter -> scaleModel ( vec3 ( 0.01f , 0.01f , 0.01f ) ) ;
				this -> playerCharacter -> translateModel ( vec3 ( 0.0f , ( - 0.94f ) , 0.0f ) ) ;
				this -> playerValues = {
					false , true , false ,
					( ( short unsigned int ) ( 1 * ( ( short unsigned int ) gameRoundNumber ) ) ) ,
					45.0f , 135.0f ,
					( 3.0f + log2 ( gameRoundNumber ) ) , ( 10.0f + log2 ( gameRoundNumber ) ) ,
					vec3 ( 0.0001f , 0.0001f , 0.0001f ) ,
					0.0001f , 1.1f ,
					vec3 ( 4.0f , 3.0f , 0.0f ) , vec3 ( 0.0f , ( - 0.05f ) , 0.0f ) ,
					( - 0.05f ) , 1.1f
				} ;
				this -> pausePlayer = false ;

				this -> logger -> logInfo ( "levelMaster : Initialized characters" ) ;
			}

			void initializeCharacters_endgame ( float gameRoundNumber = 1.0f ) {
				this -> logger -> logInfo ( "levelMaster : Initializing characters" ) ;
				this -> tutorialCharacter = NULL ;
				this -> tutorialCharacter2 = NULL ;

				this -> obstacleValues = {
					10.0f , 30.0f ,
					( 3.0f + log2 ( gameRoundNumber ) ) , ( 7.5f + log2 ( gameRoundNumber ) ) ,
					( 0.4f + ( log2 ( gameRoundNumber ) / 3 ) ) , ( 1.25f + ( log2 ( gameRoundNumber ) / 3 ) ) ,
					{ vec2 ( 0.0f , 0.2f ) , vec2 ( 0.0f , 0.0f ) , 0.0f , 0.00015f }
				} ;
				this -> obstacleCharacter = new levelMasterCharacterObstacle ( ( this -> obstacleValues ) . characterProperties , ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
				this -> obstacleCharacter -> setColorAlpha ( 1.0f ) ;
				this -> obstacleValues = {
					10.0f , 30.0f ,
					( 3.0f + log2 ( gameRoundNumber ) ) , ( 7.5f + log2 ( gameRoundNumber ) ) ,
					( 0.4f + log2 ( gameRoundNumber ) ) , ( 1.25f + log2 ( gameRoundNumber ) ) ,
					{ vec2 ( 0.0f , 0.02f ) , vec2 ( 0.0f , 0.0f ) , 0.25f , 0.00015f }
				} ;
				this -> playerObstacled = 0 ;

				this -> achievementValues = {
					10.0f , 30.0f ,
					( 3.0f * ( ( 16.0f - gameRoundNumber ) / 15.0f ) ) , ( 7.5f * ( ( 16.0f - gameRoundNumber ) / 15.0f ) ) ,
					( 0.4f * ( ( 16.0f - gameRoundNumber ) / 15.0f ) ) , ( 1.25f * ( ( 16.0f - gameRoundNumber ) / 15.0f ) ) ,
					{ vec2 ( 0.0f , 0.1f ) , vec2 ( 0.0f , 0.0f ) , 0.0f , 0.0001f }
				} ;
				this -> achievementCharacter = new levelMasterCharacterAchievement ( ( this -> achievementValues ) . characterProperties , ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
				this -> achievementCharacter -> setColorAlpha ( 1.0f ) ;
				this -> achievementValues = {
					10.0f , 30.0f ,
					( 3.0f * ( ( 16.0f - gameRoundNumber ) / 15.0f ) ) , ( 7.5f * ( ( 16.0f - gameRoundNumber ) / 15.0f ) ) ,
					( 0.4f * ( ( 16.0f - gameRoundNumber ) / 15.0f ) ) , ( 1.25f * ( ( 16.0f - gameRoundNumber ) / 15.0f ) ) ,
					{ vec2 ( 0.0f , 0.0f ) , vec2 ( 0.0f , 0.0f ) , 0.225f , 0.0001f }
				} ;

				this -> playerCharacter = new levelMasterCharacterPlayer ( ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
				this -> playerCharacter -> setColorAlpha ( 0.0f ) ;
				this -> playerCharacter -> scaleModel ( vec3 ( 0.01f , 0.01f , 0.01f ) ) ;
				this -> playerCharacter -> translateModel ( vec3 ( 0.0f , ( - 0.94f ) , 0.0f ) ) ;
				this -> playerValues = {
					false , false , false ,
					1 ,
					45.0f , 135.0f ,
					( 3.33333f * ( ( 11.0f - gameRoundNumber ) / 10.0f ) ) , ( 10.0f * ( ( 11.0f - gameRoundNumber ) / 10.0f ) ) ,
					vec3 ( ( 0.000075f * ( ( 11.0f - gameRoundNumber ) / 10.0f ) ) , ( 0.000075f * ( ( 11.0f - gameRoundNumber ) / 10.0f ) ) , ( 0.000075f * ( ( 11.0f - gameRoundNumber ) / 10.0f ) ) ) ,
					( 0.000075f * ( ( 11.0f - gameRoundNumber ) / 10.0f ) ) , 1.01f ,
					vec3 ( 4.0f , 3.0f , 0.0f ) , vec3 ( 0.0f , ( - 0.05f ) , 0.0f ) ,
					( - 0.0125f ) , 1.01f
				} ;

				this -> logger -> logInfo ( "levelMaster : Initialized characters" ) ;
			}

			void destroyCharacters ( ) {
				this -> logger -> logInfo ( "levelMaster : Destroying characters" ) ;
				levelMasterCharacterObstacle * tmpCharacter1 = NULL ;
				levelMasterCharacterAchievement * tmpCharacter2 = NULL ;

				if ( ( this -> tutorialCharacter ) != NULL )
					delete this -> tutorialCharacter ;
				if ( ( this -> tutorialCharacter2 ) != NULL )
					delete this -> tutorialCharacter2 ;

				while ( ( this -> achievementCharacter ) != NULL ) {
					tmpCharacter2 = this -> achievementCharacter ;
					this -> achievementCharacter = this -> achievementCharacter -> nextCharacter ;
					delete tmpCharacter2 ;
				}

				while ( ( this -> obstacleCharacter ) != NULL ) {
					tmpCharacter1 = this -> obstacleCharacter ;
					this -> obstacleCharacter = this -> obstacleCharacter -> nextCharacter ;
					delete tmpCharacter1 ;
				}
				
				delete this -> playerCharacter ;

				this -> logger -> logInfo ( "levelMaster : Destroyed characters" ) ;
			}

			void drawCharacters ( ) {
				levelMasterCharacterObstacle * tmpCharacter1 = NULL ;
				levelMasterCharacterAchievement * tmpCharacter2 = NULL ;

				if ( ( this -> tutorialCharacter ) != NULL )
					this -> tutorialCharacter -> updateAndDraw ( ) ;
				if ( ( this -> tutorialCharacter2 ) != NULL )
					this -> tutorialCharacter2 -> updateAndDraw ( ) ;

				tmpCharacter2 = this -> achievementCharacter ;
				while ( ( tmpCharacter2 -> nextCharacter ) != NULL ) {
					tmpCharacter2 -> updateAndDraw ( ) ;
					tmpCharacter2 = tmpCharacter2 -> nextCharacter ;
				}

				tmpCharacter1 = this -> obstacleCharacter ;
				while ( ( tmpCharacter1 -> nextCharacter ) != NULL ) {
					tmpCharacter1 -> updateAndDraw ( ) ;
					tmpCharacter1 = tmpCharacter1 -> nextCharacter ;
				}

				this -> playerCharacter -> updateAndDraw ( ) ;
			}

			int checkInputs ( ) {
				static int checkEscape = 0 , checkSelection ;
				checkSelection = 0 ;

				if ( this -> inputObject -> checkKeyPressed ( GLFW_KEY_ESCAPE ) && ( checkEscape == 0 ) )
					checkEscape = 1 ;
				else if ( ( ( this -> inputObject -> checkKeyReleased ( GLFW_KEY_ESCAPE ) ) && ( checkEscape == 1 ) ) || ( this -> inputObject -> checkWindowBeingClosed ( ) == 1 ) ) {
					checkEscape = 0 ;
					checkSelection = ( - 1 ) ;
				}

				return checkSelection ;
			}

			void playerThreadFunction ( ) {
				int frameCount = 0 ;
				time_t startTime ;
				double frameTime ;
				long long secondCount = 0 ;
				vec3 deltaOrientation , deltaScale , deltaSpeed ;
				float deltaTime = 0.005f , alphaVal = 0.0f ;
				timespec sleepTime = { 0 , 5000000 } ;
				this -> logger -> logInfo ( "playerThread : Started player thread" ) ;
				this -> endThreads ++ ;
				do {
					nanosleep ( & sleepTime , NULL ) ;
					startTime = time ( 0 ) ;
					if ( ( this -> endThreads ) == 0 )
						return ;
					if ( ( this -> pauseThreads ) || ( this -> pausePlayer ) )
						continue ;
					frameCount ++ ;
					if ( frameCount >= 200 ) {
						frameCount = 0 ;
						secondCount ++ ;
					}
					if ( ( secondCount == 15 ) && ( this -> endgameMode ) )
						( this -> playerValues ) . playerStrength = 0 ;
					if ( ( secondCount == 3 ) && ( ( this -> playerValues ) . tutorialEnabled ) ) {
						( this -> playerValues ) . tutorialEnabled = false ;
						( this -> playerValues ) . showTutorial = true ;
					}
					if ( ( this -> playerValues ) . showTutorial ) {
						secondCount = frameCount = 1 ;
						alphaVal += 1.0f * deltaTime ;
						if ( alphaVal >= 1.0f ) {
							( this -> playerValues ) . switchAlpha = ! ( ( this -> playerValues ) . switchAlpha ) ;
							alphaVal -= 1.0f ;
						}
						if ( ( this -> playerValues ) . switchAlpha ) {
							this -> tutorialCharacter -> setColorAlpha ( 1.0f - alphaVal ) ;
							this -> tutorialCharacter2 -> setColorAlpha ( 1.0f - alphaVal ) ;
						}
						else {
							this -> tutorialCharacter -> setColorAlpha ( alphaVal ) ;
							this -> tutorialCharacter2 -> setColorAlpha ( alphaVal ) ;
						}
					}
					else if ( ( alphaVal > 0.0000000f ) && ( ! ( ( this -> playerValues ) . tutorialEnabled ) ) ) {
						secondCount = frameCount = 1 ;
						( this -> playerValues ) . switchAlpha = false ;
						alphaVal -= 1.0f * deltaTime ;
						this -> tutorialCharacter -> setColorAlpha ( alphaVal ) ;
						this -> tutorialCharacter2 -> setColorAlpha ( alphaVal ) ;
					}
					else if ( ( this -> tutorialCharacter != NULL ) && ( ! ( ( this -> playerValues ) . tutorialEnabled ) ) ) {
						secondCount = frameCount = 1 ;
						alphaVal = 0.0f ;
						this -> tutorialCharacter -> setColorAlpha ( 0.0f ) ;
						this -> tutorialCharacter2 -> setColorAlpha ( 0.0f ) ;
						levelMasterCharacterTutorial * tmp = this -> tutorialCharacter ;
						this -> tutorialCharacter = NULL ;
						levelMasterCharacterTutorial2 * tmp2 = this -> tutorialCharacter2 ;
						this -> tutorialCharacter2 = NULL ;
						delete tmp ;
						delete tmp2 ;
					}
					if ( ! ( this -> pauseNonPlayers ) ) {
						if ( ( ( ( long long ) ( this -> playerValues ) . secondIncrement ) != 0 ) && ( ( secondCount % ( ( long long ) ( this -> playerValues ) . secondIncrement ) ) == 0 ) && ( frameCount == 0 ) ) {
							( this -> playerValues ) . characterScale += vec3 ( ( ( this -> playerValues ) . growthScale ) , ( ( this -> playerValues ) . growthScale ) , ( ( this -> playerValues ) . growthScale ) ) ;
							if ( ( ( this -> playerValues ) . characterScale . x ) > 0.015f )
								( this -> playerValues ) . characterScale = vec3 ( 0.015f , 0.015f , 0.015f ) ;
							( this -> playerValues ) . constSpeed += vec3 ( 0.0f , ( ( this -> playerValues ) . growthSpeed ) , 0.0f ) ;
							if ( ( ( this -> playerValues ) . constSpeed . y ) < ( - 1.75f ) )
								( this -> playerValues ) . constSpeed . y = ( - 1.75f ) ;
							( this -> playerValues ) . secondIncrement -= ( this -> playerValues ) . secondIncrementGrowth ;
							if ( ( ( this -> playerValues ) . secondIncrement ) < 25.0f )
								( this -> playerValues ) . secondIncrement = 25.0f ;
						}
						if ( ( ( secondCount % ( ( long long ) ( this -> playerValues ) . secondIncrement2 ) ) == 0 ) && ( frameCount == 0 ) ) {
							( this -> playerValues ) . growthScale *= ( this -> playerValues ) . scaleIncrement ;
							if ( ( ( this -> playerValues ) . growthScale ) > 0.0075f )
								( this -> playerValues ) . growthScale = 0.0075f ;
							( this -> playerValues ) . growthSpeed *= ( this -> playerValues ) . speedIncrement ;
							if ( ( ( this -> playerValues ) . growthSpeed ) > 0.35f )
								( this -> playerValues ) . growthSpeed = 0.35f ;
							( this -> playerValues ) . secondIncrement2 -= ( this -> playerValues ) . secondIncrement2Growth ;
							if ( ( ( this -> playerValues ) . secondIncrement2 ) < 70.0f )
								( this -> playerValues ) . secondIncrement2 = 70.0f ;
						}

						while ( this -> playerAchieved -- ) {
							( this -> playerValues ) . growthScale *= ( this -> playerValues ) . scaleIncrement ;
							( this -> playerValues ) . growthSpeed *= ( this -> playerValues ) . speedIncrement ;
						}
						this -> playerAchieved = 0 ;

						while ( this -> playerObstacled -- ) {
							( this -> playerValues ) . playerStrength -- ;
							( this -> playerValues ) . growthScale /= ( this -> playerValues ) . scaleIncrement ;
							( this -> playerValues ) . growthSpeed /= ( this -> playerValues ) . speedIncrement ;
							vec3 playerSize = this -> playerCharacter -> getModelScale ( ) ;
							if ( ( ( playerSize . x ) / 2 ) >= 0.01f )
								this -> playerCharacter -> scaleModel ( vec3 ( ( - ( ( playerSize . x ) / 2 ) ) , ( - ( ( playerSize . y ) / 2 ) ) , ( - ( ( playerSize . z ) / 2 ) ) ) ) ;
							else {
								this -> playerCharacter -> scaleModel ( vec3 ( ( - ( playerSize . x ) ) , ( - ( playerSize . y ) ) , ( - ( playerSize . z ) ) ) ) ;
								this -> playerCharacter -> scaleModel ( vec3 ( 0.01f , 0.01f , 0.01f ) ) ;
							}
						}
						this -> playerObstacled = 0 ;
					}

					deltaSpeed = ( ( ( this -> playerValues ) . constSpeed ) * deltaTime ) ;
					vec3 playerSize = this -> playerCharacter -> getModelScale ( ) ;
					if ( playerSize . x < 0.025f )
						deltaScale = ( ( ( this -> playerValues ) . characterScale ) * deltaTime ) ;
					else if ( playerSize . x == 0.025f )
						deltaScale = vec3 ( 0 , 0 , 0 ) ;
					else {
						vec3 tmp = this -> playerCharacter -> getModelScale ( ) ;
						deltaScale = vec3 ( ( 0.025f - ( tmp . x ) ) , ( 0.025f - ( tmp . y ) ) , ( 0.025f - ( tmp . z ) ) ) ;
					}

					if ( ( this -> inputObject -> checkKeyPressed ( GLFW_KEY_D ) ) || ( this -> inputObject -> checkKeyPressed ( GLFW_KEY_RIGHT ) ) ) {
						this -> endgameMode = this -> pauseNonPlayers = ( this -> playerValues ) . tutorialEnabled = ( this -> playerValues ) . showTutorial = false ;
						deltaSpeed [ 0 ] += ( ( ( this -> playerValues ) . characterSpeed [ 0 ] ) * deltaTime ) ;
					}
					else if ( ( this -> inputObject -> checkKeyPressed ( GLFW_KEY_A ) ) || ( this -> inputObject -> checkKeyPressed ( GLFW_KEY_LEFT ) ) ) {
						this -> endgameMode = this -> pauseNonPlayers = ( this -> playerValues ) . tutorialEnabled = ( this -> playerValues ) . showTutorial = false ;
						deltaSpeed [ 0 ] -= ( ( ( this -> playerValues ) . characterSpeed [ 0 ] ) * deltaTime ) ;
					}
					if ( ( this -> inputObject -> checkKeyPressed ( GLFW_KEY_W ) ) || ( this -> inputObject -> checkKeyPressed ( GLFW_KEY_UP ) ) ) {
						this -> endgameMode = this -> pauseNonPlayers = ( this -> playerValues ) . tutorialEnabled = ( this -> playerValues ) . showTutorial = false ;
						deltaSpeed [ 1 ] += ( ( ( this -> playerValues ) . characterSpeed [ 1 ] ) * deltaTime ) ;
					}
					if ( ( this -> inputObject -> checkKeyPressed ( GLFW_KEY_S ) ) || ( this -> inputObject -> checkKeyPressed ( GLFW_KEY_DOWN ) ) ) {
						this -> endgameMode = this -> pauseNonPlayers = ( this -> playerValues ) . tutorialEnabled = ( this -> playerValues ) . showTutorial = false ;
						deltaSpeed [ 1 ] -= ( ( ( this -> playerValues ) . characterSpeed [ 1 ] ) * deltaTime ) ;
					}

					if ( ! ( this -> pauseNonPlayers ) ) {
						this -> playerCharacter -> translateModel ( deltaSpeed ) ;

						this -> playerCharacter -> scaleModel ( deltaScale ) ;
					}

					frameTime = difftime ( time ( 0 ) , startTime ) ;
					frameTime *= 1000000000 ;
					sleepTime = { 0 , ( 5000000 - ( ( long ) frameTime ) ) } ;
				}
				while ( ( this -> endThreads ) != 0 ) ;
				this -> logger -> logInfo ( "playerThread : Ended player thread\n" ) ;
			}

			static void * playerThreadCaller ( void * obj ) {
				levelMaster * thisObj = static_cast < levelMaster * > ( obj ) ;
				thisObj -> playerThreadFunction ( ) ;
			}

			void obstacleThreadFunction ( ) {
				static uniform_real_distribution < float > startDistributionX ( ( - 1.0f ) , 1.0f ) ;
				static uniform_real_distribution < float > endDistributionX ( ( - 0.2f ) , 0.2f ) ;
				static uniform_real_distribution < float > startDistributionY ( 0.85f , 1.0f ) ;
				static uniform_real_distribution < float > endDistributionY ( ( - 0.2f ) , 0.0f ) ;
				static uniform_int_distribution < > spawnDistribution ( 0 , 190 ) ;
				static uniform_int_distribution < > amountDistribution ( 3 , 8 ) ;
				int spawnRandom = ( spawnDistribution ( * rndGenerator ) ) ;
				int spawnAmount = ( amountDistribution ( * rndGenerator ) ) ;
				int frameCount = 0 ;
				time_t startTime ;
				double frameTime ;
				long long secondCount = 0 ;
				float deltaTime = 0.005f ;
				timespec sleepTime = { 0 , 5000000 } ;
				levelMasterCharacterObstacle ** currentCharacter , * delTmp ;
				this -> logger -> logInfo ( "obstacleThread : Started obstacle thread" ) ;
				this -> endThreads ++ ;
				while ( ( this -> pauseThreads ) || ( this -> pauseNonPlayers ) ) {
					if ( ( this -> endThreads ) == 0 )
						return ;
				}
				startTime = time ( 0 ) ;
				while ( difftime ( time ( 0 ) , startTime ) < 25.0f ) {
					if ( ( this -> endThreads ) == 0 )
						return ;
				}
				do {
					nanosleep ( & sleepTime , NULL ) ;
					startTime = time ( 0 ) ;
					if ( ( this -> endThreads ) == 0 )
						return ;
					if ( ( this -> pauseThreads ) || ( this -> pauseNonPlayers ) )
						continue ;
					frameCount ++ ;
					if ( frameCount >= 200 ) {
						frameCount = 0 ;
						secondCount ++ ;
					}
					if ( secondCount ) {
						if ( ( ( secondCount % ( ( long long ) ( this -> obstacleValues ) . secondIncrement2 ) ) == 0 ) && ( frameCount == 0 ) ) {
							( this -> obstacleValues ) . secondIncrement -= ( this -> obstacleValues ) . secondIncrementGrowth ;
							if ( ( ( this -> obstacleValues ) . secondIncrement ) < 0.2f )
								( this -> obstacleValues ) . secondIncrement = 0.2f ;
							( this -> obstacleValues ) . characterProperties . speed += ( this -> obstacleValues ) . speedGrowth ;
							if ( ( ( this -> obstacleValues ) . characterProperties . speed ) < 1.25f )
								( this -> obstacleValues ) . characterProperties . speed = 1.25f ;
							( this -> obstacleValues ) . characterProperties . growthScale *= ( this -> obstacleValues ) . scaleGrowth ;
							if ( ( ( this -> obstacleValues ) . characterProperties . growthScale ) > 0.01f )
								( this -> obstacleValues ) . characterProperties . growthScale = 0.01f ;
							( this -> obstacleValues ) . secondIncrement2 -= ( this -> obstacleValues ) . secondIncrement2Growth ;
							if ( ( ( this -> obstacleValues ) . secondIncrement2 ) < 15.0f )
								( this -> obstacleValues ) . secondIncrement2 = 15.0f ;
						}
						else if ( ( ( ( this -> obstacleValues ) . secondIncrement ) < 1.0f ) && ( ( ( ( secondCount * 5000 ) + frameCount ) % ( ( long long ) ( ( this -> obstacleValues ) . secondIncrement * 5000 ) ) ) == spawnRandom ) ) {
							spawnRandom = ( spawnDistribution ( * rndGenerator ) ) ;
							while ( spawnAmount -- ) {
								spawnRandom = ( spawnDistribution ( * rndGenerator ) ) ;
								( this -> obstacleValues ) . characterProperties . startPoint = vec2 ( startDistributionX ( * rndGenerator ) , startDistributionY ( * rndGenerator ) ) ;
								float endX = endDistributionX ( * rndGenerator ) + ( this -> playerCharacter -> getModelPosition ( ) ) . x ;
								float endY = endDistributionY ( * rndGenerator ) + ( this -> playerCharacter -> getModelPosition ( ) ) . y ;
								( this -> obstacleValues ) . characterProperties . endPoint = vec2 ( endX , endY ) ;
								delTmp = new levelMasterCharacterObstacle ( ( this -> obstacleValues ) . characterProperties , ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
								float obstacleSize = ( ( this -> obstacleValues ) . characterProperties . growthScale ) * 100 ;
								if ( obstacleSize > 0.0275f )
									obstacleSize = 0.0275f ;
								delTmp -> scaleModel ( vec3 ( obstacleSize , obstacleSize , obstacleSize ) ) ;
								delTmp -> setColorAlpha ( 1.0f ) ;
								delTmp -> nextCharacter = this -> obstacleCharacter ;
								this -> obstacleCharacter = delTmp ;
								delTmp = NULL ;
							}
							spawnAmount = ( amountDistribution ( * rndGenerator ) ) ;
						}
						else if ( ( ( ( this -> obstacleValues ) . secondIncrement ) >= 1.0f ) && ( ( secondCount % ( ( long long ) ( this -> obstacleValues ) . secondIncrement ) ) == 0 ) && ( frameCount == spawnRandom ) ) {
							spawnRandom = ( spawnDistribution ( * rndGenerator ) ) ;
							while ( spawnAmount -- ) {
								( this -> obstacleValues ) . characterProperties . startPoint = vec2 ( startDistributionX ( * rndGenerator ) , startDistributionY ( * rndGenerator ) ) ;
								float endX = endDistributionX ( * rndGenerator ) + ( this -> playerCharacter -> getModelPosition ( ) ) . x ;
								float endY = endDistributionY ( * rndGenerator ) + ( this -> playerCharacter -> getModelPosition ( ) ) . y ;
								( this -> obstacleValues ) . characterProperties . endPoint = vec2 ( endX , endY ) ;
								delTmp = new levelMasterCharacterObstacle ( ( this -> obstacleValues ) . characterProperties , ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
								float obstacleSize = ( ( this -> obstacleValues ) . characterProperties . growthScale ) * 100 ;
								if ( obstacleSize > 0.0275f )
									obstacleSize = 0.0275f ;
								delTmp -> scaleModel ( vec3 ( obstacleSize , obstacleSize , obstacleSize ) ) ;
								delTmp -> setColorAlpha ( 0.0f ) ;
								delTmp -> nextCharacter = this -> obstacleCharacter ;
								this -> obstacleCharacter = delTmp ;
								delTmp = NULL ;
							}
							spawnAmount = ( amountDistribution ( * rndGenerator ) ) ;
						}
					}

					currentCharacter = & ( this -> obstacleCharacter ) ;
					while ( ( ( * currentCharacter ) -> nextCharacter ) != NULL ) {
						if ( ( * currentCharacter ) -> destroyCharacter ) {
							currentCharacter = & ( ( * currentCharacter ) -> nextCharacter ) ;
							continue ;
						}
						vec3 obstaclePosition = ( * currentCharacter ) -> getModelPosition ( ) ;
						vec2 obstacleStartPos = ( ( * currentCharacter ) -> characterProperties ) . startPoint ;
						vec2 obstacleEndPos = ( ( * currentCharacter ) -> characterProperties ) . endPoint ;
						float obstaclePathDistance = distance ( obstacleStartPos , obstacleEndPos ) ;
						float obstacleSpeed = ( ( * currentCharacter ) -> characterProperties ) . speed ;
						float tmpVal = distance ( vec2 ( obstaclePosition . x , obstaclePosition . y ) , obstacleStartPos ) / ( obstaclePathDistance / 5 ) ;
						if ( ( tmpVal <= 1.0f ) && ! ( this -> pausePlayer ) )
							( * currentCharacter ) -> setColorAlpha ( tmpVal ) ;
						if ( ( ( obstaclePosition . y ) < ( obstacleEndPos . y ) ) && ! ( this -> pausePlayer ) )
								( * currentCharacter ) -> destroyCharacter = true ;
						else {
							vec3 originPos = this -> playerCharacter -> getModelPosition ( ) ;
							vec3 originScale = this -> playerCharacter -> getModelScale ( ) ;

							vec3 targetPos = ( * currentCharacter ) -> getModelPosition ( ) ;
							vec3 targetScale = ( * currentCharacter ) -> getModelScale ( ) ;
							float distanceBetweenPoints = distance ( vec2 ( ( originPos . x ) , ( originPos . y ) ) , vec2 ( ( targetPos . x ) , ( targetPos . y ) ) ) ;
							if ( ( distanceBetweenPoints < ( ( originScale . x ) + ( targetScale . x ) ) ) && ! ( this -> pausePlayer ) ) {
								( * currentCharacter ) -> destroyCharacter = true ;
								this -> playerObstacled ++ ;
							}
							else {
								float tmpVal2 = distance ( vec2 ( obstaclePosition . x , obstaclePosition . y ) , obstacleEndPos ) / ( obstaclePathDistance / 5 ) ;
								if ( ( tmpVal2 <= 1.0f ) && ! ( this -> pausePlayer ) )
									( * currentCharacter ) -> setColorAlpha ( tmpVal2 ) ;
								float distX = ( obstacleSpeed * deltaTime ) * ( ( ( obstacleEndPos . x ) - ( obstacleStartPos . x ) ) / distance ( vec2 ( ( obstacleStartPos . x ) , ( obstacleStartPos . y ) ) , vec2 ( ( obstacleEndPos . x ) , ( obstacleEndPos . y ) ) ) ) ;
								float distY = ( obstacleSpeed * deltaTime ) * ( ( ( obstacleEndPos . y ) - ( obstacleStartPos . y ) ) / distance ( vec2 ( ( obstacleStartPos . x ) , ( obstacleStartPos . y ) ) , vec2 ( ( obstacleEndPos . x ) , ( obstacleEndPos . y ) ) ) ) ;
								float growthVal = ( ( ( * currentCharacter ) -> characterProperties ) . growthScale ) * deltaTime ;
								( * currentCharacter ) -> translateModel ( vec3 ( distX , distY , 0.0f ) ) ;
								( * currentCharacter ) -> scaleModel ( vec3 ( growthVal , growthVal , growthVal ) ) ;
								currentCharacter = & ( ( * currentCharacter ) -> nextCharacter ) ;
							}
						}
					}

					frameTime = difftime ( time ( 0 ) , startTime ) ;
					frameTime *= 1000000000 ;
					sleepTime = { 0 , ( 5000000 - ( ( long ) frameTime ) ) } ;
				}
				while ( ( this -> endThreads ) != 0 ) ;
				this -> logger -> logInfo ( "obstacleThread : Ended obstacle thread\n" ) ;
			}

			static void * obstacleThreadCaller ( void * obj ) {
				levelMaster * thisObj = static_cast < levelMaster * > ( obj ) ;
				thisObj -> obstacleThreadFunction ( ) ;
			}

			void achievementThreadFunction ( ) {
				static uniform_real_distribution < float > startDistributionX ( ( - 1.0f ) , 1.0f ) ;
				static uniform_real_distribution < float > endDistributionX1 ( ( - 1.0f ) , 1.0f ) ;
				static uniform_real_distribution < float > endDistributionX2 ( ( - 1.0f ) , 0.7f ) ;
				static uniform_real_distribution < float > endDistributionX3 ( ( - 0.7f ) , 1.0f ) ;
				static uniform_real_distribution < float > startDistributionY ( 0.7f , 1.0f ) ;
				static uniform_real_distribution < float > endDistributionY ( ( - 1.0f ) , ( - 0.7f ) ) ;
				static uniform_int_distribution < > spawnDistribution ( 0 , 190 ) ;
				int spawnRandom = ( spawnDistribution ( * rndGenerator ) ) ;
				int frameCount = 0 ;
				time_t startTime ;
				double frameTime ;
				long long secondCount = 0 ;
				float deltaTime = 0.005f ;
				timespec sleepTime = { 0 , 5000000 } ;
				levelMasterCharacterAchievement ** currentCharacter , * delTmp ;
				this -> logger -> logInfo ( "achievementThread : Started achievement thread" ) ;
				this -> endThreads ++ ;
				do {
					nanosleep ( & sleepTime , NULL ) ;
					startTime = time ( 0 ) ;
					if ( ( this -> endThreads ) == 0 )
						return ;
					if ( ( this -> pauseThreads ) || ( this -> pauseNonPlayers ) )
						continue ;
					frameCount ++ ;
					if ( frameCount >= 200 ) {
						frameCount = 0 ;
						secondCount ++ ;
					}
					if ( secondCount ) {
						if ( ( ( secondCount % ( ( long long ) ( this -> achievementValues ) . secondIncrement2 ) ) == 0 ) && ( frameCount == 0 ) ) {
							( this -> achievementValues ) . secondIncrement -= ( this -> achievementValues ) . secondIncrementGrowth ;
							if ( ( ( this -> achievementValues ) . secondIncrement ) < 3.0f )
								( this -> achievementValues ) . secondIncrement = 3.0f ;
							( this -> achievementValues ) . characterProperties . speed += ( this -> achievementValues ) . speedGrowth ;
							if ( ( ( this -> achievementValues ) . characterProperties . speed ) < 0.85f )
								( this -> achievementValues ) . characterProperties . speed = 0.85f ;
							( this -> achievementValues ) . characterProperties . growthScale *= ( this -> achievementValues ) . scaleGrowth ;
							if ( ( ( this -> achievementValues ) . characterProperties . growthScale ) > 0.01f )
								( this -> achievementValues ) . characterProperties . growthScale = 0.01f ;
							( this -> achievementValues ) . secondIncrement2 -= ( this -> achievementValues ) . secondIncrement2Growth ;
							if ( ( ( this -> achievementValues ) . secondIncrement2 ) < 25.0f )
								( this -> achievementValues ) . secondIncrement2 = 25.0f ;
						}
						else if ( ( ( ( this -> achievementValues ) . secondIncrement ) >= 1.0f ) && ( ( secondCount % ( ( long long ) ( this -> achievementValues ) . secondIncrement ) ) == 0 ) && ( frameCount == spawnRandom ) ) {
							spawnRandom = ( spawnDistribution ( * rndGenerator ) ) ;
							( this -> achievementValues ) . characterProperties . startPoint = vec2 ( startDistributionX ( * rndGenerator ) , startDistributionY ( * rndGenerator ) ) ;
							float endX ;
							if ( ( this -> achievementValues ) . characterProperties . startPoint . x >= 0.6f )
								endX = endDistributionX2 ( * rndGenerator ) ;
							else if ( ( this -> achievementValues ) . characterProperties . startPoint . x <= ( - 0.6f ) )
								endX = endDistributionX3 ( * rndGenerator ) ;
							else
								endX = endDistributionX1 ( * rndGenerator ) ;
							( this -> achievementValues ) . characterProperties . endPoint = vec2 ( endX , endDistributionY ( * rndGenerator ) ) ;
							delTmp = new levelMasterCharacterAchievement ( ( this -> achievementValues ) . characterProperties , ( this -> inputObject ) , mat4 ( 1.0f ) , ( this -> logger ) ) ;
							float achievementSize = ( ( this -> achievementValues ) . characterProperties . growthScale ) * 100 ;
							if ( achievementSize > 0.0225f )
								achievementSize = 0.0225f ;
							delTmp -> scaleModel ( vec3 ( achievementSize , achievementSize , achievementSize ) ) ;
							delTmp -> setColorAlpha ( 0.0f ) ;
							delTmp -> nextCharacter = this -> achievementCharacter ;
							this -> achievementCharacter = delTmp ;
							delTmp = NULL ;
						}
					}

					currentCharacter = & ( this -> achievementCharacter ) ;
					while ( ( ( * currentCharacter ) -> nextCharacter ) != NULL ) {
						if ( ( * currentCharacter ) -> destroyCharacter ) {
							currentCharacter = & ( ( * currentCharacter ) -> nextCharacter ) ;
							continue ;
						}
						vec3 achievementPosition = ( * currentCharacter ) -> getModelPosition ( ) ;
						vec2 achievementStartPos = ( ( * currentCharacter ) -> characterProperties ) . startPoint ;
						vec2 achievementEndPos = ( ( * currentCharacter ) -> characterProperties ) . endPoint ;
						float achievementPathDistance = distance ( achievementStartPos , achievementEndPos ) ;
						float achievementSpeed = ( ( * currentCharacter ) -> characterProperties ) . speed ;
						float tmpVal = distance ( vec2 ( achievementPosition . x , achievementPosition . y ) , achievementStartPos ) / ( achievementPathDistance / 5 ) ;
						if ( ( tmpVal <= 1.0f ) && ! ( this -> pausePlayer ) )
							( * currentCharacter ) -> setColorAlpha ( tmpVal ) ;
						if ( ( ( achievementPosition . y ) < ( achievementEndPos . y ) ) && ! ( this -> pausePlayer ) )
							( * currentCharacter ) -> destroyCharacter = true ;
						else {
							vec3 originPos = this -> playerCharacter -> getModelPosition ( ) ;
							vec3 originScale = this -> playerCharacter -> getModelScale ( ) ;

							vec3 targetPos = ( * currentCharacter ) -> getModelPosition ( ) ;
							vec3 targetScale = ( * currentCharacter ) -> getModelScale ( ) ;
							float distanceBetweenPoints = distance ( vec2 ( ( originPos . x ) , ( originPos . y ) ) , vec2 ( ( targetPos . x ) , ( targetPos . y ) ) ) ;
							if ( ( distanceBetweenPoints < ( ( originScale . x ) + ( targetScale . x ) ) ) && ! ( this -> pausePlayer ) ) {
								( * currentCharacter ) -> destroyCharacter = true ;
								this -> playerAchieved ++ ;
							}
							else {
								float tmpVal2 = distance ( vec2 ( achievementPosition . x , achievementPosition . y ) , achievementEndPos ) / ( achievementPathDistance / 5 ) ;
								if ( ( tmpVal2 <= 1.0f ) && ! ( this -> pausePlayer ) )
									( * currentCharacter ) -> setColorAlpha ( tmpVal2 ) ;
								float distX = ( achievementSpeed * deltaTime ) * ( ( ( achievementEndPos . x ) - ( achievementStartPos . x ) ) / distance ( vec2 ( ( achievementStartPos . x ) , ( achievementStartPos . y ) ) , vec2 ( ( achievementEndPos . x ) , ( achievementEndPos . y ) ) ) ) ;
								float distY = ( achievementSpeed * deltaTime ) * ( ( ( achievementEndPos . y ) - ( achievementStartPos . y ) ) / distance ( vec2 ( ( achievementStartPos . x ) , ( achievementStartPos . y ) ) , vec2 ( ( achievementEndPos . x ) , ( achievementEndPos . y ) ) ) ) ;
								float growthVal = ( ( ( * currentCharacter ) -> characterProperties ) . growthScale ) * deltaTime ;
								( * currentCharacter ) -> translateModel ( vec3 ( distX , distY , 0.0f ) ) ;
								( * currentCharacter ) -> scaleModel ( vec3 ( growthVal , growthVal , growthVal ) ) ;
								currentCharacter = & ( ( * currentCharacter ) -> nextCharacter ) ;
							}
						}
					}

					frameTime = difftime ( time ( 0 ) , startTime ) ;
					frameTime *= 1000000000 ;
					sleepTime = { 0 , ( 5000000 - ( ( long ) frameTime ) ) } ;
				}
				while ( ( this -> endThreads ) != 0 ) ;
				this -> logger -> logInfo ( "achievementThread : Ended achievement thread\n" ) ;
			}

			static void * achievementThreadCaller ( void * obj ) {
				levelMaster * thisObj = static_cast < levelMaster * > ( obj ) ;
				thisObj -> achievementThreadFunction ( ) ;
			}

			int mainGame ( ) {
				this -> endThreads ++ ;
				int checkSelection ;
				do {
					this -> windowObject -> clearScreen ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;
					checkSelection = this -> checkInputs ( ) ;

					if ( this -> fpsObject -> checkSecondPassed ( ) ) {
						this -> fpsObject -> setFPSAndSPF ( ) ;
						double fps = this -> fpsObject -> getFPS ( ) ;
						double spf = this -> fpsObject -> getSPF ( ) ;
						printf ( "FPS : %f\t" , fps ) ;
						printf ( "SPF : %f\n" , spf ) ;
					}
					else this -> fpsObject -> incrementFrames ( ) ;

					if ( checkSelection == ( - 1 ) ) {
						glFlush ( ) ;
						glFinish ( ) ;
						this -> inputObject -> pollEvents ( ) ;
						this -> logger -> logInfo ( "levelMaster : User wants to exit. Ending game main level" ) ;
						return ( - 1 ) ;
					}

					levelMasterCharacterObstacle ** tmpCharacter1 = NULL , * tmpDel1 ;
					levelMasterCharacterAchievement ** tmpCharacter2 = NULL , * tmpDel2 ;

					tmpCharacter2 = & ( this -> achievementCharacter ) ;
					while ( ( ( * tmpCharacter2 ) -> nextCharacter ) != NULL ) {
						if ( ( * tmpCharacter2 ) -> destroyCharacter ) {
							tmpDel2 = ( * tmpCharacter2 ) ;
							( * tmpCharacter2 ) = ( * tmpCharacter2 ) -> nextCharacter ;
							delete tmpDel2 ;
						}
						else
							tmpCharacter2 = & ( ( * tmpCharacter2 ) -> nextCharacter ) ;
					}

					tmpCharacter1 = & ( this -> obstacleCharacter ) ;
					while ( ( ( * tmpCharacter1 ) -> nextCharacter ) != NULL ) {
						if ( ( * tmpCharacter1 ) -> destroyCharacter ) {
							tmpDel1 = ( * tmpCharacter1 ) ;
							( * tmpCharacter1 ) = ( * tmpCharacter1 ) -> nextCharacter ;
							delete tmpDel1 ;
						}
						else
							tmpCharacter1 = & ( ( * tmpCharacter1 ) -> nextCharacter ) ;
					}

					this -> drawCharacters ( ) ;

					this -> inputObject -> pollEvents ( ) ;
				}
				while ( ( this -> playerValues ) . playerStrength > 0 ) ;
				return 0 ;
			}

			int initialAnimation ( ) {
				int checkSelection ;
				float deltaTime = 0.01f ;
				time_t startTime ;
				double frameTime ;
				timespec sleepTime = { 0 , 10000000 } ;
				do {
					nanosleep ( & sleepTime , NULL ) ;
					startTime = time ( 0 ) ;
					this -> windowObject -> clearScreen ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;
					checkSelection = this -> checkInputs ( ) ;

					float currentAlpha = this -> playerCharacter -> getColorAlpha ( ) ;
					currentAlpha += 0.6f * deltaTime ;
					this -> playerCharacter -> setColorAlpha ( currentAlpha ) ;

					if ( this -> fpsObject -> checkSecondPassed ( ) ) {
						this -> fpsObject -> setFPSAndSPF ( ) ;
						double fps = this -> fpsObject -> getFPS ( ) ;
						double spf = this -> fpsObject -> getSPF ( ) ;
						printf ( "FPS : %f\t" , fps ) ;
						printf ( "SPF : %f\n" , spf ) ;
					}
					else this -> fpsObject -> incrementFrames ( ) ;

					if ( checkSelection == ( - 1 ) ) {
						glFlush ( ) ;
						glFinish ( ) ;
						this -> inputObject -> pollEvents ( ) ;
						this -> logger -> logInfo ( "levelMaster : User wants to exit. Ending game main level" ) ;
						return ( - 1 ) ;
					}

					this -> drawCharacters ( ) ;

					frameTime = difftime ( time ( 0 ) , startTime ) ;
					frameTime *= 1000000000 ;
					sleepTime = { 0 , ( 10000000 - ( ( long ) frameTime ) ) } ;
					this -> inputObject -> pollEvents ( ) ;
				}
				while ( ( this -> playerCharacter -> getColorAlpha ( ) ) < 1.0f ) ;
				return 0 ;
			}

			int endRoundAnimation ( ) {
				int checkSelection ;
				float deltaTime = 0.01f ;
				time_t startTime ;
				double frameTime ;
				timespec sleepTime = { 0 , 10000000 } ;
				vec3 initialPlayerSize = this -> playerCharacter -> getModelScale ( ) ;
				vec3 finalPlayerSize ( 0.01f , 0.01f , 0.01f ) ;
				vec3 playerSizeChange ( ( ( finalPlayerSize . x ) - ( initialPlayerSize . x ) ) , ( ( finalPlayerSize . y ) - ( initialPlayerSize . y ) ) , ( ( finalPlayerSize . z ) - ( initialPlayerSize . z ) ) ) ;
				vec3 finalPosition ( 0.0f , ( - 0.94f ) , 0.0f ) , currentPosition = this -> playerCharacter -> getModelPosition ( ) ;
				vec3 initialPosition = currentPosition ;
				float playerDistance = distance ( finalPosition , currentPosition ) ;
				float minDistY = 0.15f * deltaTime * ( ( ( finalPosition . y ) - ( currentPosition . y ) ) / distance ( finalPosition , currentPosition ) ) ;
				float minDistX = minDistY * ( ( ( finalPosition . x ) - ( currentPosition . x ) ) / ( ( finalPosition . y ) - ( currentPosition . y ) ) ) ;
				do {
					nanosleep ( & sleepTime , NULL ) ;
					startTime = time ( 0 ) ;
					this -> windowObject -> clearScreen ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;
					checkSelection = this -> checkInputs ( ) ;

					levelMasterCharacterObstacle * currentCharacter1 = this -> obstacleCharacter ;
					while ( ( currentCharacter1 -> nextCharacter ) != NULL ) {
						currentCharacter1 -> setColorAlpha ( distance ( finalPosition , currentPosition ) / playerDistance ) ;
						currentCharacter1 = currentCharacter1 -> nextCharacter ;
					}

					levelMasterCharacterAchievement * currentCharacter2 = this -> achievementCharacter ;
					while ( ( currentCharacter2 -> nextCharacter ) != NULL ) {
						currentCharacter2 -> setColorAlpha ( distance ( finalPosition , currentPosition ) / playerDistance ) ;
						currentCharacter2 = currentCharacter2 -> nextCharacter ;
					}

					vec3 tmpValue = this -> playerCharacter -> getModelScale ( ) ;
					tmpValue *= - 1 ;
					this -> playerCharacter -> scaleModel ( tmpValue ) ;
					tmpValue = initialPlayerSize + ( playerSizeChange * ( distance ( initialPosition , currentPosition ) / playerDistance ) ) ;
					this -> playerCharacter -> scaleModel ( tmpValue ) ;

					float distX = ( ( ( finalPosition . x ) - ( currentPosition . x ) ) * deltaTime ) ;
					if ( abs ( distX ) < abs ( minDistX ) )
						distX = minDistX ;
					float distY = ( ( ( finalPosition . y ) - ( currentPosition . y ) ) * deltaTime ) ;
					if ( abs ( distY ) < abs ( minDistY ) )
						distY = minDistY ;
					this -> playerCharacter -> translateModel ( vec3 ( distX , distY , 0.0f ) ) ;

					if ( this -> fpsObject -> checkSecondPassed ( ) ) {
						this -> fpsObject -> setFPSAndSPF ( ) ;
						double fps = this -> fpsObject -> getFPS ( ) ;
						double spf = this -> fpsObject -> getSPF ( ) ;
						printf ( "FPS : %f\t" , fps ) ;
						printf ( "SPF : %f\n" , spf ) ;
					}
					else this -> fpsObject -> incrementFrames ( ) ;

					if ( checkSelection == ( - 1 ) ) {
						glFlush ( ) ;
						glFinish ( ) ;
						this -> inputObject -> pollEvents ( ) ;
						this -> logger -> logInfo ( "levelMaster : User wants to exit. Ending game main level" ) ;
						return ( - 1 ) ;
					}

					levelMasterCharacterObstacle ** tmpCharacter1 = NULL , * tmpDel1 ;
					levelMasterCharacterAchievement ** tmpCharacter2 = NULL , * tmpDel2 ;

					tmpCharacter2 = & ( this -> achievementCharacter ) ;
					while ( ( ( * tmpCharacter2 ) -> nextCharacter ) != NULL ) {
						if ( ( * tmpCharacter2 ) -> destroyCharacter ) {
							tmpDel2 = ( * tmpCharacter2 ) ;
							( * tmpCharacter2 ) = ( * tmpCharacter2 ) -> nextCharacter ;
							delete tmpDel2 ;
						}
						else
							tmpCharacter2 = & ( ( * tmpCharacter2 ) -> nextCharacter ) ;
					}

					tmpCharacter1 = & ( this -> obstacleCharacter ) ;
					while ( ( ( * tmpCharacter1 ) -> nextCharacter ) != NULL ) {
						if ( ( * tmpCharacter1 ) -> destroyCharacter ) {
							tmpDel1 = ( * tmpCharacter1 ) ;
							( * tmpCharacter1 ) = ( * tmpCharacter1 ) -> nextCharacter ;
							delete tmpDel1 ;
						}
						else
							tmpCharacter1 = & ( ( * tmpCharacter1 ) -> nextCharacter ) ;
					}

					this -> drawCharacters ( ) ;

					frameTime = difftime ( time ( 0 ) , startTime ) ;
					frameTime *= 1000000000 ;
					sleepTime = { 0 , ( 10000000 - ( ( long ) frameTime ) ) } ;
					currentPosition = this -> playerCharacter -> getModelPosition ( ) ;
					this -> inputObject -> pollEvents ( ) ;
				}
				while ( ( currentPosition . y ) > ( - 0.93875f ) ) ;
				this -> playerCharacter -> translateModel ( vec3 ( ( ( finalPosition . x ) - ( currentPosition . x ) ) , ( ( finalPosition . y ) - ( currentPosition . y ) ) , 0.0f ) ) ;
				vec3 tmpValue = this -> playerCharacter -> getModelScale ( ) ;
				tmpValue *= - 1 ;
				this -> playerCharacter -> scaleModel ( tmpValue ) ;
				this -> playerCharacter -> scaleModel ( vec3 ( 0.01f , 0.01f , 0.01f ) ) ;
				return 0 ;
			}

			int endgameFinalAnimation ( short int initialState ) {
				short int stateNo = initialState , completeStages = 0 ;
				float deltaTime = 0.02f ;
				time_t startTime ;
				double frameTime ;
				int frameCount = 0 , checkSelection ;
				long long secondCount = 0 ;
				timespec sleepTime = { 0 , 10000000 } ;
				do {
					nanosleep ( & sleepTime , NULL ) ;
					startTime = time ( 0 ) ;
					this -> windowObject -> clearScreen ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;
					checkSelection = this -> checkInputs ( ) ;
					frameCount ++ ;
					if ( frameCount >= 100 ) {
						frameCount = 0 ;
						secondCount ++ ;
						printf ( "%d\t%llu\n" , completeStages , secondCount ) ;
					}

					float currentAlpha = this -> playerCharacter -> getColorAlpha ( ) ;
					currentAlpha -= ( 0.01f / ( ( float ) initialState ) ) * deltaTime ;
					if ( ( currentAlpha < ( 1.0f / ( ( float ) ( 2 * stateNo ) ) ) ) && ( stateNo < 5 ) ) {
						this -> playerCharacter -> changeState ( ++ stateNo ) ;
					}
					if ( ( stateNo >= 5 ) && ! ( completeStages & 0b00000001 ) ) {
						completeStages = completeStages | 0b00000001 ;
						this -> playerCharacter -> setColorAlpha ( ( 1.0f / ( ( float ) ( 2 * stateNo ) ) ) ) ;
					}
					else if ( ! ( completeStages & 0b00000001 ) )
						this -> playerCharacter -> setColorAlpha ( currentAlpha ) ;

					vec3 currentPosition = this -> playerCharacter -> getModelPosition ( ) ;
					vec3 movePosition = vec3 ( 0.0f , 0.0f , 0.0f ) ;
					if ( currentPosition . x < ( - 0.000135f ) )
						movePosition . x = 0.0135f * deltaTime ;
					else if ( currentPosition . x > 0.000135f )
						movePosition . x = ( - 0.0135f ) * deltaTime ;
					else
						movePosition . x = ( - 1 ) * currentPosition . x ;
					if ( currentPosition . y < ( - 0.000135f ) )
						movePosition . y = 0.0135f * deltaTime ;
					else if ( currentPosition . y > 0.000135f )
						movePosition . y = ( - 0.0135f ) * deltaTime ;
					else
						movePosition . y = ( - 1 ) * currentPosition . y ;
					if ( currentPosition . z < ( - 0.000135f ) )
						movePosition . z = 0.0135f * deltaTime ;
					else if ( currentPosition . z > 0.000135f )
						movePosition . z = ( - 0.0135f ) * deltaTime ;
					else
						movePosition . z = ( - 1 ) * currentPosition . z ;
					if ( ( currentPosition . x > ( - 0.000135f ) ) && ( currentPosition . x < 0.000135f ) &&
						 ( currentPosition . y > ( - 0.000135f ) ) && ( currentPosition . y < 0.000135f ) &&
						 ( currentPosition . z > ( - 0.000135f ) ) && ( currentPosition . z < 0.000135f ) &&
						 ! ( completeStages & 0b00000010 ) ) {
						completeStages = completeStages | 0b00000010 ;
					}
					this -> playerCharacter -> translateModel ( movePosition ) ;

					vec3 currentScale = this -> playerCharacter -> getModelScale ( ) ;
					if ( currentScale . x <= 0.2675f )
						this -> playerCharacter -> scaleModel ( vec3 ( ( 0.0025f * deltaTime ) , ( 0.0025f * deltaTime ) , ( 0.0025f * deltaTime ) ) ) ;
					else if ( ! ( completeStages & 0b00000100 ) )
						completeStages = completeStages | 0b00000100 ;

					if ( ( ( completeStages & 0b00000111 ) == 0b00000111 ) && ( ! ( completeStages & 0b00001000 ) ) ) {
						secondCount = frameCount = 0 ;
						completeStages = completeStages | 0b00001000 ;
						this -> playerCharacter -> setColorAlpha ( 0.0f ) ;
					}
					else if ( ( ( completeStages & 0b00001111 ) == 0b00001111 ) && ( secondCount >= 5 ) )
						completeStages = completeStages | 0b00010000 ;

					if ( this -> fpsObject -> checkSecondPassed ( ) ) {
						this -> fpsObject -> setFPSAndSPF ( ) ;
						double fps = this -> fpsObject -> getFPS ( ) ;
						double spf = this -> fpsObject -> getSPF ( ) ;
						printf ( "FPS : %f\t" , fps ) ;
						printf ( "SPF : %f\n" , spf ) ;
					}
					else this -> fpsObject -> incrementFrames ( ) ;

					if ( checkSelection == ( - 1 ) ) {
						glFlush ( ) ;
						glFinish ( ) ;
						this -> inputObject -> pollEvents ( ) ;
						this -> logger -> logInfo ( "levelMaster : User wants to exit. Ending game main level" ) ;
						return ( - 1 ) ;
					}

					this -> drawCharacters ( ) ;

					frameTime = difftime ( time ( 0 ) , startTime ) ;
					frameTime *= 1000000000 ;
					sleepTime = { 0 , ( 10000000 - ( ( long ) frameTime ) ) } ;
					this -> inputObject -> pollEvents ( ) ;
				}
				while ( ( completeStages & 0b00011111 ) != 0b00011111 ) ;
				return 0 ;
			}

		public:

			levelMaster ( windowHandler * windowObject , inputHandler * inputObject , loggingHandler * logger = new loggingHandler )
				: levelBase ( windowObject , inputObject , logger ) {
				this -> logger -> logInfo ( "levelMaster : Initializing game main level\n" ) ;

				this -> fpsObject = new fpsCounter ( this -> logger ) ;
				this -> rndGenerator = new mt19937 ( time ( 0 ) / 3600 ) ;
				static uniform_int_distribution < > gameRoundsDistribution ( 4 , 5 ) ;
				this -> gameRounds = ( gameRoundsDistribution ( * rndGenerator ) ) ;

				this -> logger -> logInfo ( "levelMaster : Initialized game main level\n" ) ;
			}

			~levelMaster ( ) {
				delete this -> fpsObject ;
			}

			int main ( ) {
				this -> logger -> logInfo ( "levelMaster : Started game main level" ) ;

				short int lives = 1 , retval = 0 , currentPlayerState = 1 ;
				this -> endgameMode = false ;

				this -> initializeCharacters ( lives ++ ) ;

				this -> logger -> logInfo ( "levelMaster : Create threads for various character types and wait for them to start" ) ;
				this -> endThreads = 0 ;
				this -> pauseThreads = true ;
				this -> pauseNonPlayers = true ;
				pthread_create ( & ( this -> playerThread ) , NULL , ( this -> playerThreadCaller ) , this ) ;
				pthread_create ( & ( this -> obstacleThread ) , NULL , ( this -> obstacleThreadCaller ) , this ) ;
				pthread_create ( & ( this -> achievementThread ) , NULL , ( this -> achievementThreadCaller ) , this ) ;
				while ( ( this -> endThreads ) != 3 ) ;

				this -> fpsObject -> resetTimer ( ) ;
				retval = this -> initialAnimation ( ) ;

				do {
					this -> logger -> logInfo ( "levelMaster : All threads started. Running frames" ) ;
					if ( ! retval ) {
						this -> pauseThreads = false ;
						retval = this -> mainGame ( ) ;
					}
					if ( ! retval ) {
						this -> pausePlayer = true ;
						retval = this -> endRoundAnimation ( ) ;
						this -> pausePlayer = false ;
					}

					this -> pauseThreads = true ;
					this -> pauseNonPlayers = true ;
					this -> logger -> logInfo ( "levelMaster : Waiting for created threads to end" ) ;
					this -> endThreads = 0 ;
					pthread_join ( this -> playerThread , NULL ) ;
					pthread_join ( this -> obstacleThread , NULL ) ;
					pthread_join ( this -> achievementThread , NULL ) ;

					this -> destroyCharacters ( ) ;

					if ( ( ( this -> gameRounds ) > 1 ) && ( ! retval ) ) {
						this -> initializeCharacters ( lives ++ ) ;
						this -> playerCharacter -> setColorAlpha ( 1.0f ) ;

						this -> logger -> logInfo ( "levelMaster : Create threads for various character types and wait for them to start" ) ;
						this -> endThreads = 0 ;
						this -> pauseThreads = true ;
						this -> pauseNonPlayers = true ;
						pthread_create ( & ( this -> playerThread ) , NULL , ( this -> playerThreadCaller ) , this ) ;
						pthread_create ( & ( this -> obstacleThread ) , NULL , ( this -> obstacleThreadCaller ) , this ) ;
						pthread_create ( & ( this -> achievementThread ) , NULL , ( this -> achievementThreadCaller ) , this ) ;
						while ( ( this -> endThreads ) != 3 ) ;
					}
				}
				while ( ( -- ( this -> gameRounds ) ) && ( ! retval ) ) ;

				if ( ! retval ) {
					static uniform_int_distribution < > gameRoundsDistribution ( 4 , 5 ) ;
					this -> gameRounds = ( gameRoundsDistribution ( * rndGenerator ) ) ;
					lives = 1 ;

					this -> initializeCharacters_endgame ( lives ++ ) ;
					this -> playerCharacter -> setColorAlpha ( 1.0f ) ;

					this -> logger -> logInfo ( "levelMaster : Create threads for various character types and wait for them to start" ) ;
					this -> endThreads = 0 ;
					this -> pauseThreads = true ;
					this -> pauseNonPlayers = true ;
					pthread_create ( & ( this -> playerThread ) , NULL , ( this -> playerThreadCaller ) , this ) ;
					pthread_create ( & ( this -> obstacleThread ) , NULL , ( this -> obstacleThreadCaller ) , this ) ;
					pthread_create ( & ( this -> achievementThread ) , NULL , ( this -> achievementThreadCaller ) , this ) ;
					while ( ( this -> endThreads ) != 3 ) ;

					do {
						this -> endgameMode = true ;
						this -> logger -> logInfo ( "levelMaster : All threads started. Running frames" ) ;
						if ( ! retval ) {
							this -> pauseThreads = false ;
							retval = this -> mainGame ( ) ;
							if ( this -> endgameMode )
								break ;
						}

						if ( ( ( this -> gameRounds ) > 1 ) && ( ! retval ) ) {
							if ( ! retval ) {
								this -> pausePlayer = true ;
								retval = this -> endRoundAnimation ( ) ;
								this -> pausePlayer = false ;
							}

							this -> pauseThreads = true ;
							this -> pauseNonPlayers = true ;
							this -> logger -> logInfo ( "levelMaster : Waiting for created threads to end" ) ;
							this -> endThreads = 0 ;
							pthread_join ( this -> playerThread , NULL ) ;
							pthread_join ( this -> obstacleThread , NULL ) ;
							pthread_join ( this -> achievementThread , NULL ) ;

							this -> destroyCharacters ( ) ;

							this -> initializeCharacters_endgame ( lives ++ ) ;
							this -> playerCharacter -> setColorAlpha ( 1.0f ) ;
							this -> playerCharacter -> changeState ( ++ currentPlayerState ) ;

							this -> logger -> logInfo ( "levelMaster : Create threads for various character types and wait for them to start" ) ;
							this -> endThreads = 0 ;
							this -> pauseThreads = true ;
							this -> pauseNonPlayers = true ;
							pthread_create ( & ( this -> playerThread ) , NULL , ( this -> playerThreadCaller ) , this ) ;
							pthread_create ( & ( this -> obstacleThread ) , NULL , ( this -> obstacleThreadCaller ) , this ) ;
							pthread_create ( & ( this -> achievementThread ) , NULL , ( this -> achievementThreadCaller ) , this ) ;
							while ( ( this -> endThreads ) != 3 ) ;
						}
					}
					while ( ( -- ( this -> gameRounds ) ) && ( ! retval ) ) ;

					if ( ! ( this -> endgameMode ) ) {
						int checkSelection ;
						time_t startTime = time ( 0 ) ;
						while ( difftime ( time ( 0 ) , startTime ) < 3.0f ) {
							this -> windowObject -> clearScreen ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;
							checkSelection = this -> checkInputs ( ) ;

							if ( this -> fpsObject -> checkSecondPassed ( ) ) {
								this -> fpsObject -> setFPSAndSPF ( ) ;
								double fps = this -> fpsObject -> getFPS ( ) ;
								double spf = this -> fpsObject -> getSPF ( ) ;
								printf ( "FPS : %f\t" , fps ) ;
								printf ( "SPF : %f\n" , spf ) ;
							}
							else this -> fpsObject -> incrementFrames ( ) ;

							if ( checkSelection == ( - 1 ) ) {
								glFlush ( ) ;
								glFinish ( ) ;
								this -> inputObject -> pollEvents ( ) ;
								this -> logger -> logInfo ( "levelMaster : User wants to exit. Ending game main level" ) ;
								return ( - 1 ) ;
							}

							this -> drawCharacters ( ) ;
						}
					}

					this -> pauseThreads = true ;
					this -> pauseNonPlayers = true ;
					this -> logger -> logInfo ( "levelMaster : Waiting for created threads to end" ) ;
					this -> endThreads = 0 ;
					pthread_join ( this -> playerThread , NULL ) ;
					pthread_join ( this -> obstacleThread , NULL ) ;
					pthread_join ( this -> achievementThread , NULL ) ;

					this -> destroyCharacters ( ) ;
				}

				if ( ! retval ) {
					this -> initializeCharacters_endgame ( ) ;
					this -> playerCharacter -> setColorAlpha ( 1.0f ) ;
					if ( currentPlayerState > 1 ) this -> playerCharacter -> changeState ( currentPlayerState ) ;

					retval = this -> endgameFinalAnimation ( currentPlayerState ) ;

					this -> destroyCharacters ( ) ;
				}

				this -> logger -> logInfo ( "levelMaster : Ended the game main level. Exiting\n" ) ;
				return 0 ;
			}

	} ;

#endif