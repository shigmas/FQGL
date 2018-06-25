#ifndef FQGLTYPES_H
#define FQGLTYPES_H

#include <memory>


template <typename T> struct FQGLDeclarePtrs {
    typedef std::shared_ptr< T > SharedPtr;
    typedef std::weak_ptr< T > Ptr;
    typedef std::unique_ptr< T > UniquePtr;
};

#define FQGL_DECLARE_PTRS(type)  \
    class type;                  \
    typedef FQGLDeclarePtrs<class type>::SharedPtr  type##SharedPtr; \
    typedef FQGLDeclarePtrs<class type>::Ptr        type##Ptr;       \
    typedef FQGLDeclarePtrs<class type>::UniquePtr  type##UniquePtr;

enum FQGLPrimViewType {
    // It's still part of the scene, but drawn in the screen
    FQGLScreenViewType,
    FQGLSceneViewType
};

#define FQGL_NUM_FRAMEBUFFERS 3

enum FQGLFramebufferType {
    FQGLTextureFramebufferType = 0,
    FQGLPickingFramebufferType = 1,
    FQGLDefaultFramebufferType = 2,
};

#endif // FQGLTYPES_H
