#include "global.h"
#include "../src/lua_.h"
using namespace LabSpace::Common;


/**
 * @Function: Work as a shared fixture to load the lua file only once
 **/
class CLuaTester : public testing::Test
{
public:
    static void SetUpTestCase()
    {
        int n = m_lua.regFunc("MyCallback", LuaCallbackFunction);
        EXPECT_EQ(0, n);
        if (0 != n)
            throw "fail to register function.";

        //
        // these are the values we will check from lua file
        //
        m_objStorage["path"]         = "/pvc/data/packetdb1|5";
        m_objStorage["io_queue"]     = "8";
        m_objStorage["page_count"]   = "2048";
        m_objStorage["file_size"]    = "128";
        m_objStorage["need_warm_up"] = "false";
        
        m_strObjGlobal["log4cplus_properties"] = "/etc/pvc/pktagent/pvc_pktagent_log4cplus.properties";
        m_intObjGlobal["VENDOR_PVC"] = "4";
        
        if (0 != m_lua.doFile(g_conf.configFile))
            throw "fail_to_load_lua_file";
    }

    static void TearDownTestCase()
    {
        m_lua.dumpStack();
    }

    static int LuaCallbackFunction(lua_State *L);
    static void prepareFile();

public:
    static CLua         m_lua;
    static LuaObject    m_objStorage;
    static LuaObject    m_intObjGlobal;
    static LuaObject    m_strObjGlobal;
    static string       m_luaStatement;
};

CLua        CLuaTester::m_lua;
LuaObject   CLuaTester::m_objStorage;
LuaObject   CLuaTester::m_intObjGlobal;
LuaObject   CLuaTester::m_strObjGlobal;
string      CLuaTester::m_luaStatement;


int CLuaTester::LuaCallbackFunction(lua_State *L)
{
    return 0;
}


TEST_F(CLuaTester, loadStorageObject)
{
    LuaObject obj;
    m_lua.loadObject("storage", obj);
    LuaObject::const_iterator it = obj.begin();

    EXPECT_TRUE(obj.size());

    for (; it != obj.end(); ++it)
        EXPECT_TRUE(obj.count(it->first));

    EXPECT_FALSE(obj.count("__fake_name__"));

    it = obj.begin();   // this found by OpenCppCoverage
    for (; it != obj.end(); ++it)
    {
        const char* result = obj[it->first].c_str();
        const char* expect = m_objStorage[it->first].c_str();
        EXPECT_STRCASEEQ(result, expect);
    }
}


TEST_F(CLuaTester, loadStrGlobalObjects)
{
    assert(m_strObjGlobal.size());

    LuaObject::const_iterator it = m_strObjGlobal.begin();
    for (; it != m_strObjGlobal.end(); ++it)
    {
        string value;
        m_lua.loadString(it->first.c_str(), value);
        EXPECT_STRCASEEQ(value.c_str(), it->second.c_str());
    }
}


TEST_F(CLuaTester, loadIntGlobalObjects)
{
    assert(m_intObjGlobal.size());

    LuaObject::const_iterator it = m_intObjGlobal.begin();
    for (; it != m_intObjGlobal.end(); ++it)
    {
        int value = 0;
        m_lua.loadInteger(it->first.c_str(), value);
        EXPECT_EQ(value, atoi(it->second.c_str()));
    }
}

TEST_F(CLuaTester, setIntGlobalObjects)
{
    string statement = "VENDOR_PVC = 100";
    m_lua.doString(statement.c_str());

    int value = 0;
    m_lua.loadInteger("VENDOR_PVC", value);
    EXPECT_EQ(value, 100);

    double d = 0;
    m_lua.loadDouble("TEST_DOUBLE", d);
    EXPECT_NEAR(d, 0.13145, 0.00001);
    ASSERT_DOUBLE_EQ(d, 0.13145);

    bool b = false;
    m_lua.loadBoolean("TEST_BOOL", b);
    EXPECT_TRUE(b);
}

TEST_F(CLuaTester, doString)
{
    EXPECT_EQ(10, m_lua.doString("return 10"));
}