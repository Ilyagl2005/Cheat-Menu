-- Cheat Menu -  Cheat menu for Grand Theft Auto SanAndreas
-- Copyright (C) 2019 Grinch_

-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.

-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

local module = {}

module.tped =
{
    gang_wars   = imgui.new.bool(fconfig.get('tped.gang_wars',false)),
    images      = {},
    models      = {},
    names       = ftable.peds.list,
    path        = tcheatmenu.dir .. "peds\\",
    search_text = imgui.new.char[20](""),  
    selected    = nil,
    special     = ftable.peds.special,
    type        =
    {
        array   = {},
        names   = ftable.peds.types,
        index   = imgui.new.int(fconfig.get('tped.type.index',0)),
    },
}



module.tped.type.array = imgui.new['const char*'][#module.tped.type.names](module.tped.type.names)

for i = 0,#module.tped.names,1 do
    table.insert(module.tped.models,i)
end

function module.GetName(model)
    if module.tped.names[model] then return module.tped.names[model] else return "" end
end

function module.SpawnPed(model)  
    if  module.tped.names[model] ~= nil then
        if module.tped.special[model] == nil then
            requestModel(model)
            loadAllModelsNow()
            x,y,z = getCharCoordinates(PLAYER_PED)
            ped = createChar(module.tped.type.index[0]+2,model,x,y,z)
            markModelAsNoLongerNeeded(model)
            markCharAsNoLongerNeeded(ped)
        else
            if hasSpecialCharacterLoaded(model) then
                unloadSpecialCharacter(model)
            end
            loadSpecialCharacter(module.tped.special[model],1)
            loadAllModelsNow()
            x,y,z = getCharCoordinates(PLAYER_PED)
            ped = createChar(module.tped.type.index[0]+2,290,x,y,z)
            markModelAsNoLongerNeeded(module.tped.special[model])
            markCharAsNoLongerNeeded(ped)
        end
        printHelpString("Ped ~g~Spawned")
    end
end

function SetDensity(title,id)
    local x,y,z = getCharCoordinates(PLAYER_PED)
        
    local density = imgui.new.int(getZoneGangStrength(getNameOfInfoZone(x,y,z),id))
    if imgui.SliderInt(title,density,0,255) then
        setZoneGangStrength(getNameOfInfoZone(x,y,z),id,density[0])
        clearSpecificZonesToTriggerGangWar()
        setGangWarsActive(fped.tped.gang_wars[0])
    end
end

function module.PedMain()

        if imgui.BeginTabBar("Ped") then
        imgui.Spacing()
        if imgui.BeginTabItem("Checkbox") then
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({ address = 0x969157,name = "Elvis everywhere"})
            fcommon.CheckBox({ address = 0x969140,name = "Everyone is armed"})
            fcommon.CheckBox({ address = 0x96915B,name = "Gangs control the streets"})
            fcommon.CheckBox({ address = 0x96915A,name = "Gang members everywhere"})
            fcommon.CheckBox({ address = 0x96913F,name = "Have bounty on head"})
            imgui.NextColumn()
            fcommon.CheckBox({name = "Gang wars",var = module.tped.gang_wars,func = function()
                setGangWarsActive(module.tped.gang_wars[0])
                if module.tped.gang_wars[0] then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
            end})
            fcommon.CheckBox({ address = 0x969158,name = "Peds attack with rockets"})
            fcommon.CheckBox({ address = 0x969175,name = "Peds riot"})
            fcommon.CheckBox({ address = 0x96915D,name = "Slut magnet"})

            imgui.Columns(1)
            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
            fcommon.RadioButton("Recruit anyone",{"9mm","AK47","Rockets"},{0x96917C,0x96917D,0x96917E})

            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Menu") then
            fcommon.UpdateAddress({name = 'Pedestrian density multiplier',address = 0x8D2530,size = 4,min = 0,max = 10, default = 1,is_float = true})
            fcommon.DropDownMenu("Gang zone density",function()
                imgui.PushItemWidth(imgui.GetWindowWidth() - 200)
                SetDensity("Ballas",0)
                SetDensity("Da nang boys",4)
                SetDensity("Gang9",8)
                SetDensity("Gang10",9)
                SetDensity("Grove street families",1)
                SetDensity("Los santos vagos",2)
                SetDensity("Mafia",5)
                SetDensity("Mountain cloud triad",6)
                SetDensity("San fierro rifa",3)
                SetDensity("Varrio los aztecas",7)
                imgui.PopItemWidth()
                imgui.Spacing()
                imgui.Text("You'll need ExGangWars plugin to display some turf colors")
            end)
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Spawn") then
            imgui.Spacing()
            if imgui.Combo("Ped type", module.tped.type.index,module.tped.type.array,#module.tped.type.names) then end
            imgui.Spacing()
            if imgui.BeginTabBar("Peds list") then
                if imgui.BeginTabItem("Gang") then
                    if imgui.BeginChild("Gangs list Window") then
                        fcommon.ShowEntries("Ballas",{102,103,104},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Da nang boys",{121,122,123},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Grove street families",{105,106,107,269,270,271},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Los santos vagos",{108,109,110},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Mafia",{111,112,113,124,125,126,127},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Mountain cloud triad",{117,118,120},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("San fierro rifa",{173,174,175},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Varrios los aztecas",{114,115,116},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end

                if imgui.BeginTabItem("Civilians") then
                    if imgui.BeginChild("Civillians list Window") then
                        fcommon.ShowEntries("Antagonist",{290,291,292,293,294,295,296,297,298,299},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Beach",{18,45,138,139,140,154},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Bouncer",{163,164,165,166},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Boxer",{80,81},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Businessman",{17,141,147,148,150,177,227},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Country",{157,158,159,160,161,162,196,197,198,199,200},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Golf",{36,37},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Grl",{190,191,192,193,194,195},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Heckler",{258,259},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Hippie",{72,73},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Jogger",{90,96},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Karate student",{203,204},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Pol",{66,67},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Low class male",{32,33,34,128,132,133,202},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Low class female",{31,129,130,131,151,201},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Mountain biker",{51,52},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Rich male",{14,20,38,43,46,57,59,94,98,185,186,221,228,235,240,295},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Rich female",{9,12,40,53,55,88,91,169,215,216,219,224,231},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Roller blade",{92,99},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Street male",{15,22,44,48,58,60,95,101,142,170,188,222,229,236,241,242},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Street female",{10,13,39,41,54,56,69,76,93,218,225,226,232,233,246,256,257},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Tramp male",{78,79,134,135,136,137,212,213,230,239},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Tramp female",{77,256,257},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Elvis",{82,83,84},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Criminals") then
                    if imgui.BeginChild("Criminals list Window") then
                        fcommon.ShowEntries("Biker",{247,248},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Body guard",{24,25},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Criminal",{21,47,100,143,181,183,184,223,250},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Drug dealer",{28,29,30,154},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end

                if imgui.BeginTabItem("Jobs") then
                    if imgui.BeginChild("Jobs list Window") then
                        fcommon.ShowEntries("Cab driver",{182,206,220,234,261,262},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Construction",{27,153,260},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Croupier",{11,171,172},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Clothes seller",{211,217},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Fire dighter",{277,278,279},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Law enforcement",{71,265,266,267,280,281,282,283,284,285,286,287,288},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Life guard",{97,251},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Medic",{274,275,276},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Prostitute",{63,64,75,85,87,152,178,207,237,238,243,245,249},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Shop seller",{205,155,156,167,168,176,177,179,180},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Valet",{189,252,},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        fcommon.ShowEntries("Worker",{16,50,61,253,255},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Misc") then
                    if imgui.BeginChild("Misc list Window") then
                        fcommon.ShowEntries("Misc",{0,1,2,7,19,23,26,35,49,62,68,70,76,144,145,146,209,210,214,263,268,272},110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Search") then
                    imgui.Spacing()
                    imgui.Columns(1)
                    if imgui.InputText("Search",module.tped.search_text,ffi.sizeof(module.tped.search_text)) then end
                    imgui.SameLine()
        
                    imgui.Spacing()
                    imgui.Text("Foun entries :(" .. ffi.string(module.tped.search_text) .. ")")
                    imgui.Separator()
                    imgui.Spacing()
                    if imgui.BeginChild("Ped entries") then
                        fcommon.ShowEntries(nil,module.tped.models,110,55,module.tped.images,module.tped.path,".jpg",module.SpawnPed,module.GetName,true,module.tped.search_text)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                imgui.EndTabBar()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end
return module
