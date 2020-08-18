#version 330
layout(location = 0) in vec3 position;  // 位置变量的属性位置值为0
layout(location = 1) in vec3 color;     // 位置变量的属性位置值为1
out vec4 vColor;                        // 为片段着色器指定一个颜色输出
void main(){
    gl_Position = vec4(position, 1.0);
    vColor = vec4(color, 1.0);          // 设置输出变量颜色
}
