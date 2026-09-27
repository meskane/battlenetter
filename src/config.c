#include "config.h"

#include <stdlib.h>
#include <wchar.h>
#include <windows.h>

int config_read(char **out_data, size_t *out_size)
{
    *out_data = NULL;
    *out_size = 0;

    wchar_t *config_path = NULL;
    HANDLE config_file = INVALID_HANDLE_VALUE;
    char *config_data = NULL;
    int result = 1;

    const wchar_t config_suffix[] = L"\\Battle.net\\Battle.net.config";
    const size_t suffix_length = sizeof config_suffix / sizeof *config_suffix - 1;

    DWORD required_size = GetEnvironmentVariableW(L"APPDATA", NULL, 0);
    if (required_size == 0)
    {
        goto cleanup;
    }

    size_t path_capacity = required_size + suffix_length;
    config_path = malloc(path_capacity * sizeof *config_path);
    if (!config_path)
    {
        goto cleanup;
    }

    DWORD chars_written = GetEnvironmentVariableW(L"APPDATA", config_path, required_size);
    if (chars_written == 0 || chars_written >= required_size)
    {
        goto cleanup;
    }

    wmemcpy(config_path + chars_written, config_suffix, suffix_length + 1);

    config_file = CreateFileW(config_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (config_file == INVALID_HANDLE_VALUE)
    {
        goto cleanup;
    }

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(config_file, &file_size))
    {
        goto cleanup;
    }

    if (file_size.QuadPart <= 0 || file_size.QuadPart > 1024 * 1024)
    {
        goto cleanup;
    }

    size_t config_size = (size_t)file_size.QuadPart;
    config_data = malloc(config_size + 1);
    if (!config_data)
    {
        goto cleanup;
    }

    DWORD bytes_read = 0;
    if (!ReadFile(config_file, config_data, (DWORD)config_size, &bytes_read, NULL) ||
        bytes_read != config_size)
    {
        goto cleanup;
    }
    config_data[bytes_read] = '\0';

    *out_data = config_data;
    *out_size = config_size;
    config_data = NULL;
    result = 0;

cleanup:
    free(config_data);
    if (config_file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(config_file);
    }
    free(config_path);
    return result;
}
