#version 330
layout(location = 0) in vec3 position;  // 位置变量的属性位置值为0
layout(location = 1) in vec3 color;    // 位置变量的属性位置值为1
out vec4 vColor;                        // 为片段着色器指定一个颜色输出
uniform mat4 modelToWorld;              // 在OpenGL程序代码中设定这个, 全局变量
uniform mat4 worldToCamera;
uniform mat4 cameraToView;
void main(){
    gl_Position = cameraToView * worldToCamera * modelToWorld * vec4(position, 1.0);  // 这里是个不恰当的例子，建议把三个矩阵在Shader外面计算好，这样只需一次矩阵向量乘法
    vColor = vec4(color, 1.0);         // 设置输出变量颜色
}
