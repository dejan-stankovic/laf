// LAF OS Library
// Copyright (C) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_COMMON_SYSTEM_H
#define OS_COMMON_SYSTEM_H
#pragma once

#ifdef _WIN32
  #include "os/win/native_dialogs.h"
#elif defined(__APPLE__)
  #include "os/osx/app.h"
  #include "os/osx/menus.h"
  #include "os/osx/native_dialogs.h"
#elif defined(LAF_OS_WITH_GTK)
  #include "os/gtk/native_dialogs.h"
#else
  #include "os/native_dialogs.h"
#endif

#include "ft/lib.h"
#include "os/common/freetype_font.h"
#include "os/common/sprite_sheet_font.h"
#include "os/event_queue.h"
#include "os/menus.h"
#include "os/system.h"

#include <memory>

namespace os {

#ifdef __APPLE__
Logger* getOsxLogger();
#endif

class CommonSystem : public System {
public:
  CommonSystem()
    : m_nativeDialogs(nullptr)
    , m_menus(nullptr) {
#ifdef _WIN32
    m_useWintabAPI = true;
#endif
  }

  ~CommonSystem() {
    delete m_nativeDialogs;
    delete m_menus;
  }

  void dispose() override {
    set_instance(nullptr);
    delete this;
  }

  void activateApp() override {
#if __APPLE__
    OSXApp::instance()->activateApp();
#endif
  }

  void finishLaunching() override {
#if __APPLE__
    // Start processing NSApplicationDelegate events. (E.g. after
    // calling this we'll receive application:openFiles: and we'll
    // generate DropFiles events.)  events
    OSXApp::instance()->finishLaunching();
#endif
  }

  void useWintabAPI(bool state) override {
#ifdef _WIN32
    m_useWintabAPI = state;
#endif
  }

#ifdef _WIN32
  bool useWintabAPI() const {
    return m_useWintabAPI;
  }
#endif

  Logger* logger() override {
#ifdef __APPLE__
    return getOsxLogger();
#else
    return nullptr;
#endif
  }

  Menus* menus() override {
#ifdef __APPLE__
    if (!m_menus)
      m_menus = new MenusOSX();
#endif
    return m_menus;
  }

  NativeDialogs* nativeDialogs() override {
#ifdef _WIN32
    if (!m_nativeDialogs)
      m_nativeDialogs = new NativeDialogsWin32();
#elif defined(__APPLE__)
    if (!m_nativeDialogs)
      m_nativeDialogs = new NativeDialogsOSX();
#elif defined(LAF_OS_WITH_GTK)
    if (!m_nativeDialogs)
      m_nativeDialogs = new NativeDialogsGTK();
#endif
    return m_nativeDialogs;
  }

  EventQueue* eventQueue() override {
    return EventQueue::instance();
  }

  Font* loadSpriteSheetFont(const char* filename, int scale) override {
    Surface* sheet = loadRgbaSurface(filename);
    Font* font = nullptr;
    if (sheet) {
      sheet->applyScale(scale);
      font = SpriteSheetFont::fromSurface(sheet);
    }
    return font;
  }

  Font* loadTrueTypeFont(const char* filename, int height) override {
    if (!m_ft)
      m_ft.reset(new ft::Lib());
    return load_free_type_font(*m_ft.get(), filename, height);
  }

  KeyModifiers keyModifiers() override {
    return
      (KeyModifiers)
      ((isKeyPressed(kKeyLShift) ||
        isKeyPressed(kKeyRShift) ? kKeyShiftModifier: 0) |
       (isKeyPressed(kKeyLControl) ||
        isKeyPressed(kKeyRControl) ? kKeyCtrlModifier: 0) |
       (isKeyPressed(kKeyAlt) ? kKeyAltModifier: 0) |
       (isKeyPressed(kKeyAltGr) ? (kKeyCtrlModifier | kKeyAltModifier): 0) |
       (isKeyPressed(kKeyCommand) ? kKeyCmdModifier: 0) |
       (isKeyPressed(kKeySpace) ? kKeySpaceModifier: 0) |
       (isKeyPressed(kKeyLWin) ||
        isKeyPressed(kKeyRWin) ? kKeyWinModifier: 0));
  }

private:
#ifdef _WIN32
  bool m_useWintabAPI;
#endif
  NativeDialogs* m_nativeDialogs;
  Menus* m_menus;
  std::unique_ptr<ft::Lib> m_ft;
};

} // namespace os

#endif
