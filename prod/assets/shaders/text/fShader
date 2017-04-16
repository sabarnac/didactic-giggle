#version 330 core

in vec2 UV ;

out vec3 color ;

uniform sampler2D textureSampler ;
uniform vec4 pickingColor ;

void main ( ) {

	if ( pickingColor . a == 0 ) {
		color . rgb = texture ( textureSampler , UV ) . rgb ;
	}
	else
		color = pickingColor . rgb ;

}