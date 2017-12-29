#include <windows.h>

#include "folderwatcher.hpp"
#include "util.hpp"

FolderWatcher::FolderWatcher() : m_Watching(false), m_HierarchyWatcher(nullptr)
{
}

FolderWatcher::~FolderWatcher()
{
    EndWatch();
}

void FolderWatcher::SetTargetFolder(std::string path)
{
    m_TargetFolder = path;
}

void FolderWatcher::SetHierarchyWatcher(FolderHierarchyWatcher *heirarchyWatcher)
{
    m_HierarchyWatcher = heirarchyWatcher;
}

void FolderWatcher::BeginWatch()
{
    EndWatch();

    m_Watching = true;
    m_WatcherThread = std::thread(&FolderWatcher::FolderWatchLoop, this);
}

void FolderWatcher::EndWatch()
{
    if (!m_Watching)
    {
        return;
    }

    m_Watching = false;

    DWORD result = ::TerminateThread(m_WatcherThread.native_handle(), 0);
    m_WatcherThread.detach();

    return;
}

void FolderWatcher::FolderWatchLoop()
{
    m_DirectoryHandle = CreateFile(m_TargetFolder.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    while (m_Watching)
    {
        FILE_NOTIFY_INFORMATION notifyInfoString[1024];
        DWORD bytesReturned;

        BOOL status = ReadDirectoryChangesW(m_DirectoryHandle, (LPVOID)&notifyInfoString, sizeof(notifyInfoString), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, NULL, NULL);

        if (status)
        {
            bool processesAllChanges = false;

            FILE_NOTIFY_INFORMATION *notifyInfo = &notifyInfoString[0];

            while (!processesAllChanges)
            {
                WCHAR *filename = notifyInfo->FileName;
                DWORD filenameLength = notifyInfo->FileNameLength;

                std::wstring changedFilename(filename, filenameLength / 2);

                if (m_HierarchyWatcher != nullptr)
                {
                    std::string fullPathOfChangedFile = m_TargetFolder + "/" + std::string(changedFilename.begin(), changedFilename.end());

                    FolderModificationEvent modificationEvent;
                    modificationEvent.targetFile = std::string(changedFilename.begin(), changedFilename.end());
                    modificationEvent.fullPath = fullPathOfChangedFile;

                    m_HierarchyWatcher->NotifyModification(modificationEvent);
                }

                if (notifyInfo->NextEntryOffset == 0)
                {
                    processesAllChanges = true;
                    continue;
                }

                notifyInfo = notifyInfo + (notifyInfo->NextEntryOffset);
            }
        }
    }
}

FolderHierarchyWatcher::FolderHierarchyWatcher()
{
    m_HierarchyModified = false;
}

FolderHierarchyWatcher::~FolderHierarchyWatcher()
{
}

void FolderHierarchyWatcher::WatchFolderHierarchy(const std::string &path)
{
    DWORD fileType = GetFileAttributesA(path.c_str());

    if (fileType == INVALID_FILE_ATTRIBUTES)
    {
        throw new OSError("Hierarchy watcher started on invalid path.");
    }

    if (!(fileType & FILE_ATTRIBUTE_DIRECTORY))
    {
        throw new OSError("Hierarchy watcher started on non-folder path.");
    }

    m_TopFolder = path;

    std::vector<std::string> subdirectories;

    subdirectories.push_back(path);

    while (!subdirectories.empty())
    {
        std::string subdirectory = subdirectories.back();
        subdirectories.pop_back();

        std::shared_ptr<FolderWatcher> folderWatcher = std::make_shared<FolderWatcher>();
        folderWatcher->SetTargetFolder(subdirectory);
        folderWatcher->SetHierarchyWatcher(this);
        folderWatcher->BeginWatch();

        m_FolderWatchers.push_back(folderWatcher);

        WIN32_FIND_DATA findFileData;
        HANDLE findHandle;

        findHandle = FindFirstFile((subdirectory + "/*").c_str(), &findFileData);

        BOOL moreFiles = true;

        if (findHandle == INVALID_HANDLE_VALUE)
        {
            moreFiles = false;
        }

        while (moreFiles)
        {
            std::string filename = findFileData.cFileName;

            if (StringEndsWith(subdirectory, filename))
            {
                moreFiles = FindNextFile(findHandle, &findFileData);
                continue;
            }

            if (filename.compare(".") == 0 || filename.compare("..") == 0)
            {
                moreFiles = FindNextFile(findHandle, &findFileData);
                continue;
            }

            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                std::string subfolderName = subdirectory + "/" + findFileData.cFileName;
                subdirectories.push_back(subfolderName);
            }

            moreFiles = FindNextFile(findHandle, &findFileData);
        }

        GetLastError();
        FindClose(findHandle);
    }
}

bool FolderHierarchyWatcher::IsHierarchyModified()
{
    return m_HierarchyModified;
}

std::vector<FolderModificationEvent> FolderHierarchyWatcher::GetFolderModificationEvents()
{
    // Make sure the event container is unmodified while copying
    m_FolderModificationMutex.lock();

    std::vector<FolderModificationEvent> modificationEvents = m_FolderModificationEvents;
    m_FolderModificationEvents.clear();

    m_HierarchyModified = false;

    m_FolderModificationMutex.unlock();

    return modificationEvents;
}

void FolderHierarchyWatcher::NotifyModification(const FolderModificationEvent &folderModificationEvent)
{
    m_FolderModificationMutex.lock();
    m_FolderModificationEvents.push_back(folderModificationEvent);
    m_HierarchyModified = true;
    m_FolderModificationMutex.unlock();
}

const std::string &FolderHierarchyWatcher::GetWatchedFolder() const
{
    return m_TopFolder;
}
