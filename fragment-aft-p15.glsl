#version 330 core
//--- out_Color: ���ؽ� ���̴������Է¹޴»���
//--- FragColor: ����� ������ �����������ӹ��۷����޵�. 

in vec3 out_color;
uniform vec3 vColor;

out vec4 FragColor;
 
void main ()
{
	FragColor = vec4 (vColor, 1.0f);
}