#ifndef FQGLTYPES_H
#define FQGLTYPES_H

#include <memory>


template <typename T> struct FQGLDeclarePtrs {
    typedef std::shared_ptr< T > SharedPtr;
    typedef std::weak_ptr< T > Ptr;
};

#define FQGL_DECLARE_PTRS(type)  \
    class type;                  \
    typedef FQGLDeclarePtrs<class type>::SharedPtr  type##SharedPtr; \
    typedef FQGLDeclarePtrs<class type>::Ptr        type##Ptr;
    


#endif // FQGLTYPES_H
