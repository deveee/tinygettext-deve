// tinygettext - A gettext replacement that works directly on .po files
// Copyright (c) 2009 Ingo Ruhnke <grumbel@gmail.com>
// Copyright (c) 2015 Dawid Gan <deveee@gmail.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifdef ANDROID

#include "android_file_system.hpp"

#include <sys/types.h>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#ifdef SDL2_ENABLED

#include <jni.h>
#include <android/asset_manager_jni.h>

#ifndef DECLSPEC
#  if defined(__GNUC__) && __GNUC__ >= 4
#    define DECLSPEC __attribute__ ((visibility("default")))
#  elif defined(__GNUC__) && __GNUC__ >= 2
#    define DECLSPEC __declspec(dllexport)
#  else
#    define DECLSPEC
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern DECLSPEC void * SDL_AndroidGetJNIEnv();
extern DECLSPEC void * SDL_AndroidGetActivity();
#ifdef __cplusplus
}
#endif

#endif

namespace tinygettext {

AndroidFileSystem::AndroidFileSystem()
{
    m_assets_dir = false;

    #ifdef SDL2_ENABLED
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject activity = (jobject)SDL_AndroidGetActivity();

    jclass clazz(env->GetObjectClass(activity));

    jmethodID method_id = env->GetStaticMethodID(clazz,
            "getContext","()Landroid/content/Context;");

    jobject context = env->CallStaticObjectMethod(clazz, method_id);

    method_id = env->GetMethodID(env->GetObjectClass(context),
            "getAssets", "()Landroid/content/res/AssetManager;");

    jobject assetManager = env->CallObjectMethod(context, method_id);

    m_asset_mgr = AAssetManager_fromJava(env, assetManager);

    // clean up the local references.
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(context);
    env->DeleteLocalRef(assetManager);
    #endif
}

std::vector<std::string>
AndroidFileSystem::open_directory(const std::string& pathname)
{
    std::vector<std::string> files;
    DIR* dir = opendir(pathname.c_str());

    if (dir)
    {
        struct dirent* dp;

        while ((dp = readdir(dir)) != 0)
        {
            files.push_back(dp->d_name);
        }

        closedir(dir);
    }
    else
    {
        #ifdef SDL2_ENABLED
        // Try to read directory from assets...
        AAssetDir* assetDir = AAssetManager_openDir(m_asset_mgr, pathname.c_str());

        bool finished = false;
        while (!finished)
        {
            const char* filename = AAssetDir_getNextFileName(assetDir);

            if (filename)
            {
                files.push_back(filename);
            }
            else
            {
                finished = true;
            }
        }

        if (files.size() > 0)
        {
            m_assets_dir = true;
        }

        AAssetDir_close(assetDir);
        #endif
    }

    return files;
}

std::auto_ptr<std::istream>
AndroidFileSystem::open_file(const std::string& filename)
{
    if (m_assets_dir == false)
    {
        return std::auto_ptr<std::istream>(new std::ifstream(filename.c_str()));
    }
    else
    {
        #ifdef SDL2_ENABLED
        AAsset* file = AAssetManager_open(m_asset_mgr, filename.c_str(), 0);

        std::auto_ptr<std::stringstream> ss;
        ss = std::auto_ptr<std::stringstream>(new std::stringstream());

        char buf[BUFSIZ];
        int nb_read = 0;

        bool finished = false;
        while (!finished)
        {
            nb_read = AAsset_read(file, buf, BUFSIZ);

            if (nb_read > 0)
            {
                *ss << std::string(buf).substr(0, nb_read);
            }
            else
            {
                finished = true;
            }
        }

        AAsset_close(file);

        return std::auto_ptr<std::istream>(ss);
        #endif
    }
}

} // namespace tinygettext

#endif

/* EOF */
