# ifndef GAME_FPS_COUNTER

	# define GAME_FPS_COUNTER 1

	# include <time.h>

	class fpsCounter {

		private:
			
			time_t                                      previousTime                  ;
			short unsigned int                          frameCount                    ;
			double                                      FPS                           ;
			double                                      SPF                           ;
			
			loggingHandler                            * logger                        ;

		public:

			fpsCounter ( loggingHandler * logger = new loggingHandler ) {
				this -> logger = logger ;

				this -> logger -> logInfo ( "fpsCounter : Initializing the FPS counter" ) ;

				this -> logger -> logInfo ( "fpsCounter : Setting initial values" ) ;
				this -> previousTime = time ( 0 ) ;
				this -> frameCount = 0 ;

				this -> logger -> logInfo ( "fpsCounter : Initialized the FPS counter\n" ) ;
			}

			void incrementFrames ( ) {
				this -> frameCount ++ ;
			}

			int checkSecondPassed ( ) {
				double seconds_since_start = difftime ( time ( 0 ) , ( this -> previousTime ) ) ;
				if ( seconds_since_start >= 1.0f )
					return 1 ;
				else
					return 0 ;
			}

			void setFPSAndSPF ( ) {
				time_t currentTime = time ( 0 ) ;
				double timePeriod = difftime ( currentTime , ( this -> previousTime ) ) ;

				this -> FPS = ( ( ( double ) ( this -> frameCount ) ) / timePeriod ) ;
				this -> SPF = ( timePeriod / ( ( double ) ( this -> frameCount ) ) ) ;

				this -> previousTime = currentTime ;
				this -> frameCount = 0 ;
			}

			double getFPS ( ) {
				return this -> FPS ;
			}

			double getSPF ( ) {
				return this -> SPF ;
			}

			void resetTimer ( ) {
				this -> previousTime = time ( 0 ) ;
				this -> frameCount = 0 ;
			}

	} ;

#endif