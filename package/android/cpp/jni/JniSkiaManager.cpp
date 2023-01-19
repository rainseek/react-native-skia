#include "JniSkiaManager.h"

#include <android/log.h>
#include <jni.h>
#include <string>
#include <utility>

#include "JniSkiaDrawView.h"
#include <RNSkManager.h>

#include <GLES3/gl3.h>

namespace RNSkia {

namespace jsi = facebook::jsi;

// JNI binding
void JniSkiaManager::registerNatives() {
  registerHybrid({
      makeNativeMethod("initHybrid", JniSkiaManager::initHybrid),
      makeNativeMethod("initializeRuntime", JniSkiaManager::initializeRuntime),
      makeNativeMethod("invalidate", JniSkiaManager::invalidate),
      makeNativeMethod("MakeOffscreenSurface", JniSkiaManager::MakeOffscreenSurface),
  });
}

// JNI init
jni::local_ref<jni::HybridClass<JniSkiaManager>::jhybriddata>
JniSkiaManager::initHybrid(jni::alias_ref<jhybridobject> jThis, jlong jsContext,
                           JSCallInvokerHolder jsCallInvokerHolder,
                           JavaPlatformContext skiaContext) {

  // cast from JNI hybrid objects to C++ instances
  return makeCxxInstance(jThis, reinterpret_cast<jsi::Runtime *>(jsContext),
                         jsCallInvokerHolder->cthis()->getCallInvoker(),
                         skiaContext->cthis());
}

void JniSkiaManager::initializeRuntime() {
  // Create the cross platform skia manager
  _skManager =
      std::make_shared<RNSkManager>(_jsRuntime, _jsCallInvoker, _context);
}

struct ColorSettings {
    ColorSettings(sk_sp<SkColorSpace> colorSpace) {
        if (colorSpace == nullptr || colorSpace->isSRGB()) {
            colorType = kRGBA_8888_SkColorType;
            pixFormat = GL_RGBA8;
        } else {
            colorType = kRGBA_F16_SkColorType;
            pixFormat = GL_RGBA16F;
        }
    };
    SkColorType colorType;
    GrGLenum pixFormat;
};

void JniSkiaManager::MakeOffscreenSurface() {
    int width = 100;
    int height = 100;

    // setup interface
    auto interface = GrGLMakeNativeInterface();
    // setup context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 0);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    sk_sp<GrDirectContext> dContext = GrDirectContext::MakeGL(interface);
    //dContext->resetContext(kRenderTarget_GrGLBackendState | kMisc_GrGLBackendState);

    GLint buffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);

    GLint stencil;
    glGetIntegerv(GL_STENCIL_BITS, &stencil);

    GLint samples;
    glGetIntegerv(GL_SAMPLES, &samples);

    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = buffer;
    fbInfo.fFormat = 0x8058;

    GrBackendRenderTarget _skRenderTarget(width, height, samples, stencil, fbInfo);


    sk_sp<SkSurface> _skSurface = SkSurface::MakeFromBackendRenderTarget(
        dContext.get(), _skRenderTarget,
        kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, nullptr, nullptr);
  //return surface;
}

} // namespace RNSkia
