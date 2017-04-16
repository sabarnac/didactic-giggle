char gameVersion [ ] = "0.0.1" ;

# include "include/logging_handler.cpp"
fileLoggingHandler * logger = new fileLoggingHandler ;

# include "include/window_handler.cpp"
# include "include/input_handler.cpp"
//# include "include/text_handler.cpp"

# include "levels/level_master.cpp"

int main ( ) {
	logger -> logInfo ( "MAIN : Creating window handler" ) ;
	char gameTitle [ 100 ] ;
	snprintf ( gameTitle , 100 , "No I Won't Tell You The Name Of My Game [ %s ]" , gameVersion ) ;
	windowHandler * windowObject = new windowHandler ( gameTitle , logger ) ;

	logger -> logInfo ( "MAIN : Creating input handler" ) ;
	inputHandler * inputObject = new inputHandler ( logger ) ;

	//logger -> logInfo ( "MAIN : Creating 2D text handler" ) ;
	//textObject = new textHandler ( logger ) ;

	logger -> logInfo ( "MAIN : Setting clear screen color" ) ;
	windowObject -> setClearScreenColor ( vec4 ( 0.0f , 0.0f , 0.0f , 0.0f ) ) ;

	logger -> logInfo ( "MAIN : Enabling depth testing and face culling" ) ;
	windowObject -> enableMSAA ( ) ;
	windowObject -> enableAlphaBlending ( ) ;

	logger -> logInfo ( "MAIN : Setting input modes" ) ;
	//inputObject -> setInputMode ( GLFW_STICKY_KEYS , GL_TRUE ) ;
	inputObject -> setInputMode ( GLFW_CURSOR , GLFW_CURSOR_DISABLED ) ;
	inputObject -> setSwapInterval ( ) ;

	printf ( "%f\n" , ( 0.0001f * 1.01f ) ) ;
    
	logger -> logInfo ( "MAIN : Doing a quick refresh on inputs" ) ;
    inputObject -> pollEvents ( ) ;

	logger -> logInfo ( "MAIN : Initializing master level" ) ;
	levelMaster * masterLevel = new levelMaster ( windowObject , inputObject , logger ) ;
	logger -> logInfo ( "MAIN : Executing master level" ) ;
	masterLevel -> main ( ) ;

	logger -> logInfo ( "MAIN : Freeing memory" ) ;
	delete masterLevel ;
	//delete textObject ;
	delete inputObject ;
	delete windowObject ;

	logger -> logInfo ( "MAIN : Exiting\n" ) ;
	delete logger ;

	return 0 ;
}