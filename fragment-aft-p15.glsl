#version 330 core
//--- out_Color: 버텍스 세이더에서입력받는색상값
//--- FragColor: 출력할 색상의 값으로프레임버퍼로전달됨. 

in vec3 out_color;
uniform vec3 vColor;

out vec4 FragColor;
 
void main ()
{
	FragColor = vec4 (vColor, 1.0f);
}