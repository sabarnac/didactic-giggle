# ifndef GL_MODEL_OBJECT_HANDLER

	# define GL_MODEL_OBJECT_HANDLER 1

	# include <string.h>
	# include <map>
	# include <GL/glew.h>
	# include <glm/glm.hpp>

	# include "logging_handler.cpp"
	# include "structure_handler.cpp"
	# include "texture_handler.cpp"
	# include "shaders_handler.cpp"

	using namespace glm ;
	using namespace std ;

	class modelObjectsHandler ;

	map < string , modelObjectsHandler * > modelObjectsList ;
	map < string , int > modelObjectsListCount ;

	class modelObjectsHandler {

		protected:

			string                                      modelName                     ;
			string                                      shaderName                    ;
			short int                                   modelType                     ;

			loggingHandler                            * logger                        ;

		public:

			modelStructureHandler                     * modelStructure                ;
			modelTextureHandler                       * modelTexture                  ;
			modelShadersHandler                       * modelShaders                  ;

			modelObjectsHandler ( const char * modelName , const char * shaderName , short int modelType , loggingHandler * logger = new loggingHandler ) {
				this -> logger = logger ;

				char logMsg [ 200 ] = "modelObjectsHandler : Initializing the model objects handler for model \'" ;
				strcat ( logMsg , modelName ) ;
				strcat ( logMsg , "\'" ) ;
				this -> logger -> logInfo ( logMsg ) ;

				this -> logger -> logInfo ( "modelObjectsHandler : Initializing model object structure" ) ;
				this -> modelStructure = modelStructureHandler :: checkStructure ( modelName ) ;
				if ( this -> modelStructure == NULL )
					this -> modelStructure = new modelStructureHandler ( modelName , modelType , ( this -> logger ) ) ;
				else
					modelStructuresListCount [ modelName ] ++ ;

				this -> logger -> logInfo ( "modelObjectsHandler : Initializing model object texture" ) ;
				this -> modelTexture = modelTextureHandler :: checkTexture ( modelName ) ;
				if ( this -> modelTexture == NULL )
					this -> modelTexture = new modelTextureHandler ( modelName , ( this -> logger ) ) ;
				else
					modelTexturesListCount [ modelName ] ++ ;

				this -> logger -> logInfo ( "modelObjectsHandler : Initializing model object shaders" ) ;
				this -> modelShaders = modelShadersHandler :: checkShaders ( shaderName ) ;
				if ( this -> modelShaders == NULL )
					this -> modelShaders = new modelShadersHandler ( shaderName , ( this -> logger ) ) ;
				else
					modelShadersListCount [ shaderName ] ++ ;

				this -> logger -> logInfo ( "modelObjectsHandler : Adding current object to the list of available objects to use directly" ) ;
				modelObjectsList [ modelName ] = this ;
				modelObjectsListCount [ modelName ] = 1 ;
				this -> modelName = modelName ;
				this -> shaderName = shaderName ;
				this -> modelType = modelType ;

				this -> logger -> logInfo ( "modelObjectsHandler : Initialized the model objects handler\n" ) ;
			}

			~modelObjectsHandler ( ) {
				modelObjectsList . erase ( this -> modelName ) ;
				modelObjectsListCount . erase ( this -> modelName ) ;

				if ( ( -- modelStructuresListCount [ modelName ] ) == 0 )
					delete modelStructuresList [ modelName ] ;

				if ( ( -- modelTexturesListCount [ modelName ] ) == 0 )
					delete modelTexturesList [ modelName ] ;

				if ( ( -- modelShadersListCount [ shaderName ] ) == 0 )
					delete modelShadersList [ shaderName ] ;
			}

			void bindBuffers ( ) {
				this -> modelStructure -> bindVertexBuffer ( ) ;
				this -> modelStructure -> bindUVBuffer ( ) ;
				if ( this -> modelType ) {
					this -> modelStructure -> bindNormalBuffer ( ) ;
					this -> modelStructure -> bindIndicesBuffer ( ) ;
				}
			}

			void drawObject ( ) {
				this -> modelStructure -> drawStructure ( ) ;
			}

			static modelObjectsHandler * checkObjects ( const char * modelName ) {
				if ( modelObjectsList . count ( modelName ) == 0 )
					return NULL ;
				else
					return modelObjectsList [ modelName ] ;
			}

	} ;

#endif