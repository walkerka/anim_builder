#include <GL/glew.h>
#include <GL/gl.h>
#include "openglrenderer.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <memory.h>
#include <assert.h>

GLRenderTarget::GLRenderTarget(GLTexture* texture)
    :mWidth(texture->GetWidth())
    ,mHeight(texture->GetHeight())
    ,mFrameBufferId(0)
    ,mTexture(texture)
{
    int textureId = texture->mTextureId;
    if (textureId == 0)
    {
        return;
    }

    GLuint frameBufferId = 0;
    glGenFramebuffers(1, &frameBufferId);

    if (frameBufferId == 0)
    {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mFrameBufferId = frameBufferId;
}

GLRenderTarget::~GLRenderTarget()
{
    if (mFrameBufferId <= 0)
    {
        return;
    }
    GLuint fid = mFrameBufferId;
    glDeleteFramebuffers(1, &fid);
    if (mTexture)
    {
        delete mTexture;
    }
}

void GLRenderTarget::Blit(GLRenderTarget* src, bool smooth)
{
    if (!src)
    {
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBufferId);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src->mFrameBufferId);
    glBlitFramebuffer(0, 0, src->mWidth, src->mHeight, 0, 0, mWidth, mHeight, GL_COLOR_BUFFER_BIT, smooth ? GL_LINEAR : GL_NEAREST);
}

GLTexture::GLTexture(int width, int height, int* data, bool topDownOrder, int channels, bool magSmooth, bool repeat, bool useMipmap)
    :mWidth(width)
    ,mHeight(height)
    ,mTextureId(0)
{
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    if (textureId == 0)
    {
        return;
    }
    glBindTexture(GL_TEXTURE_2D, textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLenum format = GL_RGBA;
    if (channels == 1)
    {
        format = GL_LUMINANCE;
    }
    else if (channels == 3)
    {
        format = GL_RGB;
    }

    if (topDownOrder)
    {
        int rowSize = channels * width;
        unsigned char* tempData = new unsigned char[rowSize * height];
        unsigned char* pData = (unsigned char*) data;
        for (int i = 0; i < height; ++i)
        {
            memcpy(tempData + i * rowSize, pData + (height - 1 - i) * rowSize, rowSize);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, tempData);
        delete tempData;
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }

    if (useMipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magSmooth ? GL_LINEAR : GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    mTextureId = textureId;
}

GLTexture::~GLTexture()
{
    if (mTextureId == 0)
    {
        return;
    }

    GLuint t = mTextureId;
    glDeleteTextures(1, &t);
}

void GLTexture::ReadTexture(int* data)
{
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture::WriteTexture(int* data, int w, int h, bool useMipmap)
{
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glPixelStoref(GL_PACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

    if (useMipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}


static int CreateShader(GLuint programId, const char* source, GLenum shaderType)
{
    GLint status = 0;
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, (const char**)&source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status)
    {
        glAttachShader(programId, shader);
        return shader;
    }
    else
    {
        const int MAX_INFO_LOG_SIZE = 1000;
        GLchar log[MAX_INFO_LOG_SIZE];
        glGetShaderInfoLog(shader, MAX_INFO_LOG_SIZE, NULL, log);
        printf(log); fflush(stdout);
        return 0;
    }
}

GLShaderProgram::GLShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
    :mProgramId(0)
{
    GLuint programId = 0;
    GLuint vertexShaderId = 0;
    GLuint fragmentShaderId = 0;

    bool failed = false;
    do
    {
        programId = glCreateProgram();
        if (programId == 0)
        {
            failed = true;
            break;
        }

        vertexShaderId = CreateShader(programId, vertexShaderSource, GL_VERTEX_SHADER);
        if (vertexShaderId == 0)
        {
            failed = true;
            break;
        }

        fragmentShaderId = CreateShader(programId, fragmentShaderSource, GL_FRAGMENT_SHADER);
        if (fragmentShaderId == 0)
        {
            failed = true;
            break;
        }

        GLint status = 0;
        glLinkProgram(programId);
        glGetProgramiv(programId, GL_LINK_STATUS, &status);
        if (!status)
        {
            const int MAX_INFO_LOG_SIZE = 1000;
            GLchar log[MAX_INFO_LOG_SIZE];
            glGetProgramInfoLog(programId, MAX_INFO_LOG_SIZE, NULL, log);
            printf(log); fflush(stdout);
        }
    }
    while(0);

    if (vertexShaderId)
    {
        glDeleteShader(vertexShaderId);
    }
    if (fragmentShaderId)
    {
        glDeleteShader(fragmentShaderId);
    }

    if (failed && programId)
    {
        glDeleteProgram(programId);
        return;
    }

    mProgramId = programId;
}

GLShaderProgram::~GLShaderProgram()
{
    if (mProgramId <= 0)
    {
        return;
    }
    glDeleteProgram((GLuint)mProgramId);
}

void GLShaderProgram::SetAttribute(const char* name, int index, int numComponents, float* attr)
{
    glVertexAttribPointer(glGetAttribLocation(mProgramId, name), numComponents, GL_FLOAT, GL_FALSE, 0, attr);
    glEnableVertexAttribArray(index);
}

void GLShaderProgram::SetTexture(const char* name, int index, GLTexture* texture)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
    glUniform1i(glGetUniformLocation(mProgramId, name), index);
}

void GLShaderProgram::SetColor(const char* name, float r, float g, float b, float a)
{
    glUniform4f(glGetUniformLocation(mProgramId, name), r, g, b, a);
}

void GLShaderProgram::SetMatrix(const char* name, float* mat)
{
    glUniformMatrix4fv(glGetUniformLocation(mProgramId, name), 1, GL_FALSE, mat);
}

void GLShaderProgram::SetFloat4(const char* name, float* v)
{
    glUniform4fv(glGetUniformLocation(mProgramId, name), 1, v);
}

void GLShaderProgram::SetFloat3(const char* name, float* v)
{
    glUniform3fv(glGetUniformLocation(mProgramId, name), 1, v);
}

void GLShaderProgram::SetFloat2(const char* name, float* v)
{
    glUniform2fv(glGetUniformLocation(mProgramId, name), 1, v);
}

void GLShaderProgram::SetFloat(const char* name, float v)
{
    glUniform1f(glGetUniformLocation(mProgramId, name), v);
}

void GLShaderProgram::SetFloat4Array(const char* name, float* v, int count)
{
    glUniform4fv(glGetUniformLocation(mProgramId, name), count, v);
}

void GLShaderProgram::SetFloat3Array(const char* name, float* v, int count)
{
    glUniform3fv(glGetUniformLocation(mProgramId, name), count, v);
}

void GLShaderProgram::SetFloat2Array(const char* name, float* v, int count)
{
    glUniform2fv(glGetUniformLocation(mProgramId, name), count, v);
}

void GLShaderProgram::SetFloatArray(const char* name, float* v, int count)
{
    glUniform1fv(glGetUniformLocation(mProgramId, name), count, v);
}

GLUniformValue::GLUniformValue()
    :mType(ValueTypeNone)
{
}

GLUniformValue::GLUniformValue(int index, GLTexture* texture)
    :mType(ValueTypeTexture)
{
    TextureValue tv;
    tv.index = index;
    tv.texture = texture;
    mValue.texture = tv;
}

GLUniformValue::GLUniformValue(float x, float y, float z, float w)
    :mType(ValueTypeFloat4)
{
    mValue.vec4[0] = x;
    mValue.vec4[1] = y;
    mValue.vec4[2] = z;
    mValue.vec4[3] = w;
}

GLUniformValue::GLUniformValue(float x, float y, float z)
    :mType(ValueTypeFloat3)
{
    mValue.vec3[0] = x;
    mValue.vec3[1] = y;
    mValue.vec3[2] = z;
}

GLUniformValue::GLUniformValue(float x, float y)
    :mType(ValueTypeFloat2)
{
    mValue.vec2[0] = x;
    mValue.vec2[1] = y;
}

GLUniformValue::GLUniformValue(float v)
    :mType(ValueTypeFloat)
{
    mValue.value = v;
}

GLUniformValue::GLUniformValue(const float* mat)
    :mType(ValueTypeMatrix4)
{
    memcpy(mValue.mat4, mat, sizeof(float) * 16);
}

GLUniformValue::~GLUniformValue()
{
}

void GLUniformValue::Apply(const char* name, GLShaderProgram* program)
{
    switch (mType)
    {
    case GLUniformValue::ValueTypeTexture:
        program->SetTexture(name, mValue.texture.index, mValue.texture.texture);
        break;
    case GLUniformValue::ValueTypeFloat:
        program->SetFloat(name, mValue.value);
        break;
    case GLUniformValue::ValueTypeFloat2:
        program->SetFloat2(name, mValue.vec2);
        break;
    case GLUniformValue::ValueTypeFloat3:
        program->SetFloat3(name, mValue.vec3);
        break;
    case GLUniformValue::ValueTypeFloat4:
        program->SetFloat4(name, mValue.vec4);
        break;
    case GLUniformValue::ValueTypeMatrix4:
        program->SetMatrix(name, mValue.mat4);
        break;
    default:
        break;
    }
}

GLShaderState::GLShaderState()
{
}

GLShaderState::~GLShaderState()
{
}

void GLShaderState::SetValue(const char* name, const GLUniformValue& value)
{
    mValues[name] = value;
}

void GLShaderState::Apply(GLShaderProgram* program)
{
    for (ValueMap::iterator it = mValues.begin(); it != mValues.end(); ++it)
    {
        it->second.Apply(it->first.c_str(), program);
    }
}

GLAttribute::GLAttribute(int components, int count)
    :mComponets(components)
    ,mCount(count)
    ,mData(0)
{
    assert(mComponets > 0 && mComponets <= 4);
    mData = new float[components * count];
}

GLAttribute::~GLAttribute()
{
    delete[] mData;
}

void GLAttribute::SetValue(int index, float value)
{
    assert(mComponets == 1 && index >= 0 && index < mCount);
    mData[index] = value;
}

void GLAttribute::SetValue(int index, float x, float y)
{
    assert(mComponets == 2 && index >= 0 && index < mCount);
    float* p = mData + index * mComponets;
    p[0] = x;
    p[1] = y;
}

void GLAttribute::SetValue(int index, float x, float y, float z)
{
    assert(mComponets == 3 && index >= 0 && index < mCount);
    float* p = mData + index * mComponets;
    p[0] = x;
    p[1] = y;
    p[2] = z;
}

void GLAttribute::SetValue(int index, float x, float y, float z, float w)
{
    assert(mComponets == 4 && index >= 0 && index < mCount);
    float* p = mData + index * mComponets;
    p[0] = x;
    p[1] = y;
    p[2] = z;
    p[3] = w;
}

GLMesh::GLMesh(int vertexCount, int indexCount)
    :mVertexCount(vertexCount)
    ,mIndexCount(indexCount)
    ,mIndices(0)
{
    mIndices = new unsigned int[indexCount];
}

GLMesh::~GLMesh()
{
    for (AttributeMap::iterator it = mAttributes.begin(); it != mAttributes.end(); ++it)
    {
        delete it->second;
    }
    delete mIndices;
}

GLAttribute* GLMesh::AddAttribute(const char* name, int components)
{
    AttributeMap::iterator it = mAttributes.find(name);
    if (it != mAttributes.end())
    {
        return NULL;
    }

    GLAttribute* attr = new GLAttribute(components, mVertexCount);
    mAttributes[name] = attr;
    return attr;
}

GLAttribute* GLMesh::GetAttribute(const char* name)
{
    AttributeMap::iterator it = mAttributes.find(name);
    if (it != mAttributes.end())
    {
        return it->second;
    }
    return NULL;
}

void GLMesh::SetIndexValue(int index, unsigned int value)
{
    mIndices[index] = value;
}

void GLMesh::Apply(GLShaderProgram* program)
{
    int i = 0;
    for (AttributeMap::iterator it = mAttributes.begin(); it != mAttributes.end(); ++it)
    {
        GLAttribute* attr = it->second;
        program->SetAttribute(it->first.c_str(), i++, attr->mComponets, attr->mData);
    }
}

GLShape::GLShape()
    :mMesh(NULL)
{
    mColor[0] = 1;
    mColor[1] = 1;
    mColor[2] = 1;
    mColor[3] = 1;
}

GLShape::~GLShape()
{
    delete mMesh;
}

void GLShape::SetColor(float r, float g, float b, float a)
{
    mColor[0] = r;
    mColor[1] = g;
    mColor[2] = b;
    mColor[3] = a;
}

void GLShape::Build()
{
    if (mMesh)
    {
        delete mMesh;
        mMesh = NULL;
    }

    int n = (int)(mVertices.size() / 2);
    int ni = (int)(mIndices.size());
    mMesh = new GLMesh(n, ni);
    GLAttribute* posAttr = mMesh->AddAttribute("position", 2);

    int idx = 0;
    for (int i = 0; i < n; ++i)
    {
        posAttr->SetValue(idx++, mVertices[i * 2], mVertices[i * 2 + 1]);
    }

    for (int i = 0; i < ni; ++i)
    {
        mMesh->SetIndexValue(i, mIndices[i]);
    }
}

void GLShape::Clear()
{
    mVertices.clear();
    mIndices.clear();
}

void GLShape::AddRect(float x, float y, float width, float height)
{
    unsigned int n = mVertices.size() / 2;
    mVertices.push_back(x); mVertices.push_back(y);
    mVertices.push_back(x + width); mVertices.push_back(y);
    mVertices.push_back(x + width); mVertices.push_back(y + height);
    mVertices.push_back(x); mVertices.push_back(y + height);
    mIndices.push_back(n + 0);
    mIndices.push_back(n + 1);
    mIndices.push_back(n + 2);
    mIndices.push_back(n + 0);
    mIndices.push_back(n + 2);
    mIndices.push_back(n + 3);
}

void GLShape::AddCircle(float x, float y, float r)
{
    const float d = 0.2f;
    unsigned int n = mVertices.size() / 2;
    // angle = 3.1415926f * 2 / samples
    // d = r - r * cosf(angle/2)
    int samples = (int)ceilf(3.1415926f / acosf((r - d) / r));
    if (samples < 3)
    {
        samples = 3;
    }

    mVertices.push_back(x);
    mVertices.push_back(y);
    for (int i = 0; i < samples; ++i)
    {
        float angle = i * 3.1415926f * 2 / samples;
        mVertices.push_back(x + r * cosf(angle));
        mVertices.push_back(y + r * sinf(angle));
        mIndices.push_back(n + 0);
        mIndices.push_back(n + 1 + i);
        mIndices.push_back(n + 1 + ((i + 1) % samples));
    }
}

void GLShape::AddLine(float x0, float y0, float x1, float y1, float w)
{
    AddLineV(x0, y0, w, x1, y1, w);
}

void GLShape::AddLineV(float x0, float y0, float w0, float x1, float y1, float w1)
{
    unsigned int n = mVertices.size() / 2;
    float vx = x1 - x0;
    float vy = y1 - y0;
    float upx = -vy;
    float upy = vx;
    float len = sqrt(upx * upx + upy * upy);
    upx /= len;
    upy /= len;

    float r0 = w0 * 0.5f;
    float r1 = w1 * 0.5f;

    mVertices.push_back(x0 - upx * r0); mVertices.push_back(y0 - upy * r0);
    mVertices.push_back(x1 - upx * r1); mVertices.push_back(y1 - upy * r1);
    mVertices.push_back(x1 + upx * r1); mVertices.push_back(y1 + upy * r1);
    mVertices.push_back(x0 + upx * r0); mVertices.push_back(y0 + upy * r0);
    mIndices.push_back(n + 0);
    mIndices.push_back(n + 1);
    mIndices.push_back(n + 2);
    mIndices.push_back(n + 0);
    mIndices.push_back(n + 2);
    mIndices.push_back(n + 3);
}

class BlurProgram
{
    friend class GLRenderer;
public:
    BlurProgram(int radius);
    ~BlurProgram();

private:
    void BuildShaders();

private:
    GLShaderProgram* mHorizontalProgram;
    GLShaderProgram* mVerticalProgram;
    int mRadius;
};

static const char *vsBlur =
"attribute vec4 posTexcoord;\n"
"varying vec2 texcoord;\n"
"uniform mat4 mvp;\n"
"void main() {\n"
"   texcoord = posTexcoord.zw;\n"
"   gl_Position = mvp * vec4(posTexcoord.xy, 0.0, 1.0);\n"
"}\n";

static const char *fsBlur0 =
"varying vec2 texcoord;\n"
"uniform sampler2D srcTex;\n"
"uniform vec2 texSizeInv;"
"void main() {\n"
"	vec4 c = vec4(0, 0, 0, 0);\n"
"	vec4 cp = vec4(0, 0, 0, 0);\n";
static const char *fsBlur1 =
"cp = texture2D(srcTex, texcoord + vec2(%f * texSizeInv.x,%f * texSizeInv.y));\n"
"c += vec4(cp.rgb * cp.a, cp.a) * %f;\n";
static const char *fsBlur2 =
"  	gl_FragColor = vec4(c.rgb/c.a,c.a);\n"
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

void BlurProgram::BuildShaders()
{
    int radius = mRadius;

    if (radius < 0)
    {
        return;
    }

    std::string psH(fsBlur0);
    std::string psV(fsBlur0);

    std::vector<double> samples;
    std::vector<double> offsets;
    double total = 0;
    for (int i = 0; i <= radius; ++i)
    {
        int off = i > 0 ? -1 : 0;
        float v = cos((i + off) / (double)radius);
        samples.push_back(v);
        offsets.push_back(i);
        total += v;
        if (i > 0)
        {
            samples.push_back(v);
            offsets.push_back(-i);
            total += v;
        }
    }

    char psStr[200];
    for (size_t i = 0; i < samples.size(); ++i)
    {
        double factor = samples[i] / total;
        sprintf(psStr, fsBlur1, offsets[i], 0.0, factor);
        psH.append(psStr);
        sprintf(psStr, fsBlur1, 0.0, offsets[i], factor);
        psV.append(psStr);
    }
    psH.append(fsBlur2);
    psV.append(fsBlur2);

//    printf("%s", psH.c_str());fflush(stdout);
//    printf("%s", psV.c_str());fflush(stdout);

    mHorizontalProgram = new GLShaderProgram(vsBlur, psH.c_str());
    mVerticalProgram = new GLShaderProgram(vsBlur, psV.c_str());
}


const char* vsBlendSource =
"attribute vec4 posTexcoord;\n"
"varying vec2 texcoord;\n"
"uniform mat4 mvp;\n"
"void main() {\n"
"   texcoord = posTexcoord.zw;\n"
"   gl_Position = mvp * vec4(posTexcoord.xy, 0.0, 1.0);\n"
"}\n";

const char* psBlendSource =
"varying vec2 texcoord;\n"
"uniform sampler2D srcTex;\n"
"uniform vec4 color;\n"
"void main() {\n"
"   vec4 c = texture2D(srcTex, texcoord) * color;\n"
"   c = vec4(c.rgb * c.a, c.a);\n"
"   gl_FragColor = vec4(c.rgb/c.a,c.a);\n"
"}\n";

const char* vsBlendSourceOver =
"attribute vec4 posTexcoord;\n"
"varying vec2 texcoord;\n"
"uniform mat4 mvp;\n"
"void main() {\n"
"   texcoord = posTexcoord.zw;\n"
"   gl_Position = mvp * vec4(posTexcoord.xy, 0.0, 1.0);\n"
"}\n";

const char* psBlendSourceOver =
"varying vec2 texcoord;\n"
"uniform sampler2D srcTex;\n"
"uniform sampler2D dstTex;\n"
"uniform vec4 color;\n"
"void main() {\n"
"   vec4 s = texture2D(srcTex, texcoord) * color;\n"
"   vec4 d = texture2D(dstTex, texcoord);\n"
"   float da = d.a * (1.0 - s.a);\n"
"   float a = s.a + da;\n"
"   gl_FragColor = vec4((s.rgb * s.a + d.rgb * da) / a, a);\n"
"}\n";

const char* vsBlendMultiply =
"attribute vec4 posTexcoord;\n"
"varying vec2 texcoord;\n"
"uniform mat4 mvp;\n"
"void main() {\n"
"   texcoord = posTexcoord.zw;\n"
"   gl_Position = mvp * vec4(posTexcoord.xy, 0.0, 1.0);\n"
"}\n";

const char* psBlendMultiply =
"varying vec2 texcoord;\n"
"uniform sampler2D srcTex;\n"
"uniform sampler2D dstTex;\n"
"uniform vec4 color;\n"
"void main() {\n"
"   vec4 s = texture2D(srcTex, texcoord) * color;\n"
"   vec4 d = texture2D(dstTex, texcoord);\n"
"   s.rgb *= s.a;\n"
"   d.rgb *= d.a;\n"
"   float da = d.a * (1.0 - s.a);\n"
"   float a = s.a + da;\n"
"   gl_FragColor = vec4((s.rgb * d.rgb * s.a + d.rgb * da) / a, a);\n"
"   gl_FragColor.rgb /= gl_FragColor.a;\n"
"}\n";

const char* vsMask =
"attribute vec4 posTexcoord;\n"
"varying vec2 texcoord;\n"
"uniform mat4 mvp;\n"
"void main() {\n"
"   texcoord = posTexcoord.zw;\n"
"   gl_Position = mvp * vec4(posTexcoord.xy, 0.0, 1.0);\n"
"}\n";

const char* psMask =
"varying vec2 texcoord;\n"
"uniform sampler2D srcTex;\n"
"uniform sampler2D colorMaskTex;\n"
"uniform sampler2D alphaMaskTex;\n"
"uniform float alpha;\n"
"void main() {\n"
"   float a = texture2D(colorMaskTex, texcoord).r * texture2D(alphaMaskTex, texcoord).a * alpha;\n"
"   gl_FragColor = texture2D(srcTex, texcoord) * vec4(1.0,1.0,1.0,a);\n"
"}\n";

const char* vsShape =
"attribute vec2 position;\n"
"uniform mat4 mvp;\n"
"void main() {\n"
"   gl_Position = mvp * vec4(position, 0.0, 1.0);\n"
"}\n";

const char* psShape =
"uniform vec4 color;\n"
"void main() {\n"
"   gl_FragColor = color;\n"
"}\n";

GLRenderer::GLRenderer()
    :mWidth(1)
    ,mHeight(1)
{
    glewInit();
    int data = 0xFFFFFFFF;
    mDefaultTexture = new GLTexture(1, 1, &data, false, 4, false, false, false);
    mBlendSourceProgram = new GLShaderProgram(vsBlendSource, psBlendSource);
    mBlendSourceOverProgram = new GLShaderProgram(vsBlendSourceOver, psBlendSourceOver);
    mBlendMultiplyProgram = new GLShaderProgram(vsBlendMultiply, psBlendMultiply);
    mMaskProgram = new GLShaderProgram(vsMask, psMask);
    mShapeProgram = new GLShaderProgram(vsShape, psShape);
    memset(mBlurPrograms, 0, sizeof(mBlurPrograms));

    glClearColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
}

GLRenderer::~GLRenderer()
{
    for (int i = 0; i < MAX_BLUR_RADIUS; ++i)
    {
        delete mBlurPrograms[i];
    }
    delete mBlendMultiplyProgram;
    delete mBlendSourceOverProgram;
    delete mBlendSourceProgram;
    delete mMaskProgram;
    delete mDefaultTexture;
    delete mShapeProgram;
}

GLTexture* GLRenderer::CreateTexture(int width, int height, int* data, bool topDownOrder)
{
    return new GLTexture(width, height, data, topDownOrder, 4, false, false, false);
}

GLRenderTarget* GLRenderer::CreateTarget(int width, int height, int* data, bool topDownOrder)
{
    return new GLRenderTarget(new GLTexture(width, height, data, topDownOrder, 4, false, false, false));
}

void GLRenderer::Clear(GLRenderTarget* target, float r, float g, float b, float a)
{
    glBindFramebuffer(GL_FRAMEBUFFER, target ? target->mFrameBufferId : 0);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderer::SetScreenSize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

void GLRenderer::UseProgram(GLShaderProgram* program)
{
    glUseProgram(program ? program->mProgramId : 0);
}

void GLRenderer::SetRenderTarget(GLRenderTarget* target)
{
    if (target)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, target->mFrameBufferId);
        glViewport(0, 0, target->GetWidth(), target->GetHeight());
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, mWidth, mHeight);
    }

}

void GLRenderer::BlendSource(GLRenderTarget* target, GLTexture* src, float* mvp, float r, float g, float b, float a)
{
    SetRenderTarget(target);

    float w = (float)src->GetWidth();
    float h = (float)src->GetHeight();
    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
           w, 0.0f, 1.0f, 0.0f,
        0.0f,    h, 0.0f, 1.0f,
           w,    h, 1.0f, 1.0f
    };

    glUseProgram(mBlendSourceProgram->mProgramId);
    mBlendSourceProgram->SetColor("color", r, g, b, a);
    mBlendSourceProgram->SetTexture("srcTex", 0, src);
    mBlendSourceProgram->SetMatrix("mvp", mvp);
    mBlendSourceProgram->SetAttribute("posTexcoord", 0, 4, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLRenderer::BlendSourceOver(GLRenderTarget* target, GLTexture* dst, GLTexture* src, float* mvp, float r, float g, float b, float a)
{
    SetRenderTarget(target);

    float w = (float)src->GetWidth();
    float h = (float)src->GetHeight();
    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
           w, 0.0f, 1.0f, 0.0f,
        0.0f,    h, 0.0f, 1.0f,
           w,    h, 1.0f, 1.0f
    };

    glUseProgram(mBlendSourceOverProgram->mProgramId);
    mBlendSourceOverProgram->SetColor("color", r, g, b, a);
    mBlendSourceOverProgram->SetTexture("srcTex", 0, src);
    mBlendSourceOverProgram->SetTexture("srcTex", 1, dst);
    mBlendSourceOverProgram->SetMatrix("mvp", mvp);
    mBlendSourceOverProgram->SetAttribute("posTexcoord", 0, 4, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLRenderer::BlendMultipy(GLRenderTarget* target, GLTexture* dst, GLTexture* src, float* mvp, float r, float g, float b, float a)
{
    SetRenderTarget(target);

    float w = (float)src->GetWidth();
    float h = (float)src->GetHeight();
    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
           w, 0.0f, 1.0f, 0.0f,
        0.0f,    h, 0.0f, 1.0f,
           w,    h, 1.0f, 1.0f
    };

    glUseProgram(mBlendMultiplyProgram->mProgramId);
    mBlendMultiplyProgram->SetColor("color", r, g, b, a);
    mBlendMultiplyProgram->SetTexture("srcTex", 0, src);
    mBlendMultiplyProgram->SetTexture("srcTex", 1, dst);
    mBlendMultiplyProgram->SetMatrix("mvp", mvp);
    mBlendMultiplyProgram->SetAttribute("posTexcoord", 0, 4, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLRenderer::Blur(GLRenderTarget* target, GLRenderTarget* targetTemp, float* mvp, GLTexture* src, int radius)
{
    if (radius > MAX_BLUR_RADIUS)
    {
        radius = MAX_BLUR_RADIUS;
        printf("Too large blur radius, clamp to %d", MAX_BLUR_RADIUS);
        fflush(stdout);
    }

    if (!mBlurPrograms[radius])
    {
        mBlurPrograms[radius] = new BlurProgram(radius);
    }

    float w = (float)src->GetWidth();
    float h = (float)src->GetHeight();
    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
           w, 0.0f, 1.0f, 0.0f,
        0.0f,    h, 0.0f, 1.0f,
           w,    h, 1.0f, 1.0f
    };

    BlurProgram* prog = mBlurPrograms[radius];
    float texSizeInv[] = {1.0f / w, 1.0f /h};

    GLShaderProgram* hProg = prog->mHorizontalProgram;
    SetRenderTarget(targetTemp);
    glUseProgram(hProg->mProgramId);
    hProg->SetMatrix("mvp", mvp);
    hProg->SetTexture("srcTex", 0, src);
    hProg->SetFloat2("texSizeInv", texSizeInv);
    hProg->SetAttribute("posTexcoord", 0, 4, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    GLShaderProgram* vProg = prog->mVerticalProgram;
    SetRenderTarget(target);
    glUseProgram(vProg->mProgramId);
    vProg->SetMatrix("mvp", mvp);
    vProg->SetTexture("srcTex", 0, targetTemp->mTexture);
    vProg->SetFloat2("texSizeInv", texSizeInv);
    vProg->SetAttribute("posTexcoord", 0, 4, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLRenderer::Mask(GLRenderTarget* target, GLTexture* src, float* mvp, GLTexture* colorMask, GLTexture* alphaMask, float alpha)
{
    SetRenderTarget(target);

    float w = (float)src->GetWidth();
    float h = (float)src->GetHeight();
    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
           w, 0.0f, 1.0f, 0.0f,
        0.0f,    h, 0.0f, 1.0f,
           w,    h, 1.0f, 1.0f
    };

    glUseProgram(mMaskProgram->mProgramId);
    mMaskProgram->SetTexture("srcTex", 0, src);
    mMaskProgram->SetTexture("colorMaskTex", 1, colorMask ? colorMask : mDefaultTexture);
    mMaskProgram->SetTexture("alphaMaskTex", 2, alphaMask ? alphaMask : mDefaultTexture);
    mMaskProgram->SetFloat("alpha", alpha);
    mMaskProgram->SetMatrix("mvp", mvp);
    mMaskProgram->SetAttribute("posTexcoord", 0, 4, vertices);
    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


}

void GLRenderer::DrawMesh(GLMesh* mesh, GLShaderProgram* program, GLShaderState* state)
{
    if (!program)
    {
        return;
    }

    UseProgram(program);
    state->Apply(program);
    mesh->Apply(program);
    glDrawElements(GL_TRIANGLES, mesh->mIndexCount, GL_UNSIGNED_INT, mesh->mIndices);
}

void GLRenderer::DrawShape(GLShape* shape, float* mvp)
{
    if (!shape)
    {
        return;
    }

    GLMesh* mesh = shape->GetMesh();
    if (!mesh)
    {
        return;
    }

    GLShaderState state;
    state.SetValue("mvp", GLUniformValue(mvp));
    state.SetValue("color", GLUniformValue(shape->mColor[0], shape->mColor[1], shape->mColor[2], shape->mColor[3]));

    UseProgram(mShapeProgram);
    state.Apply(mShapeProgram);
    mesh->Apply(mShapeProgram);
    glDrawElements(GL_TRIANGLES, mesh->mIndexCount, GL_UNSIGNED_INT, mesh->mIndices);
}

GLMesh* GLRenderer::CreateCube(float center[3], float size[3])
{
    float sx = size[0] * 0.5f;
    float sy = size[1] * 0.5f;
    float sz = size[2] * 0.5f;

    float vs[][3] = {
        {center[0]-sx, center[1]-sy, center[2]-sz},
        {center[0]+sx, center[1]-sy, center[2]-sz},
        {center[0]+sx, center[1]+sy, center[2]-sz},
        {center[0]-sx, center[1]+sy, center[2]-sz},
        {center[0]-sx, center[1]-sy, center[2]+sz},
        {center[0]+sx, center[1]-sy, center[2]+sz},
        {center[0]+sx, center[1]+sy, center[2]+sz},
        {center[0]-sx, center[1]+sy, center[2]+sz}
    };


    GLMesh* mesh = new GLMesh(24, 36);
    GLAttribute* posAttr = mesh->AddAttribute("position", 3);
    GLAttribute* texAttr = mesh->AddAttribute("texcoord", 2);
    GLAttribute* normalAttr = mesh->AddAttribute("normal", 3);

    int i = 0;
    unsigned int vi = 0;
    unsigned int base = 0;
    //x- 3 0 4 7
    base = vi;
    texAttr->SetValue(vi, 0, 0); normalAttr->SetValue(vi, -1, 0, 0);
    posAttr->SetValue(vi++, vs[3][0], vs[3][1], vs[3][2]);
    texAttr->SetValue(vi, 1, 0); normalAttr->SetValue(vi, -1, 0, 0);
    posAttr->SetValue(vi++, vs[0][0], vs[0][1], vs[0][2]);
    texAttr->SetValue(vi, 1, 1); normalAttr->SetValue(vi, -1, 0, 0);
    posAttr->SetValue(vi++, vs[4][0], vs[4][1], vs[4][2]);
    texAttr->SetValue(vi, 0, 1); normalAttr->SetValue(vi, -1, 0, 0);
    posAttr->SetValue(vi++, vs[7][0], vs[7][1], vs[7][2]);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 1);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 3);
    //x+ 1 2 6 5
    base = vi;
    texAttr->SetValue(vi, 0, 0); normalAttr->SetValue(vi, 1, 0, 0);
    posAttr->SetValue(vi++, vs[1][0], vs[1][1], vs[1][2]);
    texAttr->SetValue(vi, 1, 0); normalAttr->SetValue(vi, 1, 0, 0);
    posAttr->SetValue(vi++, vs[2][0], vs[2][1], vs[2][2]);
    texAttr->SetValue(vi, 1, 1); normalAttr->SetValue(vi, 1, 0, 0);
    posAttr->SetValue(vi++, vs[6][0], vs[6][1], vs[6][2]);
    texAttr->SetValue(vi, 0, 1); normalAttr->SetValue(vi, 1, 0, 0);
    posAttr->SetValue(vi++, vs[5][0], vs[5][1], vs[5][2]);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 1);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 3);
    //y- 0 1 5 4
    base = vi;
    texAttr->SetValue(vi, 0, 0); normalAttr->SetValue(vi, 0, -1, 0);
    posAttr->SetValue(vi++, vs[0][0], vs[0][1], vs[0][2]);
    texAttr->SetValue(vi, 1, 0); normalAttr->SetValue(vi, 0, -1, 0);
    posAttr->SetValue(vi++, vs[1][0], vs[1][1], vs[1][2]);
    texAttr->SetValue(vi, 1, 1); normalAttr->SetValue(vi, 0, -1, 0);
    posAttr->SetValue(vi++, vs[5][0], vs[5][1], vs[5][2]);
    texAttr->SetValue(vi, 0, 1); normalAttr->SetValue(vi, 0, -1, 0);
    posAttr->SetValue(vi++, vs[4][0], vs[4][1], vs[4][2]);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 1);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 3);
    //y+ 2 3 7 6
    base = vi;
    texAttr->SetValue(vi, 0, 0); normalAttr->SetValue(vi, 0, 1, 0);
    posAttr->SetValue(vi++, vs[2][0], vs[2][1], vs[2][2]);
    texAttr->SetValue(vi, 1, 0); normalAttr->SetValue(vi, 0, 1, 0);
    posAttr->SetValue(vi++, vs[3][0], vs[3][1], vs[3][2]);
    texAttr->SetValue(vi, 1, 1); normalAttr->SetValue(vi, 0, 1, 0);
    posAttr->SetValue(vi++, vs[7][0], vs[7][1], vs[7][2]);
    texAttr->SetValue(vi, 0, 1); normalAttr->SetValue(vi, 0, 1, 0);
    posAttr->SetValue(vi++, vs[6][0], vs[6][1], vs[6][2]);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 1);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 3);
    //z- 0 3 2 1
    base = vi;
    texAttr->SetValue(vi, 0, 0); normalAttr->SetValue(vi, 0, 0, -1);
    posAttr->SetValue(vi++, vs[0][0], vs[0][1], vs[0][2]);
    texAttr->SetValue(vi, 1, 0); normalAttr->SetValue(vi, 0, 0, -1);
    posAttr->SetValue(vi++, vs[3][0], vs[3][1], vs[3][2]);
    texAttr->SetValue(vi, 1, 1); normalAttr->SetValue(vi, 0, 0, -1);
    posAttr->SetValue(vi++, vs[2][0], vs[2][1], vs[2][2]);
    texAttr->SetValue(vi, 0, 1); normalAttr->SetValue(vi, 0, 0, -1);
    posAttr->SetValue(vi++, vs[1][0], vs[1][1], vs[1][2]);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 1);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 3);
    //z+ 4 5 6 7
    base = vi;
    texAttr->SetValue(vi, 0, 0); normalAttr->SetValue(vi, 0, 0, 1);
    posAttr->SetValue(vi++, vs[4][0], vs[4][1], vs[4][2]);
    texAttr->SetValue(vi, 1, 0); normalAttr->SetValue(vi, 0, 0, 1);
    posAttr->SetValue(vi++, vs[5][0], vs[5][1], vs[5][2]);
    texAttr->SetValue(vi, 1, 1); normalAttr->SetValue(vi, 0, 0, 1);
    posAttr->SetValue(vi++, vs[6][0], vs[6][1], vs[6][2]);
    texAttr->SetValue(vi, 0, 1); normalAttr->SetValue(vi, 0, 0, 1);
    posAttr->SetValue(vi++, vs[7][0], vs[7][1], vs[7][2]);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 1);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 0);
    mesh->SetIndexValue(i++, base + 2);
    mesh->SetIndexValue(i++, base + 3);
    return mesh;
}
