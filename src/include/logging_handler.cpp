# ifndef LOGGING_HANDLER

	# define LOGGING_HANDLER 1

	# include <stdio.h>
	# include <ctime>

	class loggingHandler {

		public:

			loggingHandler ( ) {
				this -> logInfo ( "Initialized logger" ) ;
			}

			virtual void logInfo ( const char * infoMessage ) {
				time_t currentTime = time ( 0 ) ;
				struct tm * currentTimeStructure = localtime ( & currentTime ) ;
				fprintf ( stderr , "%d:%d:%d %d-%d-%d :: %s\n" , ( currentTimeStructure -> tm_hour )
				                                               , ( currentTimeStructure -> tm_min )
				                                               , ( currentTimeStructure -> tm_sec )
				                                               , ( currentTimeStructure -> tm_mday )
				                                               , ( ( currentTimeStructure -> tm_mon ) + 1 )
				                                               , ( ( currentTimeStructure -> tm_year ) + 1900 )
				                                               , ( infoMessage ) ) ;
			}

	} ;

	class fileLoggingHandler : public loggingHandler {

		private:
		
			FILE                  * logFile           ;

		public:

			fileLoggingHandler ( ) {
				this -> logFile = fopen ( "logs/log.txt" , "w" ) ;
				fprintf ( ( this -> logFile ) , "--- END ---\n\n--- START ---\n\n" ) ;
				this -> logInfo ( "Initialized file logger" ) ;
			}

			~fileLoggingHandler ( ) {
				fclose ( this -> logFile ) ;
			}

			void logInfo ( const char * infoMessage ) {
				time_t currentTime = time ( 0 ) ;
				struct tm * currentTimeStructure = localtime ( & currentTime ) ;
				fprintf ( ( this -> logFile ) , "%d:%d:%d %d-%d-%d :: %s\n" , ( currentTimeStructure -> tm_hour )
				                                               , ( currentTimeStructure -> tm_min )
				                                               , ( currentTimeStructure -> tm_sec )
				                                               , ( currentTimeStructure -> tm_mday )
				                                               , ( ( currentTimeStructure -> tm_mon ) + 1 )
				                                               , ( ( currentTimeStructure -> tm_year ) + 1900 )
				                                               , ( infoMessage ) ) ;
			}

	} ;

#endif