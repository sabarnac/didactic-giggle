# ifndef GL_INPUT_HANDLER

	# define GL_INPUT_HANDLER 1

	# include <glm/glm.hpp>
	# include <glm/gtc/matrix_transform.hpp>
	# include <glm/gtx/rotate_vector.hpp>

	# include "window_handler.cpp"
	# include "logging_handler.cpp"

	using namespace glm ;

	class inputHandler {

		private:
			
			double                                      previousCallTime              ;
			mat4                                        viewMatrix                    ;
			mat4                                        projectionMatrix              ;

			loggingHandler                            * logger                        ;

		public:

			inputHandler ( loggingHandler * logger = new loggingHandler ) {
				this -> logger = logger ;

				this -> logger -> logInfo ( "inputHandler : Initializing the input handler" ) ;

				this -> viewMatrix = lookAt ( vec3 ( 0 , 0 , 5 ) , vec3 ( 0 , 0 , 0 ) , vec3 ( 0 , 1 , 0 ) ) ;
				this -> projectionMatrix = ortho ( ( - aspectRatio ) , aspectRatio , ( - 1.0f ) , 1.0f , ( - 1.0f ) , 1.0f ) ;

				this -> logger -> logInfo ( "inputHandler : Initialized the input handler\n" ) ;
			}

			mat4 getViewMatrix ( ) {
				return this -> viewMatrix ;
			}

			mat4 getProjectionMatrix ( ) {
				return this -> projectionMatrix ;
			}

			void pollEvents ( ) {
				glfwSwapBuffers ( mainWindow ) ;
				glfwPollEvents ( ) ;
			}

			void setSwapInterval ( int time = 0 ) {
				glfwSwapInterval ( time ) ;
			}

			void waitForEventsTillTimeout ( float timeout = 0.5 ) {
				glfwSwapBuffers ( mainWindow ) ;
				glfwWaitEventsTimeout ( timeout ) ;
			}

			void waitForEvents ( ) {
				glfwSwapBuffers ( mainWindow ) ;
				glfwWaitEvents ( ) ;
			}

			void setInputMode ( int mode = GLFW_CURSOR , int value = GLFW_CURSOR_DISABLED ) {
				glfwSetInputMode ( mainWindow , mode , value ) ;
			}

			int getInputMode ( int mode = GLFW_CURSOR ) {
				return glfwGetInputMode ( mainWindow , mode ) ;
			}

			int checkKeyPressed ( int key ) {
				if ( glfwGetKey ( mainWindow , key ) == GLFW_PRESS ) return 1 ;
				else return 0 ;
			}

			int checkKeyReleased ( int key ) {
				if ( glfwGetKey ( mainWindow , key ) == GLFW_RELEASE ) return 1 ;
				else return 0 ;
			}

			int checkMouseButtonPressed ( int key ) {
				if ( glfwGetMouseButton ( mainWindow , key ) == GLFW_PRESS ) return 1 ;
				else return 0 ;
			}

			int checkMouseButtonReleased ( int key ) {
				if ( glfwGetMouseButton ( mainWindow , key ) == GLFW_RELEASE ) return 1 ;
				else return 0 ;
			}

			int checkWindowBeingClosed ( ) {
				return glfwWindowShouldClose ( mainWindow ) ;
			}

	} ;

#endif