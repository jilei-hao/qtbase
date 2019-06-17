/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Gui module
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QRHIGLES2_P_H
#define QRHIGLES2_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qrhigles2_p.h"
#include "qrhi_p_p.h"
#include "qshaderdescription_p.h"
#include <qopengl.h>
#include <QSurface>

QT_BEGIN_NAMESPACE

class QOpenGLExtensions;

struct QGles2Buffer : public QRhiBuffer
{
    QGles2Buffer(QRhiImplementation *rhi, Type type, UsageFlags usage, int size);
    ~QGles2Buffer();
    void release() override;
    bool build() override;

    GLuint buffer = 0;
    GLenum target;
    QByteArray ubuf;
    friend class QRhiGles2;
};

struct QGles2RenderBuffer : public QRhiRenderBuffer
{
    QGles2RenderBuffer(QRhiImplementation *rhi, Type type, const QSize &pixelSize,
                       int sampleCount, QRhiRenderBuffer::Flags flags);
    ~QGles2RenderBuffer();
    void release() override;
    bool build() override;
    QRhiTexture::Format backingFormat() const override;

    GLuint renderbuffer = 0;
    GLuint stencilRenderbuffer = 0; // when packed depth-stencil not supported
    int samples;
    friend class QRhiGles2;
};

struct QGles2SamplerData
{
    GLenum glminfilter = 0;
    GLenum glmagfilter = 0;
    GLenum glwraps = 0;
    GLenum glwrapt = 0;
    GLenum glwrapr = 0;
    GLenum gltexcomparefunc = 0;
};

inline bool operator==(const QGles2SamplerData &a, const QGles2SamplerData &b)
{
    return a.glminfilter == b.glminfilter
            && a.glmagfilter == b.glmagfilter
            && a.glwraps == b.glwraps
            && a.glwrapt == b.glwrapt
            && a.glwrapr == b.glwrapr
            && a.gltexcomparefunc == b.gltexcomparefunc;
}

inline bool operator!=(const QGles2SamplerData &a, const QGles2SamplerData &b)
{
    return !(a == b);
}

struct QGles2Texture : public QRhiTexture
{
    QGles2Texture(QRhiImplementation *rhi, Format format, const QSize &pixelSize,
                  int sampleCount, Flags flags);
    ~QGles2Texture();
    void release() override;
    bool build() override;
    bool buildFrom(const QRhiNativeHandles *src) override;
    const QRhiNativeHandles *nativeHandles() override;

    bool prepareBuild(QSize *adjustedSize = nullptr);

    GLuint texture = 0;
    bool owns = true;
    GLenum target;
    GLenum glintformat;
    GLenum glformat;
    GLenum gltype;
    QGles2SamplerData samplerState;
    bool specified = false;
    int mipLevelCount = 0;
    QRhiGles2TextureNativeHandles nativeHandlesStruct;

    uint generation = 0;
    friend class QRhiGles2;
};

struct QGles2Sampler : public QRhiSampler
{
    QGles2Sampler(QRhiImplementation *rhi, Filter magFilter, Filter minFilter, Filter mipmapMode,
                  AddressMode u, AddressMode v);
    ~QGles2Sampler();
    void release() override;
    bool build() override;

    QGles2SamplerData d;
    uint generation = 0;
    friend class QRhiGles2;
};

struct QGles2RenderPassDescriptor : public QRhiRenderPassDescriptor
{
    QGles2RenderPassDescriptor(QRhiImplementation *rhi);
    ~QGles2RenderPassDescriptor();
    void release() override;
};

struct QGles2RenderTargetData
{
    QGles2RenderTargetData(QRhiImplementation *) { }

    QGles2RenderPassDescriptor *rp = nullptr;
    QSize pixelSize;
    float dpr = 1;
    int sampleCount = 1;
    int colorAttCount = 0;
    int dsAttCount = 0;
    bool srgbUpdateAndBlend = false;
};

struct QGles2ReferenceRenderTarget : public QRhiRenderTarget
{
    QGles2ReferenceRenderTarget(QRhiImplementation *rhi);
    ~QGles2ReferenceRenderTarget();
    void release() override;

    QSize pixelSize() const override;
    float devicePixelRatio() const override;
    int sampleCount() const override;

    QGles2RenderTargetData d;
};

struct QGles2TextureRenderTarget : public QRhiTextureRenderTarget
{
    QGles2TextureRenderTarget(QRhiImplementation *rhi, const QRhiTextureRenderTargetDescription &desc, Flags flags);
    ~QGles2TextureRenderTarget();
    void release() override;

    QSize pixelSize() const override;
    float devicePixelRatio() const override;
    int sampleCount() const override;

    QRhiRenderPassDescriptor *newCompatibleRenderPassDescriptor() override;
    bool build() override;

    QGles2RenderTargetData d;
    GLuint framebuffer = 0;
    friend class QRhiGles2;
};

struct QGles2ShaderResourceBindings : public QRhiShaderResourceBindings
{
    QGles2ShaderResourceBindings(QRhiImplementation *rhi);
    ~QGles2ShaderResourceBindings();
    void release() override;
    bool build() override;

    uint generation = 0;
    friend class QRhiGles2;
};

struct QGles2GraphicsPipeline : public QRhiGraphicsPipeline
{
    QGles2GraphicsPipeline(QRhiImplementation *rhi);
    ~QGles2GraphicsPipeline();
    void release() override;
    bool build() override;

    GLuint program = 0;
    GLenum drawMode = GL_TRIANGLES;
    QShaderDescription vsDesc;
    QShaderDescription fsDesc;
    bool canUseUniformBuffers = false;

    struct Uniform {
        QShaderDescription::VariableType type;
        int glslLocation;
        int binding;
        uint offset;
        int size;
    };
    QVector<Uniform> uniforms;

    struct Sampler {
        int glslLocation;
        int binding;
    };
    QVector<Sampler> samplers;

    uint generation = 0;
    friend class QRhiGles2;
};

Q_DECLARE_TYPEINFO(QGles2GraphicsPipeline::Uniform, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(QGles2GraphicsPipeline::Sampler, Q_MOVABLE_TYPE);

struct QGles2CommandBuffer : public QRhiCommandBuffer
{
    QGles2CommandBuffer(QRhiImplementation *rhi);
    ~QGles2CommandBuffer();
    void release() override;

    struct Command {
        enum Cmd {
            BeginFrame,
            EndFrame,
            Viewport,
            Scissor,
            BlendConstants,
            StencilRef,
            BindVertexBuffer,
            BindIndexBuffer,
            Draw,
            DrawIndexed,
            BindGraphicsPipeline,
            BindShaderResources,
            BindFramebuffer,
            Clear,
            BufferData,
            BufferSubData,
            CopyTex,
            ReadPixels,
            SubImage,
            CompressedImage,
            CompressedSubImage,
            BlitFromRenderbuffer,
            GenMip
        };
        Cmd cmd;

        static const int MAX_UBUF_BINDINGS = 32; // should be more than enough

        // QRhi*/QGles2* references should be kept at minimum (so no
        // QRhiTexture/Buffer/etc. pointers).
        union {
            struct {
                float x, y, w, h;
                float d0, d1;
            } viewport;
            struct {
                int x, y, w, h;
            } scissor;
            struct {
                float r, g, b, a;
            } blendConstants;
            struct {
                quint32 ref;
                QRhiGraphicsPipeline *ps;
            } stencilRef;
            struct {
                QRhiGraphicsPipeline *ps;
                GLuint buffer;
                quint32 offset;
                int binding;
            } bindVertexBuffer;
            struct {
                GLuint buffer;
                quint32 offset;
                GLenum type;
            } bindIndexBuffer;
            struct {
                QRhiGraphicsPipeline *ps;
                quint32 vertexCount;
                quint32 firstVertex;
            } draw;
            struct {
                QRhiGraphicsPipeline *ps;
                quint32 indexCount;
                quint32 firstIndex;
            } drawIndexed;
            struct {
                QRhiGraphicsPipeline *ps;
            } bindGraphicsPipeline;
            struct {
                QRhiGraphicsPipeline *ps;
                QRhiShaderResourceBindings *srb;
                int dynamicOffsetCount;
                uint dynamicOffsetPairs[MAX_UBUF_BINDINGS * 2]; // binding, offsetInConstants
            } bindShaderResources;
            struct {
                GLbitfield mask;
                float c[4];
                float d;
                quint32 s;
            } clear;
            struct {
                GLuint fbo;
                bool srgb;
                int colorAttCount;
            } bindFramebuffer;
            struct {
                GLenum target;
                GLuint buffer;
                int offset;
                int size;
                const void *data; // must come from retainData()
            } bufferSubData;
            struct {
                GLenum srcFaceTarget;
                GLuint srcTexture;
                int srcLevel;
                int srcX;
                int srcY;
                GLenum dstTarget;
                GLuint dstTexture;
                GLenum dstFaceTarget;
                int dstLevel;
                int dstX;
                int dstY;
                int w;
                int h;
            } copyTex;
            struct {
                QRhiReadbackResult *result;
                GLuint texture;
                int w;
                int h;
                QRhiTexture::Format format;
                GLenum readTarget;
                int level;
            } readPixels;
            struct {
                GLenum target;
                GLuint texture;
                GLenum faceTarget;
                int level;
                int dx;
                int dy;
                int w;
                int h;
                GLenum glformat;
                GLenum gltype;
                int rowStartAlign;
                const void *data; // must come from retainImage()
            } subImage;
            struct {
                GLenum target;
                GLuint texture;
                GLenum faceTarget;
                int level;
                GLenum glintformat;
                int w;
                int h;
                int size;
                const void *data; // must come from retainData()
            } compressedImage;
            struct {
                GLenum target;
                GLuint texture;
                GLenum faceTarget;
                int level;
                int dx;
                int dy;
                int w;
                int h;
                GLenum glintformat;
                int size;
                const void *data; // must come from retainData()
            } compressedSubImage;
            struct {
                GLuint renderbuffer;
                int w;
                int h;
                GLenum target;
                GLuint texture;
                int dstLevel;
            } blitFromRb;
            struct {
                GLenum target;
                GLuint texture;
            } genMip;
        } args;
    };

    QVector<Command> commands;
    QRhiRenderTarget *currentTarget;
    QRhiGraphicsPipeline *currentPipeline;
    uint currentPipelineGeneration;
    QRhiShaderResourceBindings *currentSrb;
    uint currentSrbGeneration;

    QVector<QByteArray> dataRetainPool;
    QVector<QImage> imageRetainPool;

    // relies heavily on implicit sharing (no copies of the actual data will be made)
    const void *retainData(const QByteArray &data) {
        dataRetainPool.append(data);
        return dataRetainPool.constLast().constData();
    }
    const void *retainImage(const QImage &image) {
        imageRetainPool.append(image);
        return imageRetainPool.constLast().constBits();
    }
    void resetCommands() {
        commands.clear();
        dataRetainPool.clear();
        imageRetainPool.clear();
    }
    void resetState() {
        resetCommands();
        currentTarget = nullptr;
        resetCachedState();
    }
    void resetCachedState() {
        currentPipeline = nullptr;
        currentPipelineGeneration = 0;
        currentSrb = nullptr;
        currentSrbGeneration = 0;
    }
};

Q_DECLARE_TYPEINFO(QGles2CommandBuffer::Command, Q_MOVABLE_TYPE);

struct QGles2SwapChain : public QRhiSwapChain
{
    QGles2SwapChain(QRhiImplementation *rhi);
    ~QGles2SwapChain();
    void release() override;

    QRhiCommandBuffer *currentFrameCommandBuffer() override;
    QRhiRenderTarget *currentFrameRenderTarget() override;

    QSize surfacePixelSize() override;

    QRhiRenderPassDescriptor *newCompatibleRenderPassDescriptor() override;
    bool buildOrResize() override;

    QSurface *surface = nullptr;
    QSize pixelSize;
    QGles2ReferenceRenderTarget rt;
    QGles2CommandBuffer cb;
    int frameCount = 0;
};

class QRhiGles2 : public QRhiImplementation
{
public:
    QRhiGles2(QRhiGles2InitParams *params, QRhiGles2NativeHandles *importDevice = nullptr);

    bool create(QRhi::Flags flags) override;
    void destroy() override;

    QRhiGraphicsPipeline *createGraphicsPipeline() override;
    QRhiShaderResourceBindings *createShaderResourceBindings() override;
    QRhiBuffer *createBuffer(QRhiBuffer::Type type,
                             QRhiBuffer::UsageFlags usage,
                             int size) override;
    QRhiRenderBuffer *createRenderBuffer(QRhiRenderBuffer::Type type,
                                         const QSize &pixelSize,
                                         int sampleCount,
                                         QRhiRenderBuffer::Flags flags) override;
    QRhiTexture *createTexture(QRhiTexture::Format format,
                               const QSize &pixelSize,
                               int sampleCount,
                               QRhiTexture::Flags flags) override;
    QRhiSampler *createSampler(QRhiSampler::Filter magFilter, QRhiSampler::Filter minFilter,
                               QRhiSampler::Filter mipmapMode,
                               QRhiSampler:: AddressMode u, QRhiSampler::AddressMode v) override;

    QRhiTextureRenderTarget *createTextureRenderTarget(const QRhiTextureRenderTargetDescription &desc,
                                                       QRhiTextureRenderTarget::Flags flags) override;

    QRhiSwapChain *createSwapChain() override;
    QRhi::FrameOpResult beginFrame(QRhiSwapChain *swapChain, QRhi::BeginFrameFlags flags) override;
    QRhi::FrameOpResult endFrame(QRhiSwapChain *swapChain, QRhi::EndFrameFlags flags) override;
    QRhi::FrameOpResult beginOffscreenFrame(QRhiCommandBuffer **cb) override;
    QRhi::FrameOpResult endOffscreenFrame() override;
    QRhi::FrameOpResult finish() override;

    void resourceUpdate(QRhiCommandBuffer *cb, QRhiResourceUpdateBatch *resourceUpdates) override;

    void beginPass(QRhiCommandBuffer *cb,
                   QRhiRenderTarget *rt,
                   const QColor &colorClearValue,
                   const QRhiDepthStencilClearValue &depthStencilClearValue,
                   QRhiResourceUpdateBatch *resourceUpdates) override;
    void endPass(QRhiCommandBuffer *cb, QRhiResourceUpdateBatch *resourceUpdates) override;

    void setGraphicsPipeline(QRhiCommandBuffer *cb,
                             QRhiGraphicsPipeline *ps) override;

    void setShaderResources(QRhiCommandBuffer *cb,
                            QRhiShaderResourceBindings *srb,
                            int dynamicOffsetCount,
                            const QRhiCommandBuffer::DynamicOffset *dynamicOffsets) override;

    void setVertexInput(QRhiCommandBuffer *cb,
                        int startBinding, int bindingCount, const QRhiCommandBuffer::VertexInput *bindings,
                        QRhiBuffer *indexBuf, quint32 indexOffset,
                        QRhiCommandBuffer::IndexFormat indexFormat) override;

    void setViewport(QRhiCommandBuffer *cb, const QRhiViewport &viewport) override;
    void setScissor(QRhiCommandBuffer *cb, const QRhiScissor &scissor) override;
    void setBlendConstants(QRhiCommandBuffer *cb, const QColor &c) override;
    void setStencilRef(QRhiCommandBuffer *cb, quint32 refValue) override;

    void draw(QRhiCommandBuffer *cb, quint32 vertexCount,
              quint32 instanceCount, quint32 firstVertex, quint32 firstInstance) override;

    void drawIndexed(QRhiCommandBuffer *cb, quint32 indexCount,
                     quint32 instanceCount, quint32 firstIndex,
                     qint32 vertexOffset, quint32 firstInstance) override;

    void debugMarkBegin(QRhiCommandBuffer *cb, const QByteArray &name) override;
    void debugMarkEnd(QRhiCommandBuffer *cb) override;
    void debugMarkMsg(QRhiCommandBuffer *cb, const QByteArray &msg) override;

    const QRhiNativeHandles *nativeHandles(QRhiCommandBuffer *cb) override;
    void beginExternal(QRhiCommandBuffer *cb) override;
    void endExternal(QRhiCommandBuffer *cb) override;

    QVector<int> supportedSampleCounts() const override;
    int ubufAlignment() const override;
    bool isYUpInFramebuffer() const override;
    bool isYUpInNDC() const override;
    bool isClipDepthZeroToOne() const override;
    QMatrix4x4 clipSpaceCorrMatrix() const override;
    bool isTextureFormatSupported(QRhiTexture::Format format, QRhiTexture::Flags flags) const override;
    bool isFeatureSupported(QRhi::Feature feature) const override;
    int resourceLimit(QRhi::ResourceLimit limit) const override;
    const QRhiNativeHandles *nativeHandles() override;
    void sendVMemStatsToProfiler() override;
    void makeThreadLocalNativeContextCurrent() override;

    bool ensureContext(QSurface *surface = nullptr) const;
    void executeDeferredReleases();
    QRhi::FrameOpResult flushCommandBuffer();
    void enqueueSubresUpload(QGles2Texture *texD, QGles2CommandBuffer *cbD,
                             int layer, int level, const QRhiTextureSubresourceUploadDescription &subresDesc);
    void enqueueResourceUpdates(QRhiCommandBuffer *cb, QRhiResourceUpdateBatch *resourceUpdates);
    void executeCommandBuffer(QRhiCommandBuffer *cb);
    void executeBindGraphicsPipeline(QRhiGraphicsPipeline *ps);
    void bindShaderResources(QRhiGraphicsPipeline *ps, QRhiShaderResourceBindings *srb,
                             const uint *dynOfsPairs, int dynOfsCount);
    QGles2RenderTargetData *enqueueBindFramebuffer(QRhiRenderTarget *rt, QGles2CommandBuffer *cbD,
                                                   bool *wantsColorClear = nullptr, bool *wantsDsClear = nullptr);
    int effectiveSampleCount(int sampleCount) const;

    QOpenGLContext *ctx = nullptr;
    bool importedContext = false;
    QSurfaceFormat requestedFormat;
    QSurface *fallbackSurface = nullptr;
    QWindow *maybeWindow = nullptr;
    mutable bool needsMakeCurrent = false;
    QOpenGLExtensions *f = nullptr;
    uint vao = 0;
    struct Caps {
        Caps()
            : ctxMajor(2),
              ctxMinor(0),
              maxTextureSize(2048),
              maxDrawBuffers(4),
              msaaRenderBuffer(false),
              npotTexture(true),
              npotTextureRepeat(true),
              gles(false),
              fixedIndexPrimitiveRestart(false),
              bgraExternalFormat(false),
              bgraInternalFormat(false),
              r8Format(false),
              r16Format(false),
              floatFormats(false),
              depthTexture(false),
              packedDepthStencil(false),
              srgbCapableDefaultFramebuffer(false),
              coreProfile(false),
              uniformBuffers(false),
              elementIndexUint(false)
        { }
        int ctxMajor;
        int ctxMinor;
        int maxTextureSize;
        int maxDrawBuffers;
        int maxSamples;
        // Multisample fb and blit are supported (GLES 3.0 or OpenGL 3.x). Not
        // the same as multisample textures!
        uint msaaRenderBuffer : 1;
        uint npotTexture : 1;
        uint npotTextureRepeat : 1;
        uint gles : 1;
        uint fixedIndexPrimitiveRestart : 1;
        uint bgraExternalFormat : 1;
        uint bgraInternalFormat : 1;
        uint r8Format : 1;
        uint r16Format : 1;
        uint floatFormats : 1;
        uint depthTexture : 1;
        uint packedDepthStencil : 1;
        uint srgbCapableDefaultFramebuffer : 1;
        uint coreProfile : 1;
        uint uniformBuffers : 1;
        uint elementIndexUint : 1;
    } caps;
    bool inFrame = false;
    bool inPass = false;
    QGles2SwapChain *currentSwapChain = nullptr;
    QVector<GLint> supportedCompressedFormats;
    mutable QVector<int> supportedSampleCountList;
    QRhiGles2NativeHandles nativeHandlesStruct;

    struct DeferredReleaseEntry {
        enum Type {
            Buffer,
            Pipeline,
            Texture,
            RenderBuffer,
            TextureRenderTarget
        };
        Type type;
        union {
            struct {
                GLuint buffer;
            } buffer;
            struct {
                GLuint program;
            } pipeline;
            struct {
                GLuint texture;
            } texture;
            struct {
                GLuint renderbuffer;
                GLuint renderbuffer2;
            } renderbuffer;
            struct {
                GLuint framebuffer;
            } textureRenderTarget;
        };
    };
    QVector<DeferredReleaseEntry> releaseQueue;

    struct OffscreenFrame {
        OffscreenFrame(QRhiImplementation *rhi) : cbWrapper(rhi) { }
        bool active = false;
        QGles2CommandBuffer cbWrapper;
    } ofr;
};

Q_DECLARE_TYPEINFO(QRhiGles2::DeferredReleaseEntry, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

#endif