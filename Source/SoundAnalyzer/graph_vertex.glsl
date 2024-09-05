attribute vec4 qt_Vertex;
attribute vec4 qt_MultiTexCoord0;
uniform mat4 qt_ModelViewProjectionMatrix;
varying vec4 qt_TexCoord0;

attribute vec4 vertices;
uniform mat4 mvp_matrix;

void main(void)
{
    gl_Position = mvp_matrix * vertices;
}
