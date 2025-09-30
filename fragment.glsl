#version 400

out vec4 color;


uniform vec2 resolution;
uniform vec3 bh_p;
uniform float bh_r;
uniform vec3 camPosGlobal;
uniform vec3 camDirGlobal;
uniform float disk_outer_radius;
uniform float disk_inner_radius;
uniform sampler2D hdrMap;

uniform vec3 camForward;
uniform vec3 camRight;
uniform vec3 camUp;
uniform float fov;

struct Ray {
    vec3 position;
    vec3 d;
};

//all shapes have certain signed distance functions:
//credit: https://iquilezles.org/articles/distfunctions/
float signedDistanceSphere(vec3 p, vec3 center, float r){

return length(p - center) - r;

}


float hitDisk(vec3 p, vec3 p_prev){
    float y_t1 = p.y;
    float y_t0 = p_prev.y;

    if (y_t1 * y_t0 >= 0.0001){
    //if no sign change then hasn't crossed the disk plane
    return 0.0f;
    }

    // we have crossed the accretion disk plane, lets check
    // that its within the inner and outer radius

    //inside outer ring
    float outer_dist = length(p.xz - bh_p.xz) - disk_outer_radius;
    if (outer_dist <= 0.00001) {
        //outside inner ring
        if (length(p.xz - bh_p.xz) - disk_inner_radius >= 0.00001){
            return abs(outer_dist) / disk_outer_radius;
        }
    }
    return 0.0f;

}


void rayMarch(float t) {
    //t here is the distance to march
    return;
}


void main() {
    color = vec4(0.0);
    vec2 uv = (gl_FragCoord.xy / resolution) * 2 - 1;
    uv.x *= resolution.x / resolution.y;

    //ray origin
    vec3 ro = camPosGlobal;


    //calculate ray direction
    float focalLength = 1.0 / tan(radians(fov) * 0.5);
    vec3 rd = normalize(camForward * focalLength + camRight * uv.x + camUp * uv.y);

    //create the ray
    Ray r = Ray(ro, rd);
    vec3 r_position_prev = r.position;

    //we can set max loops by changing the 100

    for (int time = 0; time < 15000; time += 1) {


        float dist = signedDistanceSphere(r.position, bh_p, bh_r);

        if (dist >= 1000){
        break;
        }


        if (dist < 0.001) {
        color += vec4(0.0);
        return;
        }

        //photon ring
        if ( (dist <= 0.01)  &&  (abs(dot(r.d, (bh_p - r.position))) < 3.4f )  )  {

        color += vec4(vec3(255.0/255, 50.0/255, 18.00/255), 1.0);
        return;

        }

        //temporary gravity

        dist = max(0.1, dist);
        float gravity =  (1 / dist);   //(1 / pow(dist, 1.0)) / 50000;
        vec3 gravDir = normalize(bh_p - r.position);

        r.d += gravDir * gravity * 0.001;
        r.d = normalize(r.d);


        float diskRatio = hitDisk(r.position, r_position_prev);
        if (diskRatio > 0.0){

            vec3 accretionColor = vec3(255.0/255, 50.0/255, 18.00/255);

            // Perfect spheres ring
            float ringNoise = abs(sin(  sqrt(   pow(r.position.x, 2) + pow(r.position.z,2)   )    ));

            color += vec4(accretionColor.xyz, 1.0) * diskRatio * ringNoise * 4;
        }
        
        r_position_prev = r.position;

        r.position += r.d * 0.01;
    }


    //this only happens if the ray doesn't hit anything. If this happens, we should map it to an HDR texture
    //credits https://community.khronos.org/t/random-values/75728
    
    // https://en.wikipedia.org/wiki/UV_mapping for uv sphere mapping
    float texU = 0.5 + atan(r.d.z, r.d.x) / (2 * 3.1415); 
    float texV = 0.5 + asin(r.d.y) / 3.1415;

    vec2 texUV = vec2(texU, texV);
 
    vec3 hdrColor = texture(hdrMap, texUV).rgb;
    color += vec4(hdrColor, 1.0);

    //can include this if you want to see ray direction
    //color = vec4(r.d, 1.0);

}