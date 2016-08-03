#ifndef __HILUO_LUAWRAP_INCLUDE_H__
#define __HILUO_LUAWRAP_INCLUDE_H__

#include "common.h"
#include <map>
#include <vector>

extern "C" 
{ 
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


namespace LabSpace
{
    namespace Common
    {
        typedef std::map<string, string>    LuaObject;
        typedef int(*lua_CFunction) (lua_State *L);

        class CLua
        {
        public:
            CLua();
            ~CLua();

            /**
             * @Function: load a lua file
             **/
            int     doFile      (const tstring& _filePath);
            int     doString    (const char* _chunk);

            void    dumpStack();

            /**
             * @Function: call a lua function.
             * @Param _func: the name of function to be called
             * @Param _params: the format of parameters and return values.
             *                 like "dd>d" means two double parameters and a double return value
             **/
            int     callFunc    (const char* _func, const char* _params, ...);

            /**
             * @Function: register a function to be called to lua
             **/
            int     regFunc     (const char* _funcName, lua_CFunction _func);

            /**
             * @Function: register an object.
             **/
            int     regClass    (const char* _className, luaL_Reg* _classMems);
            char**  makeArgv    (const char * _cmd);

            bool    loadObject  (const char* _tableName, LuaObject& _fields);
            bool    loadRecords (const char* _tableName, std::vector<LuaObject>& _records);

            bool    loadArray_s (const char* _tableName, std::vector<string>& _vec);
            bool    loadArray_i (const char* _tableName, std::vector<int>& _vec);
            bool    loadArray_d (const char* _tableName, std::vector<double>& _vec);

            bool    loadBoolean (const char* _name, bool& _value);
            bool    loadBoolean (const char* _tableName, const char* _name, bool& _value);

            bool    loadInteger (const char* _name, int& _value);
            bool    loadInteger (const char* _tableName, const char* _name, int& _value);

            bool    loadDouble  (const char* _name, double& _value);
            bool    loadDouble  (const char* _tableName, const char* _name, double& _value);

            bool    loadString  (const char* _name, string& _value);
            bool    loadString  (const char* _tableName, const char* _name, string& _value);

        private:
            void error(const tchar *_fmt, ...);

        private:
            lua_State* m_luaState;
        };
    }
}

#endif
