#version 430

out vec4 FragColor;

in vec2 fragCoord;

uniform sampler2D scene;

void main(){
	FragColor = texture(scene,fragCoord);
}