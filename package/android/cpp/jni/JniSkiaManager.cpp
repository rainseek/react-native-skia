#include "JniSkiaManager.h"

#include <android/log.h>
#include <jni.h>
#include <string>
#include <utility>

#include "JniSkiaDrawView.h"
#include "JsiSkSurface.h"
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
      makeNativeMethod("MakeOffscreenSurface",
                       JniSkiaManager::MakeOffscreenSurface),
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

  auto renderer = std::make_shared<SkiaOpenGLRenderer>();
  bool init = renderer->ensureInitialised();
  assert(init);
  renderer->ensureSkiaSurface(width, height);
  auto surface = renderer->getSurface();
  assert(surface != nullptr);
  auto jsiSurface = jsi::Object::createFromHostObject(
      *_jsRuntime,
      std::make_shared<JsiSkSurface>(_context, std::move(surface)));
  _jsRuntime->global().setProperty(*_jsRuntime, "Surface", jsiSurface);
}

} // namespace RNSkia
