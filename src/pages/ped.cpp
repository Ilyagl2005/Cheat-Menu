#include "pch.h"
#include "ped.h"
#include <CPopulation.h>
#include "weapon.h"
#include "utils/widget.h"
#include "utils/util.h"

#ifdef GTASA
#include <ePedBones.h>

static const char* pedTypeList = "Civ Male\0Civ Female\0Cop\0Ballas\0Grove Street Families"
"\0Los Santos Vagos\0San Fierro Rifa\0Da Nang Boys\0Mafia\0Mountain Cloud Triads"
"\0Varrio Los Aztecas\0Gang 9\0Medic\0Dealer\0Criminal\0Fireman\0Prostitute\0";

#elif GTAVC
static const char* pedTypeList = "Civ Male\0Civ Female\0Cop (crash)\0Cubans\0Haitians\0Streetwannabe's"
"\0Diaz' Gang\0Security Guards\0Biker Gang\0Vercetti Gang\0Golfers\0Gang 9\0Emergency\0Fireman"
"\0Criminal\0Unused\0Prostitute\0Special\0";

#else
static const char* pedTypeList = "Civ Male\0Civ Female\0Cop\0Leones\0Triads\0Diablos\0Yakuza\0Yardies\0Colombians\0"
"Hoods\0unused\0unused\0Emergency\0Fireman\0Criminal\0unused\0Prostitute\0Special\0";

#endif

PedPage &pedPage = PedPage::Get();

PedPage::PedPage()
 : IPage<PedPage>(ePageID::Ped, "Window.PedPage", true)
{
    /*
    	Taken from gta chaos mod by Lordmau5 & _AG
    	TODO: Implement in VC too
    */
#ifdef GTASA
    Events::pedRenderEvent += [this](CPed *ped)
    {

        if (m_bBigHead)// || m_bThinBody)
        {
            auto animHier = GetAnimHierarchyFromSkinClump(ped->m_pRwClump);
            auto matrices = RpHAnimHierarchyGetMatrixArray (animHier);

            RwV3d scale = {0.7f, 0.7f, 0.7f};
            if (m_bThinBody)
            {
                for (int i = 1; i <= 54; i++)
                {
                    RwMatrixScale (&matrices[RpHAnimIDGetIndex (animHier, i)], &scale, rwCOMBINEPRECONCAT);
                }
            }
            scale = {3.0f, 3.0f, 3.0f};
            if (m_bBigHead)
            {
                for (int i = 5; i <= 8; i++)
                {
                    RwMatrixScale (&matrices[RpHAnimIDGetIndex (animHier, i)], &scale, rwCOMBINEPRECONCAT);
                }
            }
        }
    };
#elif GTA3
    CdeclEvent <AddressList<0x4CFE12, H_CALL>, PRIORITY_AFTER, ArgPickN<CPed*, 0>, void(CPed*)> onPreRender;
    
    onPreRender += [this](CPed* ped) 
    {
        if (!m_bBigHead)
        {
            return;
        }
        
        RwFrame* frame = ped->m_apFrames[2]->m_pFrame;

        if (frame) 
        {
            RwMatrix* headMatrix = RwFrameGetMatrix(frame);

            if (headMatrix) 
            {
                CMatrix mat;
                mat.m_pAttachMatrix = NULL;
                mat.Attach(headMatrix, false);
                mat.SetScale(3.0f);
                mat.SetTranslateOnly(0.4f, 0.0f, 0.0f);
                mat.UpdateRW();
            }
        }
    };
#endif
}

void PedPage::AddNewPed()
{
    static char name[8];
    static int model = 0;
    ImGui::InputTextWithHint(TEXT("Menu.Name"), "PEDNAME", name, 7);
    Widget::InputInt(TEXT("Ped.Model"), &model, 0, 999999);
    ImGui::Spacing();
    ImVec2 sz = Widget::CalcSize(2);
    if (ImGui::Button(TEXT("Ped.AddPed"), sz))
    {
        Command<Commands::REQUEST_MODEL>(model);
        Command<Commands::LOAD_ALL_MODELS_NOW>();
        if (Command<Commands::IS_MODEL_AVAILABLE>(model))
        {
            std::string key = std::format("Custom.{} (Added)", name);
            m_PedData.m_pData->Set(key.c_str(), std::to_string(model));
            m_PedData.m_pData->Save();
            Util::SetMessage(TEXT("Ped.AddPedMSG"));
            Command<Commands::MARK_MODEL_AS_NO_LONGER_NEEDED>(model);
        }
        else
        {
            Util::SetMessage(TEXT("Vehicle.InvalidID"));
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(TEXT("Ped.GetPlayerModel"), sz))
    {
        model = FindPlayerPed()->m_nModelIndex;
    }
}
#ifdef GTASA
void PedPage::SpawnPed(std::string& model)
#else
void PedPage::SpawnPed(std::string& cat, std::string& name, std::string& model)
#endif
{
    if (m_Spawner.m_List.size() == SPAWN_PED_LIMIT)
    {
        Util::SetMessage(TEXT("Ped.MaxLimit"));
        return;
    }

    if (BY_GAME(m_PedData.m_pData->Contains(model.c_str()), true, true))
    {
        CPlayerPed* player = FindPlayerPed();
        CVector pos = player->GetPosition();
        pos.y += 1;

        CPed* ped;
        int hplayer;
        static size_t currentSlot = 1;

#ifdef GTASA
        if (m_SpecialPedData.Contains(model.c_str()))
        {
            std::string name;
            if (m_SpecialPedData.Contains(model.c_str()))
            {
                name = m_SpecialPedData.Get(model.c_str(), "Unknown");
            }
            else
            {
                name = model;
            }

            CStreaming::RequestSpecialChar(currentSlot, name.c_str(), PRIORITY_REQUEST);
            CStreaming::LoadAllRequestedModels(true);

            Command<Commands::CREATE_CHAR>(m_Spawner.m_nSelectedPedType + 4, 290 + currentSlot, pos.x, pos.y, pos.z + 1, &hplayer);
            CStreaming::SetSpecialCharIsDeletable(290 + currentSlot);

            // SA has 10 slots
            ++currentSlot;
            if (currentSlot > 9)
            {
                currentSlot = 1;
            }
        }
#else
        if (cat == "Special") // Special model
        {
#ifdef GTA3
            Util::SetMessage(TEXT("Player.SpecialNotImplement"));
            return;
#else
            Command<Commands::LOAD_SPECIAL_CHARACTER>(currentSlot, model.c_str());
            Command<Commands::LOAD_ALL_MODELS_NOW>();

            Command<Commands::CREATE_CHAR>(m_Spawner.m_nSelectedPedType + 4, 108+currentSlot, pos.x, pos.y, pos.z + 1, &hplayer);
            Command<Commands::UNLOAD_SPECIAL_CHARACTER>(currentSlot);

            ++currentSlot;
            if (currentSlot > 21)
            {
                currentSlot = 1;
            }
#endif
        }
#endif
        else
        {
            int iModel = std::stoi(model);
            CStreaming::RequestModel(iModel, eStreamingFlags::PRIORITY_REQUEST);
            CStreaming::LoadAllRequestedModels(false);

            Command<Commands::CREATE_CHAR>(m_Spawner.m_nSelectedPedType + 4, iModel, pos.x, pos.y, pos.z + 1, &hplayer);
            CStreaming::SetModelIsDeletable(iModel);
        }

        ped = CPools::GetPed(hplayer);

        if (m_Spawner.m_bPedMove)
        {
            m_Spawner.m_List.push_back(ped);
        }
        else
        {
            Command<Commands::MARK_CHAR_AS_NO_LONGER_NEEDED>(hplayer);
        }
        ped->m_nPedFlags.bPedIsBleeding = m_Spawner.m_bPedBleed;
        ped->m_nWeaponAccuracy = m_Spawner.m_nAccuracy;
        ped->m_fHealth = m_Spawner.m_nPedHealth;
#ifdef GTASA
        if (m_Spawner.m_nWeaponId != 0)
        {
            int model = 0;
            Command<Commands::GET_WEAPONTYPE_MODEL>(m_Spawner.m_nWeaponId, &model);
            CStreaming::RequestModel(model, PRIORITY_REQUEST);
            CStreaming::LoadAllRequestedModels(false);
            Command<Commands::GIVE_WEAPON_TO_CHAR>(hplayer, m_Spawner.m_nWeaponId, 999);
        }
#endif
    }
}

void PedPage::Draw()
{
    if (ImGui::BeginTabBar("Ped", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT("Window.CheckboxTab")))
        {
            ImGui::Spacing();
            ImGui::BeginChild("CheckboxesChild");
            ImGui::Columns(2, 0, false);
#ifndef GTAVC
            Widget::Checkbox(TEXT("Ped.BigHead"), &m_bBigHead);
#endif
#ifdef GTASA
            Widget::CheckboxAddr(TEXT("Ped.ElvisEverywhere"), 0x969157);
            Widget::CheckboxAddr(TEXT("Ped.EveryoneArmed"), 0x969140);
            Widget::CheckboxAddr(TEXT("Ped.GangsControl"), 0x96915B);
            Widget::CheckboxAddr(TEXT("Ped.GangsEverywhere"), 0x96915A);
            Widget::Checkbox(TEXT("Ped.GangWars"), &CGangWars::bGangWarsActive);

            ImGui::NextColumn();

            Widget::CheckboxAddr(TEXT("Ped.PedsMayhem"), 0x96913E);
            Widget::CheckboxAddr(TEXT("Ped.PedsAtkRocket"), 0x969158);
            Widget::CheckboxAddr(TEXT("Ped.PedsRiot"), 0x969175);
            Widget::CheckboxAddr(TEXT("Ped.SlutMagnet"), 0x96915D);
            Widget::Checkbox(TEXT("Ped.ThinBody"), &m_bThinBody);
#elif GTAVC
            Widget::CheckboxAddr(TEXT("Ped.NoProstitutes"), 0xA10B99);
            Widget::CheckboxAddr(TEXT("Ped.SlutMagnet"), 0xA10B5F);
            ImGui::NextColumn();
            Widget::CheckboxAddr(TEXT("Ped.WeaponAll"), 0xA10AB3);
#else
            // Bad idea lol
            static bool pedsMayhem;
            if (Widget::Checkbox(TEXT("Ped.PedsMayhem"), &pedsMayhem))
            {
                Call<0x4911C0>();
            }
            static bool everyoneAttacksPlayer;
            if (Widget::Checkbox(TEXT("Ped.EveryoneAtk"), &everyoneAttacksPlayer))
            {
                Call<0x491270>();
            }
            ImGui::NextColumn();
            Widget::CheckboxAddr(TEXT("Ped.NastyLimbs"), 0x95CD44);
            Widget::CheckboxAddr(TEXT("Ped.WeaponAll"), 0x95CCF6);
#endif
            ImGui::Columns(1);
            ImGui::EndChild();

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Window.MenusTab")))
        {
            ImGui::Spacing();
            ImGui::BeginChild("MenusChild");
            Widget::EditAddr<float>(TEXT("Ped.PedDensityMul"), reinterpret_cast<uint>(&CPopulation::PedDensityMultiplier), 0, 1, 10);
#ifdef GTASA
            if (ImGui::CollapsingHeader(TEXT("Ped.RecruitAnyone")))
            {
                static std::vector<Widget::BindInfo> selectWeapon
                {
                    {"9mm", 0x96917C}, {"AK47", 0x96917D}, {"Rockets", 0x96917E}
                };
                Widget::EditRadioBtnAddr(TEXT("Ped.SelectWeapon"), selectWeapon);
                ImGui::Spacing();
                ImGui::Separator();
            }
#endif

            if (ImGui::CollapsingHeader(TEXT("Ped.RemovePedsRadius")))
            {
                static int removeRadius = 5;
                ImGui::InputInt(TEXT("Ped.Radius"), &removeRadius);
                ImGui::Spacing();
                if (ImGui::Button(TEXT("Ped.RemovePeds"), Widget::CalcSize(1)))
                {
                    CPlayerPed* player = FindPlayerPed();
                    for (CPed* ped : CPools::ms_pPedPool)
                    {
                        if (DistanceBetweenPoints(ped->GetPosition(), player->GetPosition()) < removeRadius
                                && ped->m_pVehicle == nullptr && ped != player)
                        {
                            Command<Commands::DELETE_CHAR>(CPools::GetPedRef(ped));
                        }
                    }
                }
                ImGui::Spacing();
                ImGui::Separator();
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Window.SpawnTab")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Ped.RemoveFrozen"), Widget::CalcSize(1)))
            {
                for (CPed* ped : m_Spawner.m_List)
                {
                    CWorld::Remove(ped);
                    ped->Remove();
                }
                m_Spawner.m_List.clear();
            }
            ImGui::Spacing();
            if (ImGui::BeginTabBar("SpawnPedBar"))
            {
                ImGui::Spacing();

                if (ImGui::BeginTabItem(TEXT("Window.Search")))
                {
                    ImGui::Spacing();
#ifdef GTASA
                    Widget::ImageList(m_PedData, fArgWrapper(pedPage.SpawnPed), 
                    [this](str &text)
                    {
                        return m_PedData.m_pData->Get(text.c_str(), "Unknown");
                    },
                    nullptr, fArgNoneWrapper(pedPage.AddNewPed));
#else
                    Widget::DataList(m_PedData, fArg3Wrapper(pedPage.SpawnPed), fArgNoneWrapper(pedPage.AddNewPed));
#endif
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(TEXT("Ped.ConfigTab")))
                {
                    ImGui::Spacing();
                    ImGui::BeginChild("PedCOnfig");
                    ImGui::Columns(2, 0, false);
                    Widget::Checkbox(TEXT("Ped.NoMove"), &m_Spawner.m_bPedMove);
                    ImGui::NextColumn();
                    Widget::Checkbox(TEXT("Ped.PedBleed"), &m_Spawner.m_bPedBleed);
                    ImGui::Columns(1);

                    ImGui::Spacing();
                    ImGui::SliderInt(TEXT("Ped.Accuracy"), &m_Spawner.m_nAccuracy, 0.0, 100.0);
                    if (ImGui::InputInt(TEXT("Ped.Health"), &m_Spawner.m_nPedHealth))
                    {
                        if (m_Spawner.m_nPedHealth > 1000)
                        {
                            m_Spawner.m_nPedHealth = 1000;
                        }

                        if (m_Spawner.m_nPedHealth < 0)
                        {
                            m_Spawner.m_nPedHealth = 0;
                        }
                    }
                    ImGui::Combo(TEXT("Ped.PedType"), &m_Spawner.m_nSelectedPedType, pedTypeList);
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(TEXT("Window.WeaponPage")))
                {
                    static std::string weaponName = "None";
                    ImGui::Spacing();
                    ImGui::Text(TEXT("Ped.SelectedWeapon"), weaponName.c_str());
                    ImGui::Spacing();
#ifdef GTASA
                    Widget::ImageList(weaponPage.m_WeaponData,
                    [this](std::string& str)
                    {
                        m_Spawner.m_nWeaponId = std::stoi(str);
                        weaponName = weaponPage.m_WeaponData.m_pData->Get(str.c_str(), "Unknown");
                    },
                    [](std::string& str)
                    {
                        return weaponPage.m_WeaponData.m_pData->Get(str.c_str(), "Unknown");
                    },
                    [](std::string& str)
                    {
                        return str != "-1"; /*Jetpack*/
                    });
#else
                    Widget::DataList(weaponPage.m_WeaponData,
                    [this](std::string& root, std::string& key, std::string& id)
                    {
                        m_Spawner.m_nWeaponId = std::stoi(id);
                        weaponName = key;
                    });
#endif
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }
    
#ifdef GTASA
        if (ImGui::BeginTabItem(TEXT("Ped.Gangs")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Ped.StartWar"), ImVec2(Widget::CalcSize(2))))
            {
                if (Util::GetLargestGangInZone() == 1)
                {
                    CGangWars::StartDefensiveGangWar();
                }
                else
                {
                    CGangWars::StartOffensiveGangWar();
                }
                CGangWars::bGangWarsActive = true;
            }
            ImGui::SameLine();
            if (ImGui::Button(TEXT("Ped.EndWar"), ImVec2(Widget::CalcSize(2))))
            {
                CGangWars::EndGangWar(true);
            }

            ImGui::Dummy(ImVec2(0, 20));
            ImGui::TextWrapped(TEXT("Ped.ZoneDensity"));
            ImGui::Spacing();

            static const char* m_GangList[] =
            {
                "Ballas", "Grove street families", "Los santos vagos", "San fierro rifa",
                "Da nang boys", "Mafia", "Mountain cloud triad", "Varrio los aztecas", "Gang9", "Gang10"
            };
            ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);
            for (int i = 0; i != 10; ++i)
            {
                CVector pos = FindPlayerPed()->GetPosition();
                CZoneInfo* info = CTheZones::GetZoneInfo(&pos, nullptr);
                int density = info->m_nGangDensity[i];
                if (ImGui::SliderInt(m_GangList[i], &density, 0, 127))
                {
                    info->m_nGangDensity[i] = static_cast<int8_t>(density);
                    Command<Commands::CLEAR_SPECIFIC_ZONES_TO_TRIGGER_GANG_WAR>();
                    CGangWars::bGangWarsActive = true;
                }
            }
            ImGui::PopItemWidth();
            static bool pluginRequired = (GetModuleHandle("ExGangWars.asi") == 0); 
            if (pluginRequired)
            {
                ImGui::Spacing();
                ImGui::TextWrapped(TEXT("Ped.ExGangWarsTip"));
                ImGui::Spacing();
                if (ImGui::Button(TEXT("Ped.DownloadExGangWars"), Widget::CalcSize(1)))
                {
                    OPEN_LINK("https://gtaforums.com/topic/682194-extended-gang-wars/");
                }
            }

            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}
