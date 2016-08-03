#include "string_util.h"
#include "lua_.h"
#include "stdString.h"

namespace LabSpace
{
    namespace Common
    {
        /**
        * @Function: Split string specified by delim and put the substring into vector
        **/
        int StrUtil::StrSplit(const tstring& _src, const tstring& _delim, vector<tstring>& _out)
        {
            std::size_t pos = 0;
            std::size_t found = _src.find_first_of(_delim, pos);
            while (found != tstring::npos)
            {
                if (pos != found)
                    _out.push_back(_src.substr(pos, found - pos));

                pos = found + 1;
                found = _src.find_first_of(_delim, pos);
            }

            if (pos != _src.size())
                _out.push_back(_src.substr(pos));

            return _out.size();
        }


        /**
        * @Function: Split string by substring
        **/
        int StrUtil::StrSubSplit(const tstring& _src, const tstring& _delim, vector<tstring>& _out)
        {
            std::size_t pos = 0;
            std::size_t found = _src.find(_delim, pos);
            while (found != tstring::npos)
            {
                if (found != pos)
                    _out.push_back(_src.substr(pos, found - pos));

                pos = found + _delim.size();
                found = _src.find(_delim, pos);
            }

            if (pos != _src.size())
                _out.push_back(_src.substr(pos));

            return _out.size();
        }


        /**
        * @Function: Remove the space chars at the front and end of a string
        **/
        tstring StrUtil::StrTrim(const tstring& _src)
        {
            tstring::const_iterator begin = _src.begin();
            tstring::const_iterator end   = _src.end();
            while (isspace_t(*begin)) ++begin;
            while ((end - 1) != begin && isspace_t(*(end - 1))) --end;

            return tstring(begin, end);
        }


        /**
        * @Function: Replace the szOldTokens in szSrc with newToken
        **/
        tstring StrUtil::StrReplace(const tstring& _src, const tstring& _oldTokens, tchar _newToken)
        {
            tstring tmp = _src;
            tstring::iterator srcIt = tmp.begin();
            while (srcIt != tmp.end())
            {
                for (tstring::const_iterator tokenIt = _oldTokens.begin(); tokenIt != _oldTokens.end(); ++tokenIt)
                {
                    if (*srcIt == *tokenIt)
                    {
                        *srcIt = _newToken;
                        break;
                    }
                }

                ++srcIt;
            }

            return tmp;
        }


        /**
        * @Function: Convert a command string into argv[]
        * @Param _cmd: Contain the execute command string, such as "cmd.exe param1"
        * @Param _argv [out]: keep the params
        * @Param _argc [in, out]: keep the size of _argv and return the size of params
        * @Return the duplicated buffer, should be freed
        **/
        std::shared_ptr<tchar> StrUtil::Str2Argv(const tchar* _cmd, tchar** _argv, int& _argc)
        {
            if (!_cmd || _argc < 1)   return NULL;
            const tchar* p = _cmd;
            while (p && isspace_t(*p))
                ++p;

            std::shared_ptr<tchar> buf(strdup_t(p));
            tchar* tmp = buf.get();

            int  argc = 0;
            bool hasQuote = false;

            _argv[argc++] = tmp;
            while (*tmp && _argc > argc)
            {
                if (*tmp == ' ' && !hasQuote)
                {
                    *tmp = 0;
                    while (*++tmp == ' ');  // skip the blanks

                    if (*tmp)   _argv[argc++] = tmp;
                    else break;
                }

                if (*tmp == '\'' || *tmp == '\"')
                    hasQuote = !hasQuote;

                ++tmp;
            }

            _argc = argc;
            return buf;
        }


        /**
        * @Function: convert the binary values into string
        * @Param _len [in]: the length of _p
        * @Return the memory to keep the string, user has to free it
        **/
        const char digitsHex[] = "0123456789ABCDEF";
        std::shared_ptr<tchar> StrUtil::Bin2Str(const byte* _p, size_t _len)
        {
            int size = (_len + 1) * 2;
            std::shared_ptr<tchar> to(new tchar[size]);
            to.get()[size - 1] = 0;
            tchar* addr = to.get();

            for (; _len--; _p++)
            {
                *addr++ = digitsHex[_p[0] >> 4];
                *addr++ = digitsHex[_p[0] & 0x0f];
            }
            *addr = '\0';

            return to;
        }

        size_t StrUtil::convertHex(char buf[], const void* value)
        {
            uintptr_t i = (uintptr_t)value;
            char* p = buf;

            do
            {
                int lsd = static_cast<int>(i % 16);
                i /= 16;
                *p++ = digitsHex[lsd];
            } while (i != 0);

            *p = '\0';
            std::reverse(buf, p);

            return p - buf;
        }

        /**
        * @Function: Get the file path exclude the command line
        **/
        tstring StrUtil::GetFileName(const tstring& _exePath)
        {
            tstring path = StrTrim(_exePath);
            int pos = path.find_last_of("/\\");
            int start = pos + 1;

            if (pos != string::npos)
            {
                while (pos < path.size() && path.at(pos++) != ' ');
                path = path.substr(start, pos - start - (pos == path.size() ? 0 : 1));
            }
            else
            {
                pos = path.find(" ");
                if (pos != string::npos)
                    path = path.substr(0, pos);
            }
            return path;
        }


        /**
        * @Function: Get the file path exclude the command line
        **/
        tstring StrUtil::GetFilePath(const tstring& _exePath)
        {
            tstring path = StrTrim(_exePath);
            int pos = path.find_last_of("/\\");

            if (pos != string::npos)
            {
                while (pos < path.size() && path.at(pos++) != ' ');
                path = path.substr(0, pos - (pos == path.size() ? 0 : 1));
            }
            else
            {
                pos = path.find(" ");
                if (pos != string::npos)
                    path = path.substr(0, pos);
            }
            return path;
        }    

        /**
        *@Function: Check the regular expression
        **/
        bool StrUtil::IsMatch(const tstring& _src, const tstring& _express)
        {
            const char* findReg = "function IsMatch(src, exp)   \
                m = string.match(src, exp)  \
                return m ~= nil             \
                end";

            CLua lua;
            bool b = false;
            if (0 != lua.doString(findReg))
                return false;

            lua.callFunc("IsMatch", "ss>b", _src.c_str(), _express.c_str(), &b);

            return b;
        }

        bool StrUtil::IsAllNumber(const tstring& _src)
        {
            return IsMatch(_src, tstring("^%d+$"));
        }

        bool StrUtil::IsValidPath(const tstring& _src)
        {
            tstring ilegal = "^[%w_/\\.:]+$";
            return IsMatch(_src, ilegal);
        }
    }
}