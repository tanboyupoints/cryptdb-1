#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

static int
acquire_lock(lua_State *const L)
{
    const char *const path = lua_tolstring(L, 1, NULL);
    const int fd = creat(path, S_IRUSR | S_IWUSR);
    if (-1 == fd) {
        lua_pushboolean(L, false);
        lua_pushinteger(L, -1);
        fprintf(stderr, "bad fd!\n");
        return 2;
    }
    struct flock lock = {
        .l_type      = F_WRLCK,
        .l_whence    = SEEK_SET,
        .l_start     = 0,
        .l_len       = 0
    };

    const int res = fcntl(fd, F_SETLKW, &lock);
    if (-1 == res) {
        fprintf(stderr, "ERROR: fcntl errno(%d): %s\n", errno,
                                                        strerror(errno));
        lua_pushboolean(L, false);
        lua_pushinteger(L, -1);
        return 2;
    }

    lua_pushboolean(L, true);
    lua_pushinteger(L, fd);
    return 2;
}

static int
release_lock(lua_State *const L)
{
    const int fd = lua_tointeger(L, 1);
    close(fd);

    return 0;
}

static const struct luaL_reg locklib[] = {
    {"acquire_lock", acquire_lock},
    {"release_lock", release_lock},
    {NULL, NULL}
};

int
luaopen_locklib(lua_State *L)
{
    luaL_openlib(L, "locklib", locklib, 0);
    return 1;
}
