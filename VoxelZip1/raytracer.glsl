#version 430

#define firstRayDepth 90
#define secondRayDepth 12
#define shadowRayDepth 40
#define secondRayCount 8
#define shadowRay
#define secondRays
#define temporalFilter
const float pi = 3.14159265;
const int chunkWidth = 32, chunkSize = 32768, chunkDepth = 5, regionDepth = 3;


layout(local_size_x = 32, local_size_y = 32) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

struct Chunk {
	uint blocks[chunkSize/4];//Divide chunksize by 4 because 4 blocks per int
};

layout(binding = 2) uniform sampler2D blockTexture;
layout(binding = 3) uniform sampler2D backBuffer;

layout(std430,binding=1) buffer blockLayout{
	 Chunk chunks[];
};


uniform vec3 camPos;
uniform mat3 camMat;
uniform vec3 lastcamPos;
uniform mat3 lastcamMat;
uniform vec2 resolution;
uniform uint frame;



float width = float(chunkWidth<<regionDepth);

uint blockInfo;

vec4 castRay(vec3 v, vec3 camPos, int its);

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)); }

float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}


float random( vec3  v ) { return floatConstruct(hash( floatBitsToUint(v))); }

float rand(vec3 pos){
	return fract(sin(dot(pos,vec3(12.9898,78.233,1.618))) * 43758.5453);
	
}

bool outside(uvec3 pos){
	return pos.x <= 0 || pos.y <= 0 || pos.z <= 0 || pos.x >= width || pos.y >= width || pos.z >= width;
}

bool nOutside(vec3 n){
	return n.x == 1. && n.y == 1.;
}

void writePixel(vec3 color){
	imageStore(img_output,ivec2(gl_GlobalInvocationID.xy),vec4(color,1));
}

float getBrightness(vec3 pos, vec3 n){
	const vec3 sunDir = normalize(vec3(1.9,7.618,3.03));
	if (nOutside(n)){
		return 1.-(pos.y)*0.0078125; //that's 1/128
	}
	if (blockInfo == 4)
		return 4.;
	#ifdef shadowRay
	vec4 result = castRay(-sunDir,pos,shadowRayDepth);
	if (nOutside(result.xyz))
	#endif
		return -1.2*dot(sunDir,n);//+0.15;
	return 0.;
	
}




vec3 getTexture(vec3 pos, vec3 n, uint type){
	pos = fract(pos);
	vec2 uv;
	if (n.x != 0.)
		uv = pos.zy;
	else if (n.y != 0.)
		uv = pos.xz;
	else
		uv = pos.xy;
	uv.x /= 4.;
	uv.x += float(type-1)/4.;
	return texture(blockTexture,uv).xyz;
}

uint block(uvec3 pos, int level){ //Returns byte being queried (for occupancy or blockInfo)
	uvec3 whichChunk;
	whichChunk.x = pos.x >> chunkDepth;
	whichChunk.y = pos.y >> chunkDepth;
	whichChunk.z = pos.z >> chunkDepth;
	uint mask = ~((1 << level)-1);
	mask &= (chunkWidth - 1);
	pos.x &= mask;
	pos.y &= mask;
	pos.z &= mask;

	uint blockPos = (pos.x<<(chunkDepth<<1))+(pos.y<<chunkDepth)+pos.z;
	uint block = chunks[(whichChunk.x<<(regionDepth << 1)) + (whichChunk.y<<regionDepth) + whichChunk.z].blocks[blockPos>>2];
	
	block >>= (blockPos&3)<<3;
	//block &= 255;
	if (level == 0)
		return block & 7;
	return 1 & (block >> (2+level));
}

vec4 castRay(vec3 v, vec3 camPos, int its){
	vec3 vinv = 1. / v.xyz;
	vec3 pos = camPos, frac;
	float minimum, t, ray = 0.;
	vec3 sideColor = vec3(0);
	
	for (int i = 0; i < its; ++i){
		int level = 5;
		uvec3 upos = uvec3(pos);
		if (outside(upos))
				return vec4(vec3(1,1,1),ray);
		do {
			blockInfo = block(upos,level);
			--level;
		} while (level >= 0 && bool(blockInfo));
		
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
			/*bool stay;
			do {
				uint mask = (1 << level)-1;
				stay = bool((upos.x & mask) + (upos.y & mask) + (upos.z & mask));
				++level;
			} while(!stay);*/
			
		}	
		else{ //In this condition we have collided with a block
			return vec4(-sideColor,ray);
		}
		
	}
	return vec4(vec3(1,1,1),ray);
}

vec4 getBackColor(vec3 position){
	position -= lastcamPos; //position is now relative to camera.
	vec3 pos = position*lastcamMat;
	vec2 uv = (pos.xy / pos.z);
	uv.y *= resolution.x/resolution.y;
	uv += 0.5 + vec2(0.5/resolution.x, 0.5/resolution.y);
	
	if (uv.x >= 0. && uv.x <= 1. && uv.y >= 0. && uv.y <= 1.){
		vec3 v = normalize(position);
		vec4 result = castRay(v,lastcamPos,firstRayDepth);
		vec3 checkPos = (result.w-0.001)*v - position;
		
		if (dot(checkPos,checkPos) < 0.00001)
			return texture(backBuffer,uv);
	}
	return vec4(0);
	
}

void main(){
	vec3 pixel = vec3(0,0,0);
	vec2 pixelCoords = gl_GlobalInvocationID.xy;
	vec3 v = normalize(vec3((pixelCoords-(0.5*resolution))/resolution.x,1.));
	v *= camMat;

		
	vec3 pos = camPos;
	vec4 result = castRay(v,camPos,firstRayDepth);
	vec3 ahitPos = pos + (result.w-0.002)*v;
	vec3 n = result.xyz;
	int bounces = 0;
	while (blockInfo == 5 && bounces < 4) {
		pos = ahitPos;
		v = reflect(v,n);
		result = castRay(v,pos,firstRayDepth);
		ahitPos += (result.w-0.002)*v;
		n = result.xyz;
		++bounces;
	}
	if (blockInfo == 5){
		writePixel(vec3(0));
		return;
	}

	uint blockType = blockInfo;
	vec3 hitPos = pos + (result.w-0.001)*v;

	float brightness = 0.7*getBrightness(ahitPos,n);
	
	if (nOutside(n)){
		writePixel(vec3(brightness));
		return;
	}
	
	#ifdef temporalFilter
	vec4 backColor = getBackColor(hitPos);
	#else
	vec4 backColor = vec4(0);
	#endif
	vec3 color;
	
	#ifdef secondRays
	
	float secondaryBrightness = 0.;
	for (int i = 0; i < secondRayCount; ++i){
		vec3 seed = vec3(ahitPos.y+float(i),ahitPos.x+frame,ahitPos.z-frame);
		float latitude = 2.*asin(random(seed));
		seed += vec3(7,13,103);
		float longitude = 2.*pi*random(seed);
		mat3 normMat;
		float sinlat = sin(latitude);
		vec3 v = vec3(sinlat*sin(longitude),sinlat*cos(longitude),1.+cos(latitude));
		float contribution = length(v);
		v /= contribution;
		if (n.x != 0.){
			normMat[0] = vec3(0,0,1);
			normMat[1] = vec3(0,1,0);
			normMat[2] = n;
		}
		else if (n.y != 0.){
			normMat[0] = vec3(1,0,0);
			normMat[1] = vec3(0,0,1);
			normMat[2] = n;
		}
		else{
			normMat[0] = vec3(1,0,0);
			normMat[1] = vec3(0,1,0);
			normMat[2] = n;
		}
		v = normMat*v;
		
		vec4 aresult = castRay(v,ahitPos,secondRayDepth);
		vec3 secondHitPos = ahitPos+(aresult.w-0.002)*v;
		secondaryBrightness += getBrightness(secondHitPos,aresult.xyz);
	}
	brightness += secondaryBrightness/float(secondRayCount);
	#endif
	color = brightness*getTexture(hitPos,n,blockType);
	
	if (backColor != vec4(0)){
		color = 0.1*color + 0.9*backColor.xyz;
	}
	writePixel(color);
}