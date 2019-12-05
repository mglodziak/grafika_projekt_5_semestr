#version 330
//do t³a
out vec4 FragColor;
in vec3 pos;
in vec3 outcol;

void main()
{
	
	if(pos.z==0.0f)
		gl_FragColor  = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	else if(pos.z>0 && pos.z<0.25)
		gl_FragColor  = vec4(0.0f, 0.0f+4*pos.z, 1.0f, 1.0f);
	
	else if(pos.z>0.25 && pos.z<0.5)
		gl_FragColor  = vec4(0.0f, 1.0f, 1.0f-(4*(pos.z-0.25)), 1.0f);

	else if(pos.z>0.5 && pos.z<0.75)
		gl_FragColor  = vec4(0.0f+(4*(pos.z-0.5)), 1.0f, 0.0f,1.0f);

	else if(pos.z>0.75 && pos.z<1.0)
		gl_FragColor  = vec4(1.0f, 1.0f-(4*(pos.z-0.75)), 0.0f,1.0f);


	else 
		gl_FragColor  = vec4(1.0f, 1.0f, 1.0f, 1.0f);

} 
