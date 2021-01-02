#define UVINBOUND uv.x < halfWidth && uv.x > -halfWidth && uv.y < halfWidth && uv.y > -halfWidth

float planeIntersection(vec3 N,vec3 V,vec3 pos){
	float distToPlane = -dot(pos,N);
	float rayDotPlane = -dot(V,N);
    if (rayDotPlane > 0. && distToPlane > 0.)
        return distToPlane/rayDotPlane;
    else return 0.;
}

float ground(vec2 coord){
	return cos(0.2*coord.x)+cos(0.2*coord.y)+cos(coord.x/10.);
}

vec3 planeColor(vec3 V,vec3 N,vec3 lightVec,vec2 uv){
    float lightDist = length(lightVec);
    float threshold = 1./lightDist/lightDist/lightDist;
    //float brightness = 1000. * threshold * dot(N,lightVec);
    float brightness = dot(N,lightVec/lightDist);
    vec3 reflected = reflect(V,N);
    
    vec2 pixel = vec2(1./1024.);
    //float shine = 600. * threshold * pow(max(0.,dot(reflected,lightVec/lightDist)),50.);
    float shine = 0.3*pow(max(0.,dot(reflected,lightVec/lightDist)),50.);
    return (brightness+shine+0.2)*texture(iChannel0,501.*pixel+8.*pixel*(0.5*uv+0.5)).xyz;
}
bool filled(vec3 gridPos){
	/*const int width = 16;
    ivec3 pos = ivec3(int(gridPos.x),int(gridPos.y),int(gridPos.z)) + ivec3(8);
    ivec2 Z = ivec2(width*(pos.z%width),width*(pos.z/width));
    
    return length(texelFetch(iChannel1,Z+pos.xy,0).xyz) > 0.9;*/
    return gridPos.y < ground(vec2(gridPos.x,gridPos.z));
}

vec4 boxIntersection(vec3 viewingNormal, vec3 boxPos){
    vec3 X = vec3(1,0,0);
    vec3 Y = vec3(0,1,0);
   	vec3 Z = vec3(0,0,1);
    
    float halfWidth = 0.5;
    for (int i = 0; i < 2; ++i){
        
        
        vec2 uv;
        vec3 pos;
        
        vec3 planePos = boxPos - halfWidth*X;
        float t = planeIntersection(X,viewingNormal,planePos);
        if (t != 0.){
            pos = t*viewingNormal;

            uv = (pos - planePos).yz;
            if (UVINBOUND){
                return vec4(uv,t,0.);
            }
        }

        planePos = boxPos - halfWidth*Y;
        t = planeIntersection(Y,viewingNormal,planePos);
        if (t != 0.){
            pos = t*viewingNormal;

            uv = (pos - planePos).xz;
            if (UVINBOUND){
                return vec4(uv,t,0.1);
            }
        }
        planePos = boxPos - halfWidth*Z;
        t = planeIntersection(Z,viewingNormal,planePos);;
        if (t != 0.){
            pos = -t*viewingNormal;

            uv = (pos + planePos).xy;
            if (UVINBOUND){
                return vec4(uv,t,0.2);
            }
        }

        X *= -1.;
        Y *= -1.;
        Z *= -1.;
    }
    return vec4(0);
}  


void mainImage()
{
    vec2 uv = fragCoord.xy/resolution.x;
    float aspect = iResolution.x/resolution.y;
    
    
    float time = 0.1*iTime;
    //Based on uv
    vec3 viewingNormal = normalize( vec3( uv-vec2(0.5,0.5/aspect),1));
    vec3 cameraZ = normalize(vec3(-sin(time),0.1*sin(iTime),cos(time)));
    vec3 cameraX = normalize(cross(vec3(0,1,0),cameraZ));
    vec3 cameraY = cross(cameraZ,cameraX);
    viewingNormal = viewingNormal.x*cameraX + viewingNormal.y*cameraY + viewingNormal.z*cameraZ;
    
    
    const vec3 planeX = vec3(1,0,0);
    const vec3 planeY = vec3(0,1,0);
    const vec3 planeZ = vec3(0,0,1);
    
  	
    vec3 cameraPos = vec3(100.*cos(time),0,100.*sin(time));
   	cameraPos.y =  2.+ground(vec2(cameraPos.x,cameraPos.z));
    vec3 currentPos = cameraPos;
    vec3 lightPos = vec3(1000,1000,-40);
    
    vec3 gridPos = vec3(floor(currentPos.x),floor(currentPos.y),floor(currentPos.z));
    bool found = false;
    vec3 colour = vec3(0.6,0.7,1); //Sky color
    
    for(int i = 0; i < 100; ++i){
        
    	vec4 uvt = boxIntersection(viewingNormal,gridPos+vec3(0.5)-currentPos);
        
        currentPos += viewingNormal*(uvt.z+0.001); 
        
        gridPos = vec3(floor(currentPos.x),floor(currentPos.y),floor(currentPos.z));
        
        if (filled(gridPos)){
        	currentPos -= viewingNormal*0.001;
            vec3 lightVec = lightPos - currentPos;
        	if (!found){
            	if (uvt.w == 0.) 
            		colour = planeColor(viewingNormal, -sign(viewingNormal.x)*planeX, lightVec,uvt.xy);
            	else if (uvt.w == 0.1)
            		colour = planeColor(viewingNormal, -sign(viewingNormal.y)*planeY, lightVec,uvt.xy);
            	else if (uvt.w == 0.2) 
            		colour = planeColor(viewingNormal, -sign(viewingNormal.z)*planeZ, lightVec,uvt.xy);
            	else fragColor = vec4(vec3(0.2,0.3,0.5),1);
                viewingNormal = normalize(lightVec);
                found = true;
        	}
            else{
            	colour *= 0.45;
                break;
            }
            
            
        }
    }
    fragColor = vec4(colour, 1);
     
		
}