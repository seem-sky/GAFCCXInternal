#pragma once

#define NS_GAF_BEGIN    namespace gaf {
#define NS_GAF_END      }
#define USING_NS_GAF    using namespace gaf


#define GAF_SAFE_RELEASE_MAP(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
    i->second->release(); \
} __instance.clear()

#define GAF_SAFE_RELEASE_ARRAY(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
    (*i)->release(); \
} __instance.clear()

#define GAF_SAFE_RELEASE_ARRAY_WITH_NULL_CHECK(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
if (*i) \
    (*i)->release(); \
} __instance.clear()

#define GAF_RELEASE_ARRAY(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
    delete *i; \
} __instance.clear()

#define GAF_RELEASE_MAP(__type, __instance)\
for (__type::iterator i = __instance.begin(), e = __instance.end(); i != e; ++i) {\
    delete i->second; \
} __instance.clear()

#define gaf_fwd_this(__class__)\
class __class__;\
using __class__##Ptr = std::shared_ptr<__class__>;\
using __class__##ConstPtr = std::shared_ptr<const __class__>;\
using __class__##SPtr = std::shared_ptr<__class__>;\
using __class__##ConstSPtr = std::shared_ptr<const __class__>;\
using __class__##UPtr = std::unique_ptr<__class__>;\
using __class__##ConstUPtr = std::unique_ptr<const __class__>;\
using __class__##WPtr = std::weak_ptr<__class__>;\
using __class__##ConstWPtr = std::weak_ptr<const __class__>


#ifndef GAF_ENABLE_NEW_UNIFORM_SETTER
// Fast uniform setter is available since v3.2
#define GAF_ENABLE_NEW_UNIFORM_SETTER COCOS2D_VERSION >= 0x00030200
#endif

// Disable this option and call GAFShaderManager::Initialize by you own
// anywhere you want. These could be useful when you pass GAF loading into background thread
// whereas GAFShaderManager::Initialize is allowed to be called only on OpenGL's thread

#define GAF_ENABLE_SHADER_MANAGER_AUTOMATIC_INITIALIZATION 0

#ifndef GAF_ENABLE_SHADER_MANAGER_AUTOMATIC_INITIALIZATION
#define     GAF_ENABLE_SHADER_MANAGER_AUTOMATIC_INITIALIZATION 1
#endif

#define CHECK_CTX_IDENTITY 1

#if !defined(NO_OF_MAKE_UNIQUE) && (defined(_MSC_VER) && _MSC_VER < 1800) || (!defined(_MSC_VER) && __cplusplus <= 201103L)

namespace std
{
    template<class T> struct _Unique_if
    {
        typedef unique_ptr<T> _Single_object;
    };

    template<class T> struct _Unique_if<T[]>
    {
        typedef unique_ptr<T[]> _Unknown_bound;
    };

    template<class T, size_t N> struct _Unique_if<T[N]>
    {
        typedef void _Known_bound;
    };

    template<class T, class... Args>
    typename _Unique_if<T>::_Single_object
    make_unique(Args&&... args)
    {
        return unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template<class T>
    typename _Unique_if<T>::_Unknown_bound
    make_unique(size_t n)
    {
        typedef typename remove_extent<T>::type U;
        return unique_ptr<T>(new U[n]());
    }

    template<class T, class... Args>
    typename _Unique_if<T>::_Known_bound
    make_unique(Args&&...) = delete;
}

#endif
