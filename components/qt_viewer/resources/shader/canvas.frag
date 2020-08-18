#version 330
in vec4 vColor;                        // 从顶点着色器传来的输入变量（名称相同、类型相同）
out vec4 fColor;
void main(){
    fColor = vColor;
}
