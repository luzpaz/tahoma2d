

#include "toonz/toonzfolders.h"
#include "tsystem.h"
#include "tenv.h"
#include "tconvert.h"
#include "toonz/preferences.h"
#include <QStandardPaths>
#include <QDir>

using namespace TEnv;

//-------------------------------------------------------------------
namespace {
TFilePath getMyDocumentsPath() {
  QString documentsPath =
      QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0];
  return TFilePath(documentsPath);
}

// Desktop Path
TFilePath getDesktopPath() {
  QString desktopPath =
      QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)[0];
  return TFilePath(desktopPath);
}
}  // namespace
//-------------------------------------------------------------------

TFilePath ToonzFolder::getModulesDir() {
  return getProfileFolder() + "layouts";
}

TFilePathSet ToonzFolder::getProjectsFolders() {
  int location = Preferences::instance()->getProjectRoot();
  QString path = Preferences::instance()->getCustomProjectRoot();
  TFilePathSet fps;
  int projectPaths = Preferences::instance()->getProjectRoot();
  bool stuff       = projectPaths & 0x08;
  bool documents   = projectPaths & 0x04;
  bool desktop     = projectPaths & 0x02;
  bool custom      = projectPaths & 0x01;

  // make sure at least something is there
  if (!desktop && !custom && !documents) stuff = 1;
  TFilePathSet tempFps =
      getSystemVarPathSetValue(getSystemVarPrefix() + "PROJECTS");
  if (stuff) {
    for (TFilePath tempPath : tempFps) {
      if (TSystem::doesExistFileOrLevel(TFilePath(tempPath))) {
        fps.push_back(TFilePath(tempPath));
      }
    }
    if (tempFps.size() == 0)
      fps.push_back(TEnv::getStuffDir() +
                    TEnv::getSystemPathMap().at("PROJECTS"));
  }
  if (documents) {
    fps.push_back(getMyDocumentsPath() + "Tahoma2D");
    if (!TSystem::doesExistFileOrLevel(getMyDocumentsPath() + "Tahoma2D")) {
      TSystem::mkDir(getMyDocumentsPath() + "Tahoma2D");
    }
  }
  if (desktop) {
    fps.push_back(getDesktopPath() + "Tahoma2D");
    if (!TSystem::doesExistFileOrLevel(getDesktopPath() + "Tahoma2D")) {
      TSystem::mkDir(getDesktopPath() + "Tahoma2D");
    }
  }
  if (custom) {
    QStringList paths = path.split("**");
    for (QString tempPath : paths) {
      if (TSystem::doesExistFileOrLevel(TFilePath(tempPath))) {
        fps.push_back(TFilePath(tempPath));
      }
    }
  }
  return fps;
}

TFilePath ToonzFolder::getFirstProjectsFolder() {
  TFilePathSet fps = getProjectsFolders();
  if (fps.empty())
    return TFilePath();
  else
    return *fps.begin();
}

TFilePath ToonzFolder::getLibraryFolder() {
  TFilePath fp = getSystemVarPathValue(getSystemVarPrefix() + "LIBRARY");
  if (fp == TFilePath())
    fp = getStuffDir() + TEnv::getSystemPathMap().at("LIBRARY");
  return fp;
}

TFilePath ToonzFolder::getStudioPaletteFolder() {
  TFilePath fp = getSystemVarPathValue(getSystemVarPrefix() + "STUDIOPALETTE");
  if (fp == TFilePath())
    fp = getStuffDir() + TEnv::getSystemPathMap().at("STUDIOPALETTE");
  return fp;
}

TFilePath ToonzFolder::getFxPresetFolder() {
  TFilePath fp = getSystemVarPathValue(getSystemVarPrefix() + "FXPRESETS");
  if (fp == TFilePath())
    fp = getStuffDir() + TEnv::getSystemPathMap().at("FXPRESETS");
  return fp;
}

TFilePath ToonzFolder::getPluginsFolder() {
  TFilePath fp = getStuffDir() + TFilePath("plugins");
  return fp;
}

TFilePath ToonzFolder::getCacheRootFolder() {
  static enum STATE { FIRSTTIME, OK, NG } state = FIRSTTIME;
  QString cacheDir =
      QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
  if (state == FIRSTTIME) {
    if (QDir(cacheDir).mkpath("."))
      state = OK;
    else
      state = NG;
  }
  return (state == OK) ? TFilePath(cacheDir) : TFilePath();
}

TFilePath ToonzFolder::getProfileFolder() {
  TFilePath fp = getSystemVarPathValue(getSystemVarPrefix() + "PROFILES");
  if (fp == TFilePath())
    fp = getStuffDir() + TEnv::getSystemPathMap().at("PROFILES");
  return fp;
}

TFilePath ToonzFolder::getReslistPath(bool forCleanup) {
  return getConfigDir() + (forCleanup ? "cleanupreslist.txt" : "reslist.txt");
}

TFilePath ToonzFolder::getMyReslistPath(bool forCleanup) {
  return getMyModuleDir() + (forCleanup ? "cleanupreslist.txt" : "reslist.txt");
}

TFilePath ToonzFolder::getMyFavoritesFolder() {
  return getMyModuleDir() + "favorites";
}

TFilePath ToonzFolder::getTemplateModuleDir() {
  return getModulesDir() + "settings";
}

TFilePath ToonzFolder::getMyModuleDir() {
  return getProfileFolder() +
         TFilePath(L"users/" + TSystem::getUserName().toStdWString());
}

TFilePath ToonzFolder::getModuleFile(TFilePath filename) {
  TFilePath fp = getMyModuleDir() + filename;
  if (TFileStatus(fp).doesExist()) return fp;
  fp = getTemplateModuleDir() + filename;
  return fp;
}

TFilePath ToonzFolder::getModuleFile(std::string fn) {
  return ToonzFolder::getModuleFile(TFilePath(fn));
}

// turtle
TFilePath ToonzFolder::getRoomsDir() {
  return getProfileFolder() + TFilePath("layouts/rooms");
}

TFilePath ToonzFolder::getTemplateRoomsDir() {
  return getRoomsDir() +
         Preferences::instance()->getCurrentRoomChoice().toStdWString();
}

TFilePath ToonzFolder::getMyRoomsDir() {
  return getMyModuleDir() +
         TFilePath(
             L"layouts/" +
             Preferences::instance()->getCurrentRoomChoice().toStdWString());
}

TFilePath ToonzFolder::getRoomsFile(TFilePath filename) {
  TFilePath fp = getMyRoomsDir() + filename;
  if (TFileStatus(fp).doesExist()) return fp;
  fp = getTemplateRoomsDir() + filename;
  return fp;
}

TFilePath ToonzFolder::getRoomsFile(std::string fn) {
  return ToonzFolder::getRoomsFile(TFilePath(fn));
}

TFilePath ToonzFolder::getMyPalettesDir() {
  return getMyModuleDir() + "palettes";
}

//===================================================================

FolderListenerManager::FolderListenerManager() {}

//-------------------------------------------------------------------

FolderListenerManager::~FolderListenerManager() {}

//-------------------------------------------------------------------

FolderListenerManager *FolderListenerManager::instance() {
  static FolderListenerManager _instance;
  return &_instance;
}

//-------------------------------------------------------------------

void FolderListenerManager::notifyFolderChanged(const TFilePath &path) {
  for (std::set<Listener *>::iterator i = m_listeners.begin();
       i != m_listeners.end(); ++i)
    (*i)->onFolderChanged(path);
}

//-------------------------------------------------------------------

void FolderListenerManager::addListener(Listener *listener) {
  m_listeners.insert(listener);
}

//-------------------------------------------------------------------

void FolderListenerManager::removeListener(Listener *listener) {
  m_listeners.erase(listener);
}
