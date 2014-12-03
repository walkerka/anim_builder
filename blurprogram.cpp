#include "blurprogram.h"
#include <math.h>

static const char *vertexShaderSource =
"attribute vec4 posTexcoord;\n"
"varying vec2 texcoord;\n"
"uniform mat4 mvp;\n"
"void main() {\n"
"   texcoord = posTexcoord.zw;\n"
"   gl_Position = mvp * vec4(posTexcoord.xy, 0.0, 1.0);\n"
"}\n";

static const char *fs0 =
"varying vec2 texcoord;\n"
"uniform sampler2D srcTex;\n"
"void main() {\n"
"	vec4 c = vec4(0, 0, 0, 0);\n"
"   vec4 cs;\n"
"   ivec2 size = textureSize(srcTex,0);\n"
"   vec2 sizeInv = vec2(1.0/size.x, 1.0/size.y);\n";

#define fs1 "   cs = texture2D(srcTex, texcoord + vec2(%d,%d) * sizeInv);\nc += vec4(cs.rgb * cs.a, cs.a) * "

static const char *fs2 =
"	if (c.a == 0)\n"
"	{\n"
"       gl_FragColor = vec4(0,0,0,0);\n"
"	}\n"
"	else\n"
"	{\n"
"   	gl_FragColor = vec4(c.rgb/c.a,c.a);\n"
"	}\n"
"}\n";

BlurProgram::BlurProgram(int radius)
    :mRadius(radius)
{
    BuildShaders();
}

BlurProgram::~BlurProgram()
{
    delete mHorizontalProgram;
    delete mVerticalProgram;
}

void BlurProgram::SetPositionTexcoord(float* vs)
{
    mHorizontalProgram->setAttributeArray("posTexcoord", GL_FLOAT, vs, 4);
    mVerticalProgram->setAttributeArray("posTexcoord", GL_FLOAT, vs, 4);
}

void BlurProgram::SetMvp(const QMatrix4x4& mat)
{
    mHorizontalProgram->setUniformValue("mvp", mat);
    mVerticalProgram->setUniformValue("mvp", mat);
}

void BlurProgram::Bind()
{
    mHorizontalProgram->bind();
}

void BlurProgram::BuildShaders()
{
    int radius = mRadius;

    if (radius < 0)
    {
        return;
    }

    QString psH(fs0);
    QString psV(fs0);
    char psStr[200];

    std::vector<float> samples;
    float total = 1.0f;
    samples.push_back(1.0f);
    for (int i = 0; i < radius; ++i)
    {
        float v = cosf(i / (float)radius);
        samples.push_back(v);
        total += v * 2;
    }

    sprintf(psStr, fs1, 0, 0);
    psH.append(psStr);
    sprintf(psStr, "%f;\n", 1.0f /total);
    psH.append(psStr);

    sprintf(psStr, fs1, 0, 0);
    psV.append(psStr);
    sprintf(psStr, "%f;\n", 1.0f /total);
    psV.append(psStr);

    for (int i = 0; i < radius; ++i)
    {
        float factor = samples[i] / total;
        sprintf(psStr, fs1, i + 1, 0);
        psH.append(psStr);
        sprintf(psStr, "%f;\n", factor);
        psH.append(psStr);
        sprintf(psStr, fs1, -(i + 1), 0);
        psH.append(psStr);
        sprintf(psStr, "%f;\n", factor);
        psH.append(psStr);

        sprintf(psStr, fs1, 0, i + 1);
        psV.append(psStr);
        sprintf(psStr, "%f;\n", factor);
        psV.append(psStr);
        sprintf(psStr, fs1, 0, -(i + 1));
        psV.append(psStr);
        sprintf(psStr, "%f;\n", factor);
        psV.append(psStr);
    }
    psH.append(fs2);
    psV.append(fs2);

    mHorizontalProgram = new QOpenGLShaderProgram();
    mHorizontalProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    mHorizontalProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, psH.toStdString().c_str());
    mHorizontalProgram->link();
    mHorizontalProgram->setUniformValue("srcTex", 0);

    mVerticalProgram = new QOpenGLShaderProgram();
    mVerticalProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    mVerticalProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, psV.toStdString().c_str());
    mVerticalProgram->link();
    mVerticalProgram->setUniformValue("srcTex", 0);
}
