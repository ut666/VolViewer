// vertex to fragment shader io
varying vec3 N;
varying vec3 I;
varying vec4 Cs;


// entry point
void main()
{

    float edgefalloff = 1.0;
    float opac = dot(normalize(N), normalize(-I));
    opac = abs(opac);
    opac = 1.0-pow(opac, edgefalloff);
    
    gl_FragColor =  opac * Cs;
    //gl_FragColor.a = opac;
    //gl_FragColor = 1.0 - gl_FragColor;
    gl_FragColor.w = 1.0;
}
