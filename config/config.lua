storage = {
	path = "/pvc/data/packetdb1|5", --;/pvc/data/packetdb2|5",  		-- format: packet storage path|GB size
	io_queue 	= 8,							-- io queue length
	page_count 	= 2048,						-- huge page memory count 
	file_size 	= 128,						-- the file size saved on disk, default is 512 MB, should >= 2
	need_warm_up = false
}

log4cplus_properties = "/etc/pvc/pktagent/pvc_pktagent_log4cplus.properties"
VENDOR_PVC	= 4;
TEST_BOOL	= true
TEST_DOUBLE	= 0.13145

function IsMatch(src, exp)
	m = string.match(src, exp)
    return m ~= nil
end


function serialize (o)
	if type(o) == "number" then
		io.write(string.format("%a", o))
	elseif type(o) == "string" then
		io.write(string.format("%q", o))
	elseif type(o) == "table" then
		io.write("{\n")
		for k,v in pairs(o) do
			io.write(" ["); 
			serialize(k); 
			io.write("] = ")
			
			serialize(v)
			io.write(",\n")
		end
		io.write("}\n")
	else
		error("cannot serialize a " .. type(o))
	end
end