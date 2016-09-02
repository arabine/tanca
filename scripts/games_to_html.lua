
local header = [[

<html><header><style type="text/css">
 body {
            font-family: verdana, arial, sans-serif ;
            font-size: 12px ;
            }
        th,
        td {
            padding: 4px 4px 4px 4px ;
            text-align: center ;
            }
        th {
            border-bottom: 2px solid #333333 ;
            }
        td {
            border-bottom: 1px dotted #999999 ;
            }
        tfoot td {
            border-bottom-width: 0px ;
            border-top: 2px solid #333333 ;
            padding-top: 20px ;
            }


</style></header><body>

]]

-- see if the file exists
function file_exists(file)
  local f = io.open(file, "rb")
  if f then f:close() end
  return f ~= nil
end


print(header)
  
local rounds = 0
local file_name = arg[1]
local FS = ";"            -- field separator

local games = {} -- each entry is a round
local thead = ""
local csv_line = 0

if file_exists(file_name) then

	for line in io.lines(file_name) do     

		line = line:gsub( "<", "&lt;" )
		line = line:gsub( ">", "&gt;" )
		line = line .. FS
	
		local str = ""
		local cell = "td";
		
		if csv_line == 0 then
			cell = "th"
		end
		
		local column = 0
		local round = 1
		for field in string.gmatch( line, "(.-)["..FS.."?]" ) do
			if column == 0 then
				-- nothing, skip
			elseif column == 1 then
				if csv_line ~= 0 then
					round = tonumber(field)
					if round > rounds then
						rounds = round
					end
				end
			else
				if column > 3 then
					-- Do not show any score if the match has not been played
					if tonumber(field) == -1 then
						field = ""
					end
				end
				str = str .. "<".. cell .. ">" .. field .. "</".. cell .. ">"    
			end
			column = column + 1
		end
		
		if csv_line == 0 then
			thead = "\t<table><thead><tr>" .. str .. "\t</tr></thead><tbody>"			
		else 
			str = "\t<tr>" .. str .. "\t</tr>"
			table.insert(games, { round=round, data=str } )
		end

		csv_line = csv_line + 1
	end 
end

 for i=1, rounds, 1 do
	print("<h1>Tour " .. i .. "</h1>")
	print(thead)
	for k, game in ipairs(games) do
		if game.round == i then
			print (game.data)
		end
	end
	print("\t</tbody></table>")
end

print("<body></html>")
