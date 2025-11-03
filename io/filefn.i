typedef unsigned long long size_t;
typedef unsigned long long HANDLE;
typedef unsigned long DWORD;
typedef int BOOL;
typedef long long LONGLONG;

struct LARGE_INTEGER
{
    LONGLONG QuadPart;
};



// FILE_HANDLE struct
struct  FILE_HANDLE
{
    HANDLE handle;
    size_t size;
    size_t pos;
    int eof;
    int error;
    int mode;   // bitmask: 1=read,2=write,4=append,8=update
    int in_use; // pool flag
};
extern HANDLE CreateFileA(char *lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void *lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

extern int ReadFile(HANDLE hFile);
extern BOOL WriteFile(HANDLE hFile, char *lpBuffer, DWORD nNumberOfBytesToWrite, DWORD *lpNumberOfBytesWritten, void *lpOverlapped);
extern BOOL CloseHandle(HANDLE hObject);
extern BOOL GetFileSizeEx(HANDLE hFile, struct LARGE_INTEGER *lpFileSize);
extern BOOL SetFilePointerEx(HANDLE hFile, struct LARGE_INTEGER liDistanceToMove, struct LARGE_INTEGER *lpNewFilePointer, DWORD dwMoveMethod);
extern BOOL DeleteFileA(char *lpFileName);
extern BOOL CreateDirectoryA(char *lpPathName, void *lpSecurityAttributes);
extern DWORD GetLastError();


// Static pool configuration

struct FILE_HANDLE file_handle_pool[256];

// Allocate a FILE_HANDLE from the pool; returns 0 if none available
static struct FILE_HANDLE *allocate_file_handle_from_pool()
{
    int i;
    for (i = 0; i < 256; ++i)
    {
        if (!file_handle_pool[i].in_use)
        {
            file_handle_pool[i].in_use = 1;
            file_handle_pool[i].handle = 0;
            file_handle_pool[i].size = 0;
            file_handle_pool[i].pos = 0;
            file_handle_pool[i].eof = 0;
            file_handle_pool[i].error = 0;
            file_handle_pool[i].mode = 0;
            return &file_handle_pool[i];
        }
    }
    return 0;
}

// Release a FILE_HANDLE back to the pool
static void free_file_handle_to_pool(struct FILE_HANDLE *f)
{
    if (!f)
        return;
    f->in_use = 0;
    f->handle = 0;
    f->size = 0;
    f->pos = 0;
    f->eof = 0;
    f->error = 0;
    f->mode = 0;
}

// fopen implementation
struct FILE_HANDLE *fopen(char *path, char *mode)
{
    if (!path || !mode)
        return 0;
    DWORD access = 0;
    DWORD creation = 3;
    int read = 0, write = 0, append = 0, update = 0;

    char m0 = mode[0];
    if (m0 == 'r')
    {
        read = 1;
        creation = 3;
    }
    else if (m0 == 'w')
    {
        write = 1;
        creation = 2;
    }
    else if (m0 == 'a')
    {
        write = 1;
        append = 1;
        creation = 4;
    }

    // check for '+'
    {
        int i = 0;
        while (mode[i])
        {
            if (mode[i] == '+')
            {
                update = 1;
                read = 1;
                write = 1;
            }
            i++;
        }
    }

    if (read)
        access |= 0x80000000;
    if (write)
        access |= 0x40000000;

    DWORD share = 0x00000001 | 0x00000002;
    DWORD flags = 0x80;

    HANDLE h = CreateFileA(path, access, share, 0, creation, flags, 0);
    if (h == (HANDLE)(-1) || h == 0)
        return 0;

    struct FILE_HANDLE *f = allocate_file_handle_from_pool();
    if (!f)
    {
        CloseHandle(h);
        return 0;
    }

    f->handle = h;
    f->pos = 0;
    f->eof = 0;
    f->error = 0;
    f->mode = 0;
    if (read)
        f->mode = f->mode | 1;
    if (write)
        f->mode = f->mode | 2;
    if (append)
        f->mode = f->mode | 4;
    if (update)
        f->mode = f->mode | 8;

    struct LARGE_INTEGER li;
    if (GetFileSizeEx(h, &li))
    {
        if (li.QuadPart < 0)
            f->size = 0;
        else
            f->size = (size_t)li.QuadPart;
    }
    else
    {
        f->size = 0;
    }

    if (append)
    {
        struct LARGE_INTEGER move;
        move.QuadPart = 0;
        SetFilePointerEx(h, move, 0, 2);
        f->pos = f->size;
    }
    else
    {
        struct LARGE_INTEGER move;
        move.QuadPart = 0;
        SetFilePointerEx(h, move, 0, 0);
        f->pos = 0;
    }

    return f;
}

// fclose
int fclose(struct FILE_HANDLE *f)
{
    if (!f)
        return -1;
    if (f->handle)
        CloseHandle(f->handle);
    free_file_handle_to_pool(f);
    return 0;
}

// fread
size_t fread(void *ptr, size_t size, size_t nmemb, struct FILE_HANDLE *f)
{
    if (!f || !ptr || size == 0 || nmemb == 0)
        return 0;
    if (!(f->mode & 1) && !(f->mode & 8))
    {
        f->error = 1;
        return 0;
    }

    size_t total = size * nmemb;
    if (total == 0)
        return 0;

    DWORD toRead = (DWORD)total;
    DWORD actuallyRead = 0;
    BOOL ok = ReadFile(f->handle, (char *)ptr, toRead, &actuallyRead, 0);
    if (!ok)
    {
        f->error = 1;
        return 0;
    }

    f->pos += actuallyRead;
    if (actuallyRead < toRead)
        f->eof = 1;
    return (size_t)(actuallyRead / (DWORD)size);
}

// fwrite
size_t fwrite(void *ptr, size_t size, size_t nmemb, struct FILE_HANDLE *f)
{
    if (!f || !ptr || size == 0 || nmemb == 0)
        return 0;
    if (!(f->mode & 2) && !(f->mode & 8))
    {
        f->error = 1;
        return 0;
    }

    size_t total = size * nmemb;
    if (total == 0)
        return 0;

    DWORD toWrite = (DWORD)total;
    DWORD actuallyWritten = 0;
    BOOL ok = WriteFile(f->handle, (char *)ptr, toWrite, &actuallyWritten, 0);
    if (!ok)
    {
        f->error = 1;
        return 0;
    }

    f->pos += actuallyWritten;
    if (f->pos > f->size)
        f->size = f->pos;
    return (size_t)(actuallyWritten / (DWORD)size);
}

// fseek
int fseek(struct FILE_HANDLE *f, long long offset, int whence)
{
    if (!f)
        return -1;
    struct LARGE_INTEGER li;
    li.QuadPart = offset;
    struct LARGE_INTEGER newpos;
    BOOL ok = SetFilePointerEx(f->handle, li, &newpos, (DWORD)whence);
    if (!ok)
    {
        f->error = 1;
        return -1;
    }
    if (newpos.QuadPart < 0)
        f->pos = 0;
    else
        f->pos = (size_t)newpos.QuadPart;
    f->eof = 0;
    return 0;
}

// ftell
long long ftell(struct FILE_HANDLE *f)
{
    if (!f)
        return -1;
    return (long long)f->pos;
}

// remove_file (no ternary)
int remove_file(char *path)
{
    BOOL ok;
    if (!path)
        return -1;
    ok = DeleteFileA(path);
    if (ok)
        return 0;
    else
        return -1;
}

// make_dir (no ternary)
int make_dir(char *path)
{
    BOOL ok;
    if (!path)
        return -1;
    ok = CreateDirectoryA(path, 0);
    if (ok)
        return 0;
    else
        return -1;
}
