
function serialize_table(obj) 

end

function serialize_basic(value)
	if type(value) == "number" then
		return tostring(value)
	elseif type(value) == "string" then
		return value
	elseif type(value) == "boolean" then
		if value then return "true"
		else return "false"
		end
	elseif type(value) == "table" then
		local result = "{\r\n"
		result = result .. serialize_table(value)
		result = result .. "}\r\n"
		return result
	end

	return "nil"
end

function serialize_value(name, value)
	local result = "";
	if name == nil or value == nil then return end
	result = result .. name .. " = " .. serialize_value(value);

end

function save_config()
    local tmpFileHandle = io.open("lua_saving.lua", "w")

    --concat lines with \n separator and write those to file:
    tmpFileHandle:write(table.concat(storage, "\n"))
    tmpFileHandle:close()

end

--[[
function treaverse_global_env(curtable,level)  
    for key,value in pairs(curtable or {}) do  
		local prefix = string.rep(" ",level*5)  
		print(string.format("%s%s(%s)", prefix, key, type(value)))  
	  
		if (type(value) == "table" ) and key ~= "_G" and (not value.package) then  
			treaverse_global_env(value,level + 1)  
		elseif (type(value) == "table" ) and (value.package) then  
			print(string.format("%sSKIPTABLE:%s",prefix,key))  
		end   
    end   
end  
  
treaverse_global_env(_G,0) 
]]