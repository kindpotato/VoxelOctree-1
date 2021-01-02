#version 430

const int chunkWidth = 32, chunkSize = 32768, chunkDepth = 5;

layout(local_size_x = 32, local_size_y = 32) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

struct Chunk {
	uint blocks[chunkSize/4];//Divide chunksize by 4 because 4 blocks per int
};

layout(std430,binding=1) buffer blockLayout{
	 Chunk chunks[];	
};

uniform vec3 camPos;
uniform vec2 resolution;
uniform mat3 camOrientation;


bool getBlock(vec3 pos){
	pos = floor(pos);
	pos.xz *= 0.3;
	
	return pos.y < sin(pos.x) + sin(pos.z);
}


uint block(uvec3 pos, int level){ //Returns byte corresponding to block.
	uvec3 whichChunk;
	whichChunk.x = pos.x >> chunkDepth;
	whichChunk.y = pos.y >> chunkDepth;
	whichChunk.z = pos.z >> chunkDepth;
	uint mask = ~((1 << level)-1);
	mask &= (chunkWidth - 1);
	pos.x &= mask;
	pos.y &= mask;
	pos.z &= mask;
	//change 10 to (chunkDepth<<1)
	uint blockPos = (pos.x<<(chunkDepth<<1))+(pos.y<<chunkDepth)+pos.z;
	uint block = chunks[(whichChunk.x<<4) + (whichChunk.y<<2) + whichChunk.z].blocks[blockPos>>2];

	block >>= (blockPos&3)<<3;
	block &= 255;
	if (level == 0)
		return block & 7;
	return 1 & (block >> (2+level));
}

void main(){
	vec4 pixel = vec4(0,0,0,1);
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec3 v = normalize(vec3((pixelCoords-(0.5*resolution))/resolution.y,2.));
	v *= camOrientation;
	if (v.y > 0.){
		pixel = vec4(0.5,0.5,0.5,1);
	
	vec3 vinv = 1. / v.xyz;
	int level = 5;
	vec3 pos = camPos, frac;
	float minimum, t, ray = 0.;
	vec3 sideColor = vec3(0);
	for (int i = 0; i < 100; ++i){
		level = 5;
		uint blockInfo;
		
		do {
			blockInfo = block(uvec3(pos+0.2*sideColor),level);
			--level;
		}while (level >= 0 && bool(blockInfo));
		if (blockInfo == 0){
			++level;
			sideColor = vec3(0);
			minimum = 100;
			float nodeSize = float(1 << level);
			frac = -mod(pos,nodeSize);
			const float tolerance = 0.0;
			t = frac.x*vinv.x;
			if (t < minimum && t > tolerance){
				minimum = t;
				sideColor = vec3(-1,0,0);
			}
		
			t = (nodeSize+frac.x)*vinv.x;
			if (t < minimum && t > tolerance){
				minimum = t;
				sideColor = vec3(1,0,0);
			}

			t = frac.y*vinv.y;
			if (t < minimum && t > tolerance){
				minimum = t;
				sideColor = vec3(0,-1,0);
			}
		
			t = (nodeSize+frac.y)*vinv.y;
			if (t < minimum && t > tolerance){
				minimum = t;
				sideColor = vec3(0,1,0);
			}

			t = frac.z*vinv.z;
			if (t < minimum && t > tolerance){
				minimum = t;
				sideColor = vec3(0,0,-1);
			}
			
			t = (nodeSize+frac.z)*vinv.z;
			if (t < minimum && t > tolerance){
				minimum = t;
				sideColor = vec3(0,0,1);
			}
			ray += minimum+0.001;
			pos = camPos + ray*v;
			float width = float(chunkWidth<<2);
			if (pos.x <= 0. || pos.y <= 0. || pos.z <= 0. || pos.x >= width || pos.y >= width || pos.z >= width){
				pixel = vec4(0.3,0.2,0.05,1);
				break;
			}
		}	
		else{ //In this condition we have collided with a block
//0.5+0.5*sideColor
			pixel = vec4(1,1,1,1);
			break;
		}			
	}	
		
	
	imageStore(img_output,ivec2(pixelCoords.x,pixelCoords.y),pixel);
}

