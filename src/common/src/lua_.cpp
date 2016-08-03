#include "lua_.h"
#include <algorithm>

namespace LabSpace
{
    namespace Common
    {
        CLua::CLua()
        {
            m_luaState = luaL_newstate();
            luaL_openlibs(m_luaState);
        }

        CLua::~CLua()
        {
            if (m_luaState)
            {
                lua_close(m_luaState);
                m_luaState = NULL;
            }
        }

        int CLua::doFile(const tstring& _filePath)
        {
            if (!m_luaState)   return -1;

            luaL_openlibs(m_luaState); //Load base libraries
            if (luaL_loadfile(m_luaState, _filePath.c_str()) ||
                lua_pcall(m_luaState, 0, 0, 0))
            {
                error(lua_tostring(m_luaState, -1));
                lua_pop(m_luaState, 1);
                return -1;
            }

            return 0;
        }

        int CLua::doString(const char* _chunk)
        {
            if (!m_luaState || !_chunk)   return -1;
            if (luaL_dostring(m_luaState, _chunk))
            {
                error(lua_tostring(m_luaState, -1));
                lua_pop(m_luaState, 1);
                return -1;
            }

            return lua_tonumber(m_luaState, -1);
        }

        bool CLua::loadInteger(const char* _name, int& _value)
        {
            if (!_name) return false;
            bool r = false;

            lua_getglobal(m_luaState, _name);
            if (lua_isnumber(m_luaState, -1))
            {
                _value = (int)lua_tointeger(m_luaState, -1);
                r = true;
            }
            lua_pop(m_luaState, 1);

            return r;
        }

        bool CLua::loadString(const char* _name, string& _value)
        {
            if (!_name) return false;

            int n = lua_gettop(m_luaState);
            lua_getglobal(m_luaState, _name);
            if (lua_isstring(m_luaState, -1))
            {
                _value = (LPSTR)lua_tostring(m_luaState, -1);
            }

            lua_pop(m_luaState, 1);
            return !_value.empty();
        }

        bool CLua::loadDouble(const char* _name, double& _value)
        {
            if (!_name) return false;

            bool r = false;
            lua_getglobal(m_luaState, _name);
            if (lua_isnumber(m_luaState, -1))
            {
                _value = (double)lua_tonumber(m_luaState, -1);
                r = true;
            }
            lua_pop(m_luaState, 1);

            return r;
        }

        bool CLua::loadBoolean(const char* _name, bool& _value)
        {
            if (!_name) return false;

            bool r = false;
            lua_getglobal(m_luaState, _name);
            if (lua_isboolean(m_luaState, -1))
            {
                _value = (bool)lua_toboolean(m_luaState, -1);
                r = true;
            }
            lua_pop(m_luaState, 1);

            return r;
        }

        bool CLua::loadObject(const char* _tableName, LuaObject& _obj)
        {
            if (!_tableName) return false;
            int n = lua_gettop(m_luaState);
            bool r = false;

            lua_getglobal(m_luaState, _tableName);
            if (lua_istable(m_luaState, -1))
            {
                lua_pushnil(m_luaState); // the stack-1 => nil; -2 => table

                while (lua_next(m_luaState, -2))     // the stack-1 => value; -2 => key; index => table
                {
                    lua_pushvalue(m_luaState, -2);   // the stack-1 => key; -2 => value; -3 => key; index => table

                    const char* key = lua_tostring(m_luaState, -1);
                    const char* value = NULL;

                    if (lua_isnumber(m_luaState, -2))
                        value = lua_tostring(m_luaState, -2);
                    else if (lua_isstring(m_luaState, -2))
                        value = lua_tostring(m_luaState, -2);
                    else if (lua_isboolean(m_luaState, -2))
                        value = lua_toboolean(m_luaState, -2) ? "true" : "false";

                    if (key && value)
                    {
                        int size = strlen(key);
                        string strKey(size, ' ');   // must assign values here, otherwise transform will fail to work

                        transform(key, key + size, strKey.begin(), ::tolower);
                        _obj[strKey] = value;
                        r = true;
                    }

                    lua_pop(m_luaState, 2);   // the stack-1 => key; index => table           
                }

                if (_obj.size() == 0)
                    lua_pop(m_luaState, 1); // pop the nil key
            }

            lua_pop(m_luaState, 1);
            int m = lua_gettop(m_luaState);
            assert(m == n);

            return r;
        }

        bool CLua::loadString(const char* _tableName, const char* _name, string& _value)
        {
            bool b = false;
            if (!_tableName || !_name)  return b;

            lua_getglobal(m_luaState, _tableName);
            if (lua_istable(m_luaState, -1))
            {
                lua_pushstring(m_luaState, _name);
                lua_gettable(m_luaState, -2); // now the table is in the -2 and key in the top(-1)
                if (lua_isstring(m_luaState, -1))
                {
                    _value = (const char*)lua_tostring(m_luaState, -1);
                    b = true;
                }

                lua_pop(m_luaState, 2);
            }

            return b;
        }

        bool CLua::loadInteger(const char* _tableName, const char* _name, int& _value)
        {
            bool b = false;
            if (!_tableName || !_name)  return b;

            lua_getglobal(m_luaState, _tableName);
            if (lua_istable(m_luaState, -1))
            {
                lua_pushstring(m_luaState, _name);
                lua_gettable(m_luaState, -2); // now the table is in the -2 and key in the top(-1)
                if (lua_isnumber(m_luaState, -1))
                {
                    _value = (int)lua_tointeger(m_luaState, -1);
                    b = true;
                }

                lua_pop(m_luaState, 2);
            }
            lua_pop(m_luaState, 1);

            return b;
        }

        bool CLua::loadDouble(const char* _tableName, const char* _name, double& _value)
        {
            bool b = false;
            if (!_tableName || !_name)  return b;

            lua_getglobal(m_luaState, _tableName);
            if (lua_istable(m_luaState, -1))
            {
                lua_pushstring(m_luaState, _name);
                lua_gettable(m_luaState, -2); // now the table is in the -2 and key in the top(-1)
                if (lua_isnumber(m_luaState, -1))
                {
                    _value = (double)lua_tonumber(m_luaState, -1);
                    b = true;
                }

                lua_pop(m_luaState, 2);
            }
            lua_pop(m_luaState, 1);

            return b;
        }

        bool CLua::loadBoolean(const char* _tableName, const char* _name, bool& _value)
        {
            bool b = false;
            if (!_tableName || !_name)  return b;

            lua_getglobal(m_luaState, _tableName);
            if (lua_istable(m_luaState, -1))
            {
                lua_pushstring(m_luaState, _name);
                lua_gettable(m_luaState, -2); // now the table is in the -2 and key in the top(-1)
                if (lua_toboolean(m_luaState, -1))
                {
                    _value = (bool)lua_toboolean(m_luaState, -1);
                    b = true;
                }

                lua_pop(m_luaState, 2);
            }
            lua_pop(m_luaState, 1);

            return b;
        }


        bool CLua::loadRecords(const char* _tableName, std::vector<LuaObject>& _records)
        {
            if (!_tableName) return false;

            lua_getglobal(m_luaState, _tableName);
            if (lua_istable(m_luaState, -1))
            {
                lua_pushnil(m_luaState);
                lua_gettable(m_luaState, -2);

                while (lua_next(m_luaState, -2))
                {
                    lua_pushvalue(m_luaState, -2);
                    const char* key = lua_tostring(m_luaState, -1);

                    if (lua_istable(m_luaState, -2))
                    {
                        LuaObject record;

                        lua_pushnil(m_luaState);
                        lua_gettable(m_luaState, -3);
                        while (lua_next(m_luaState, -3))
                        {
                            lua_pushvalue(m_luaState, -2);

                            const char* key = lua_tostring(m_luaState, -1);
                            const char* value = "";

                            if (lua_isstring(m_luaState, -2))
                                value = lua_tostring(m_luaState, -2);
                            else if (lua_isboolean(m_luaState, -2))
                                value = lua_toboolean(m_luaState, -2) ? "true" : "false";

                            record[key] = value;

                            lua_pop(m_luaState, 2);
                        }

                        if (record.size())
                            _records.push_back(record);
                    }

                    lua_pop(m_luaState, 2);
                }
            }

            return true;
        }

        bool CLua::loadArray_s(const char* _tableName, std::vector<string>& _vec)
        {
            if (!_tableName) return false;

            bool r = false;
            int n = lua_gettop(m_luaState);
            lua_getglobal(m_luaState, _tableName);
            if (lua_istable(m_luaState, -1))
            {
                lua_pushnil(m_luaState);        // the stack��-1 => nil; -2 => table
                while (lua_next(m_luaState, -2))// -1 => value, -2 => key
                {
                    if (lua_isstring(m_luaState, -1))
                        _vec.push_back(lua_tostring(m_luaState, -1));

                    r = true;
                    lua_pop(m_luaState, 1);
                }
            }
            lua_pop(m_luaState, 1);

            int m = lua_gettop(m_luaState);
            assert(m == n);

            return r;
        }

        bool CLua::loadArray_i(const char* _tableName, std::vector<int>& _vec)
        {
            if (!_tableName) return false;
            bool r = false;
            int n = lua_gettop(m_luaState);

            lua_getglobal(m_luaState, _tableName);
            if (lua_istable(m_luaState, -1))
            {
                lua_pushnil(m_luaState);        // the stack��-1 => nil; -2 => table
                while (lua_next(m_luaState, -2))// -1 => value, -2 => key
                {
                    if (lua_isnumber(m_luaState, -1))
                        _vec.push_back(lua_tointeger(m_luaState, -1));

                    lua_pop(m_luaState, 1);
                    r = true;
                }
            }
            lua_pop(m_luaState, 1);

            int m = lua_gettop(m_luaState);
            assert(m == n);

            return r;
        }

        bool CLua::loadArray_d(const char* _tableName, std::vector<double>& _vec)
        {
            if (!_tableName) return false;
            bool r = false;
            int n = lua_gettop(m_luaState);

            lua_getglobal(m_luaState, _tableName);
            if (lua_istable(m_luaState, -1))
            {
                lua_pushnil(m_luaState);        // the stack��-1 => nil; -2 => table
                while (lua_next(m_luaState, -2))// -1 => value, -2 => key
                {
                    if (lua_isnumber(m_luaState, -1))
                        _vec.push_back(lua_tonumber(m_luaState, -1));

                    lua_pop(m_luaState, 1);
                    r = true;
                }
            }
            lua_pop(m_luaState, 1);

            int m = lua_gettop(m_luaState);
            assert(m == n);

            return r;
        }

        int CLua::callFunc(const char* _func, const char* _params, ...)
        {
            if (!_func) return -1;
            int nowTop = lua_gettop(m_luaState);

            lua_getglobal(m_luaState, _func); // push function name
            if (!lua_isfunction(m_luaState, -1))
            {
                lua_settop(m_luaState, nowTop);
                return -1;
            }

            va_list args;
            va_start(args, _params);

#ifdef WIN32
//            vl = (va_list)va_arg(args, va_list);
#endif
            int nArg = 0, nRes = 0;         // number of arguments and results    
            for (nArg = 0; *_params; nArg++)
            {
                switch (*_params++)
                {
                case 'b':
                    lua_pushboolean(m_luaState, va_arg(args, bool));
                    break;

                case 'd':   // double argument
                    lua_pushnumber(m_luaState, va_arg(args, double));
                    break;

                case 'i':   // int argument
                {
                    int v = va_arg(args, int);
                    lua_pushinteger(m_luaState, v);
                }
                break;

                case 's':   // string argument
                {
                    const char* v = va_arg(args, char*);
                    lua_pushstring(m_luaState, v);
                }
                    break;

                case 'u':   // user data
                {
                    void* obj = va_arg(args, void*);
                    lua_pushlightuserdata(m_luaState, obj);
                }
                    break;

                case '>':   // end of arguments
                    goto endargs;
                default:
                    error("invalid option (%c)", *(_params - 1));
                }
            }

        endargs:

            nRes = strlen(_params); // number of expected results
            if (lua_pcall(m_luaState, nArg, nRes, 0) != 0)
                error("error calling '%s': %s", _func, lua_tostring(m_luaState, -1));

 //           dumpStack();
            nRes = -nRes;   // stack index of first result
            while (*_params)
            {
                switch (*_params++)
                {
                case 'b':
                {
                    if (!lua_isboolean(m_luaState, nRes))
                        error("wrong result type of boolean");
                    else
                    {
                        *va_arg(args, bool*) = lua_toboolean(m_luaState, nRes);
                    }
                    break;
                }
                case 'd':   // double result
                {
                    if (!lua_isnumber(m_luaState, nRes))
                        error("wrong result type double");
                    else
                    {
                        double n = lua_tonumber(m_luaState, nRes);
                        *va_arg(args, double*) = n;
                    }
                    break;
                }
                case 'i':   // int result
                {
                    if (!lua_isnumber(m_luaState, nRes))
                        error("wrong result type integer");
                    else
                    {
                        int n = lua_tointeger(m_luaState, nRes);
                        int *o = va_arg(args, int*);
                        *o = n;
                    }
                    break;
                }
                case 's':   // string result
                {
                    const char *s = lua_tostring(m_luaState, nRes);
                    if (s == NULL)
                        error("wrong result type string");
                    else
                        *va_arg(args, const char **) = s;
                    break;
                }
                case 'u':
                {
                    if (!lua_isuserdata(m_luaState, nRes))
                        error("wrong result type");
                    else
                    {
                        void* ud = lua_touserdata(m_luaState, nRes);
                        *va_arg(args, void**) = ud;
                    }
                    break;
                }
                default:
                    error("invalid option (%c)", *(_params - 1));
                }
                nRes++;
            }

            va_end(args);


            lua_settop(m_luaState, nowTop);

            return 0;
        }

        int CLua::regFunc(const char* _funName, lua_CFunction _func)
        {
            if (!_funName || !_func)  return -1;

            lua_pushcfunction(m_luaState, _func);
            lua_setglobal(m_luaState, _funName);

            return 0;
        }

        int CLua::regClass(const char* _className, luaL_Reg* _classMems)
        {
            if (!m_luaState || !_className || !_classMems) return -1;
            static const luaL_Reg meta[] = { { NULL, NULL } };

            lua_createtable(m_luaState, 0, 0);
            int libID = lua_gettop(m_luaState);

            // metatable = {}
            luaL_newmetatable(m_luaState, _className);
            int metaID = lua_gettop(m_luaState);

            lua_newtable(m_luaState);
            luaL_setfuncs(m_luaState, meta, 0);
            //   luaL_register(m_luaState, NULL, meta);

            // metatable.__index = class_methods
            lua_newtable(m_luaState);
            luaL_setfuncs(m_luaState, _classMems, 0);
            //luaL_register(m_luaState, NULL, _classMems);

            lua_newtable(m_luaState);
            lua_setfield(m_luaState, metaID, "__index");

            // class.__metatable = metatable
            lua_setmetatable(m_luaState, libID);

            // _G["Foo"] = newclass
            lua_setglobal(m_luaState, _className);

            return 0;
        }

        char** CLua::makeArgv(const char * _cmd)
        {
            if (!m_luaState)   return NULL;

            char **argv;
            int i;
            int argc = lua_gettop(m_luaState) + 1;

            if (!(argv = (char**)calloc(argc, sizeof(char *))))
                luaL_error(m_luaState, "Can't allocate memory for arguments array", _cmd);

            argv[0] = (char *)_cmd;
            for (i = 1; i < argc; i++)
            {
                if (lua_isstring(m_luaState, i) || lua_isnumber(m_luaState, i))
                {
                    if (!(argv[i] = (char*)lua_tostring(m_luaState, i)))
                    {
                        luaL_error(m_luaState, "%s - error duplicating string area for arg #%d", _cmd, i);
                    }
                }
                else
                {
                    luaL_error(m_luaState, "Invalid arg #%d to %s: args must be strings or numbers", i, _cmd);
                }
            }

            return argv;
        }

        void CLua::dumpStack()
        {
            int i;
            int top = lua_gettop(m_luaState); // depth of the stack
            for (i = 1; i <= top; i++)
            {
                int t = lua_type(m_luaState, i);
                switch (t)
                {
                case LUA_TSTRING:
                    printf("'%s'", lua_tostring(m_luaState, i));
                    break;
                case LUA_TBOOLEAN:
                    printf(lua_toboolean(m_luaState, i) ? "true" : "false");
                    break;
                case LUA_TNUMBER:
                    printf("%g", lua_tonumber(m_luaState, i));
                    break;
                default:
                    printf("%s", lua_typename(m_luaState, t));
                    break;

                    printf(" "); /* put a separator */
                }
            }
            printf("\n"); /* end the listing */
        }

        /** set a new table
        lua_newtable(m_state);
        lua_pushstring(m_state, key); // key
        lua_pushnumber(m_state, 100); // value
        lua_settable(m_state, -3);    // the table is in index 3

        lua_pushnumber(m_state, 200); // value
        lua_setfield(m_state, -2, key);

        lua_setglobal(m_state, "tablename");
        **/

        void CLua::error(const char *_fmt, ...)
        {
            va_list argp;
            va_start(argp, _fmt);
            vfprintf(stderr, _fmt, argp);
            va_end(argp);
        }
    }
}