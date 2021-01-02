#version 430

out vec4 fragColor;

in vec2 fragCoord;

uniform sampler2D scene;

void main(){
	vec4 color = texture(scene,fragCoord);
	const float crossHairWidth = 0.002;
	if (fragCoord.x > 0.5 - crossHairWidth && fragCoord.x < 0.5 + crossHairWidth && fragCoord.y > 0.5-crossHairWidth && fragCoord.y < 0.5 + crossHairWidth){
		if (dot(color.rgb,vec3(1)) < 1.5)
			color = vec4(1);
		else
			color = vec4(0);
	}
	fragColor = color;
}