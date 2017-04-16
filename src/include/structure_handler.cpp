# ifndef GL_MODEL_STRUCTURE_HANDLER

	# define GL_MODEL_STRUCTURE_HANDLER 1

	# include <string>
	# include <string.h>
	# include <vector>
	# include <map>
	# include <GL/glew.h>
	# include <glm/glm.hpp>

	# include "logging_handler.cpp"

	using namespace glm ;
	using namespace std ;

	struct modelStructureInfo {
		vector < vec3 >                                 vertices                      ;
		vector < vec2 >                                 uvs                           ;
		vector < vec3 >                                 normals                       ;
		vector < unsigned short >                       indices                       ;
	} ;

	struct modelPackedVertex {
		vec3                                            position                      ;
		vec2                                            uv                            ;
		vec3                                            normal                        ;
		bool operator < ( const modelPackedVertex that ) const {
			return memcmp ( ( void * ) this , ( void * ) & that , sizeof ( modelPackedVertex ) ) > 0 ;
		} ;
	} ;

	class modelStructureHandler ;

	map < string , modelStructureHandler * > modelStructuresList ;
	map < string , int > modelStructuresListCount ;

	class modelStructureHandler {

		protected:
			
			struct modelStructureInfo                   modelStructure                ;
			GLuint                                      vertexID                      ;
			GLuint                                      uvID                          ;
			GLuint                                      normalID                      ;
			GLuint                                      indicesID                     ;
			string                                      modelName                     ;
			short int                                   modelType                     ;
			
			loggingHandler                            * logger                        ;

			int getSimilarVertexIndex ( modelPackedVertex * packed , map < modelPackedVertex , unsigned short > * vertexToOutIndex , unsigned short * result ) {
				map < modelPackedVertex , unsigned short > :: iterator it = ( * vertexToOutIndex ) . find ( * packed ) ;
				if ( it == ( ( * vertexToOutIndex ) . end ( ) ) )
					return 0 ;
				else {
					( * result ) = it -> second ;
					return 1 ;
				}
			}

			void indexVBO ( struct modelStructureInfo * unindexedModelStructure , struct modelStructureInfo * indexedModelStructure ) {
				map < modelPackedVertex , unsigned short > vertexToOutIndex ;
				for ( unsigned int i = 0 ; i < ( ( unindexedModelStructure -> vertices ) . size ( ) ) ; i ++ ) {
					modelPackedVertex packed = { ( unindexedModelStructure -> vertices ) [ i ] , ( unindexedModelStructure -> uvs ) [ i ] , ( unindexedModelStructure -> normals ) [ i ] } ;
					unsigned short index ;
					int found = this -> getSimilarVertexIndex ( & packed , & vertexToOutIndex , & index ) ;

					if ( found )
						( indexedModelStructure -> indices ) . push_back ( index ) ;
					else {
						( indexedModelStructure -> vertices ) . push_back ( ( unindexedModelStructure -> vertices ) [ i ] ) ;
						( indexedModelStructure -> uvs ) . push_back ( ( unindexedModelStructure -> uvs ) [ i ] ) ;
						( indexedModelStructure -> normals ) . push_back ( ( unindexedModelStructure -> normals ) [ i ] ) ;
						unsigned short newindex = ( unsigned short ) ( indexedModelStructure -> vertices ) . size ( ) - 1 ;
						( indexedModelStructure -> indices ) . push_back ( newindex ) ;
						vertexToOutIndex [ packed ] = newindex ;
					}
				}
			}

			int loadOBJ ( const char * filePath , struct modelStructureInfo * modelStructureRetrieved ) {
				vector < unsigned int > vertexIndices , uvIndices , normalIndices ;
				vector < vec3 > tempVertices ;
				vector < vec2 > tempUVs ;
				vector < vec3 > tempNormals ;

				FILE * file = fopen ( filePath , "r" ) ;
				if ( file == NULL )
					return ( - 1 ) ;

				while ( 1 ) {
					char lineHeader [ 128 ] ;
					int res = fscanf ( file , "%s" , lineHeader ) ;
					if ( res == EOF )
						break ;
					
					if ( strcmp ( lineHeader , "v" ) == 0 ) {
						vec3 vertex ;
						fscanf ( file , "%f %f %f\n" , & ( vertex . x ) , & ( vertex . y ) , & ( vertex . z ) ) ;
						tempVertices . push_back ( vertex ) ;
					}
					else if ( strcmp ( lineHeader , "vt" ) == 0 ) {
						vec2 uv ;
						fscanf ( file , "%f %f\n" , & ( uv . x ) , & ( uv . y ) ) ;
						tempUVs . push_back ( uv ) ;
					}
					else if ( strcmp ( lineHeader , "vn" ) == 0 ) {
						vec3 normal ;
						fscanf ( file , "%f %f %f\n" , & ( normal . x ) , & ( normal . y ) , & ( normal . z ) ) ;
						tempNormals . push_back ( normal ) ;
					}
					else if ( strcmp ( lineHeader , "f" ) == 0 ) {
						string vertex1 , vertex2 , vertex3 ;
						unsigned int vertexIndex [ 3 ] , uvIndex [ 3 ] , normalIndex [ 3 ] ;
						int matches = fscanf ( file , "%d/%d/%d %d/%d/%d %d/%d/%d\n" , & vertexIndex [ 0 ] , & uvIndex [ 0 ] , & normalIndex [ 0 ] , & vertexIndex [ 1 ] , & uvIndex [ 1 ] , & normalIndex [ 1 ] , & vertexIndex [ 2 ] , & uvIndex [ 2 ] , & normalIndex [ 2 ] ) ;
						if ( matches != 9 )
							return ( - 1 ) ;
						vertexIndices . push_back ( vertexIndex [ 0 ] ) ;
						vertexIndices . push_back ( vertexIndex [ 1 ] ) ;
						vertexIndices . push_back ( vertexIndex [ 2 ] ) ;
						uvIndices . push_back ( uvIndex [ 0 ] ) ;
						uvIndices . push_back ( uvIndex [ 1 ] ) ;
						uvIndices . push_back ( uvIndex [ 2 ] ) ;
						normalIndices . push_back ( normalIndex [ 0 ] ) ;
						normalIndices . push_back ( normalIndex [ 1 ] ) ;
						normalIndices . push_back ( normalIndex [ 2 ] ) ;
					}
					else{
						char stupidBuffer [ 1000 ] ;
						fgets ( stupidBuffer , 1000 , file ) ;
					}

				}

				struct modelStructureInfo * unindexedModelStructure = new struct modelStructureInfo ;
				for( unsigned int i = 0 ; i < vertexIndices . size ( ) ; i ++ ) {
					unsigned int vertexIndex = vertexIndices [ i ] ;
					unsigned int uvIndex = uvIndices [ i ] ;
					unsigned int normalIndex = normalIndices [ i ] ;
					
					vec3 vertex = tempVertices [ vertexIndex - 1 ] ;
					vec2 uv = tempUVs [ uvIndex - 1 ] ;
					vec3 normal = tempNormals [ normalIndex - 1 ] ;
					
					( unindexedModelStructure -> vertices ) . push_back ( vertex ) ;
					( unindexedModelStructure -> uvs ) . push_back ( uv ) ;
					( unindexedModelStructure -> normals ) . push_back ( normal ) ;
				}

				if ( this -> modelType )
					this -> indexVBO ( unindexedModelStructure , modelStructureRetrieved ) ;
				else
					* modelStructureRetrieved = * unindexedModelStructure ;

				return 0 ;
			}

		public:

			modelStructureHandler ( const char * modelName , short int modelType , loggingHandler * logger = new loggingHandler ) {
				this -> logger = logger ;

				char logMsg [ 200 ] = "modelStructureHandler : Initializing the model structure handler for model \'" ;
				strcat ( logMsg , modelName ) ;
				strcat ( logMsg , "\'" ) ;
				this -> modelType = modelType ;
				this -> logger -> logInfo ( logMsg ) ;

				this -> logger -> logInfo ( "modelStructureHandler : Retrieving structure details from file" ) ;
				char structureFilePath [ 500 ] = "assets/models/" ;
				strcat ( structureFilePath , modelName ) ;
				strcat ( structureFilePath , "/structure" ) ;
				this -> logger -> logInfo ( "modelTextureHandler : Loading structure using OBJ loader" ) ;
				if ( this -> loadOBJ ( structureFilePath , & ( this -> modelStructure ) ) ) {
					this -> logger -> logInfo ( "modelStructureHandler : Could not retrieve structure details. Have to exit\n" ) ;
					exit ( - 1 ) ;
				}

				this -> logger -> logInfo ( "modelStructureHandler : Loading structure details" ) ;
				glGenBuffers ( 1 , & ( this -> vertexID ) ) ;
				glBindBuffer ( GL_ARRAY_BUFFER , ( this -> vertexID ) ) ;
				glBufferData ( GL_ARRAY_BUFFER , ( ( this -> modelStructure ) . vertices . size ( ) ) * sizeof ( vec3 ) , & ( ( this -> modelStructure ) . vertices [ 0 ] ) , GL_STATIC_DRAW ) ;

				glGenBuffers ( 1 , & ( this -> uvID ) ) ;
				glBindBuffer ( GL_ARRAY_BUFFER , ( this -> uvID ) ) ;
				glBufferData ( GL_ARRAY_BUFFER , ( ( this -> modelStructure ) . uvs . size ( ) ) * sizeof ( vec2 ) , & ( ( this -> modelStructure ) . uvs [ 0 ] ) , GL_STATIC_DRAW ) ;

				if ( modelType ) {
					glGenBuffers ( 1 , & ( this -> normalID ) ) ;
					glBindBuffer ( GL_ARRAY_BUFFER , ( this -> normalID ) ) ;
					glBufferData ( GL_ARRAY_BUFFER , ( ( this -> modelStructure ) . normals . size ( ) ) * sizeof ( vec3 ) , & ( ( this -> modelStructure ) . normals [ 0 ] ) , GL_STATIC_DRAW ) ;

					glGenBuffers ( 1 , & ( this -> indicesID ) ) ;
					glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , ( this -> indicesID ) ) ;
					glBufferData ( GL_ELEMENT_ARRAY_BUFFER , ( ( this -> modelStructure ) . indices . size ( ) ) * sizeof ( unsigned short ) , & ( ( this -> modelStructure ) . indices [ 0 ] ) , GL_STATIC_DRAW ) ;
				}

				this -> logger -> logInfo ( "modelStructureHandler : Adding current structure to the list of available structures to use directly" ) ;
				modelStructuresList [ modelName ] = this ;
				modelStructuresListCount [ modelName ] = 1 ;
				this -> modelName = modelName ;

				this -> logger -> logInfo ( "modelStructureHandler : Initialized the model structure handler\n" ) ;
			}

			~modelStructureHandler ( ) {
				glDeleteBuffers ( 1 , & ( this -> vertexID ) ) ;
				glDeleteBuffers ( 1 , & ( this -> uvID ) ) ;
				if ( modelType ) {
					glDeleteBuffers ( 1 , & ( this -> normalID ) ) ;
					glDeleteBuffers ( 1 , & ( this -> indicesID ) ) ;
				}
				modelStructuresList . erase ( this -> modelName ) ;
				modelStructuresListCount . erase ( this -> modelName ) ;
			}

			void bindVertexBuffer ( ) {
				glEnableVertexAttribArray ( 0 ) ;
				glBindBuffer ( GL_ARRAY_BUFFER , ( this -> vertexID ) ) ;
				glVertexAttribPointer ( 0 , 3 , GL_FLOAT , GL_FALSE , 0 , ( void * ) 0 ) ;
			}

			void bindUVBuffer ( ) {
				glEnableVertexAttribArray ( 1 ) ;
				glBindBuffer ( GL_ARRAY_BUFFER , ( this -> uvID ) ) ;
				glVertexAttribPointer ( 1 , 2 , GL_FLOAT , GL_FALSE , 0 , ( void * ) 0 ) ;
			}

			void bindNormalBuffer ( ) {
				if ( this -> modelType ) {
					glEnableVertexAttribArray ( 2 ) ;
					glBindBuffer ( GL_ARRAY_BUFFER , ( this -> normalID ) ) ;
					glVertexAttribPointer ( 2 , 3 , GL_FLOAT , GL_FALSE , 0 , ( void * ) 0 ) ;
				}
			}

			void bindIndicesBuffer ( ) {
				if ( this -> modelType )
					glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER , ( this -> indicesID ) ) ;
			}

			void drawStructure ( ) {
				if ( this -> modelType )
					glDrawElements ( GL_TRIANGLES , ( ( this -> modelStructure ) . indices . size ( ) ) , GL_UNSIGNED_SHORT , ( void * ) 0 ) ;
				else
					glDrawArrays ( GL_TRIANGLES , 0 , ( this -> modelStructure ) . vertices . size ( ) ) ;

				glDisableVertexAttribArray ( 0 ) ;
				glDisableVertexAttribArray ( 1 ) ;
				if ( this -> modelType )
					glDisableVertexAttribArray ( 2 ) ;
			}

			static modelStructureHandler * checkStructure ( const char * modelName ) {
				if ( modelStructuresList . count ( modelName ) == 0 )
					return NULL ;
				else
					return modelStructuresList [ modelName ] ;
			} 

	} ;

#endif