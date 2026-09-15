#include "config.h"

#include <stdlib.h>
#include <wchar.h>
#include <windows.h>

int config_read(char **out_data, size_t *out_size)
{
    const wchar_t config_suffix[] = L"\\Battle.net\\Battle.net.config";
    DWORD required_size = GetEnvironmentVariableW(L"APPDATA", NULL, 0);
    if (required_size == 0)
    {
        return 1;
    }

    wchar_t *appdata = malloc(required_size * sizeof *appdata);
    if (!appdata)
    {
        return 1;
    }

    DWORD chars_written = GetEnvironmentVariableW(L"APPDATA", appdata, required_size);
    if (chars_written == 0 || chars_written >= required_size)
    {
        free(appdata);
        return 1;
    }

    size_t path_capacity = chars_written + sizeof config_suffix / sizeof config_suffix[0];
    wchar_t *config_path = malloc(path_capacity * sizeof *config_path);
    if (!config_path)
    {
        free(appdata);
        return 1;
    }

    wmemcpy(config_path, appdata, chars_written);
    wmemcpy(config_path + chars_written, config_suffix,
            sizeof config_suffix / sizeof config_suffix[0]);
    free(appdata);

    DWORD attributes = GetFileAttributesW(config_path);
    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        free(config_path);
        return 1;
    }
    if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
    {
        free(config_path);
        return 1;
    }

    HANDLE config_file = CreateFileW(config_path, GENERIC_READ, FILE_SHARE_READ, NULL,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (config_file == INVALID_HANDLE_VALUE)
    {
        free(config_path);
        return 1;
    }

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(config_file, &file_size))
    {
        CloseHandle(config_file);
        free(config_path);
        return 1;
    }

    if (file_size.QuadPart <= 0 || file_size.QuadPart > 1024 * 1024)
    {
        CloseHandle(config_file);
        free(config_path);
        return 1;
    }

    size_t config_size = (size_t)file_size.QuadPart;
    char *config_data = malloc(config_size + 1);
    if (!config_data)
    {
        CloseHandle(config_file);
        free(config_path);
        return 1;
    }

    DWORD bytes_read = 0;
    if (!ReadFile(config_file, config_data, (DWORD)config_size, &bytes_read, NULL) ||
        bytes_read != config_size)
    {
        CloseHandle(config_file);
        free(config_path);
        free(config_data);
        return 1;
    }
    config_data[bytes_read] = '\0';

    CloseHandle(config_file);
    free(config_path);

    *out_data = config_data;
    *out_size = config_size;
    return 0;
}
