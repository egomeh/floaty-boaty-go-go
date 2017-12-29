#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>

struct FolderModificationEvent
{
public:
    std::string targetFile;
    std::string fullPath;
};

class FolderHierarchyWatcher;

class FolderWatcher
{
public:
    FolderWatcher();
    ~FolderWatcher();

    void SetTargetFolder(std::string path);
    void SetHierarchyWatcher(FolderHierarchyWatcher *heirarchyWatcher);

    void BeginWatch();
    void EndWatch();

private:
    void FolderWatchLoop();

    bool m_Watching;

    HANDLE m_DirectoryHandle;

    std::thread m_WatcherThread;
    std::string m_TargetFolder;

    FolderHierarchyWatcher *m_HierarchyWatcher;
};

class FolderHierarchyWatcher
{
public:
    FolderHierarchyWatcher();
    ~FolderHierarchyWatcher();

    void WatchFolderHierarchy(const std::string &path);

    bool IsHierarchyModified();

    std::vector<FolderModificationEvent> GetFolderModificationEvents();

    void NotifyModification(const FolderModificationEvent &folderModificationEvent);

    const std::string &GetWatchedFolder() const;

private:
    std::atomic<bool> m_HierarchyModified;
    std::mutex m_FolderModificationMutex;

    std::vector<FolderModificationEvent> m_FolderModificationEvents;
    std::vector<std::shared_ptr<FolderWatcher>> m_FolderWatchers;

    std::string m_TopFolder;
};

