#include "ESCenterlinesScreen.h"

CESCenterlinesScreen::CESCenterlinesScreen(FILETIME& sTime, CCenterlineSettings& centerline_settings)
	: ActiveRunwaysUpdateTime(sTime), centerline_settings(centerline_settings)
{
	ActiveRunwaysLastUpdateTime = sTime;
}


CESCenterlinesScreen::~CESCenterlinesScreen()
{
}

void CESCenterlinesScreen::OnAsrContentLoaded(bool Loaded)
{
	InitAsrSettings();
	LoadRunwayData();
	CreateLines();
	RefreshActiveRunways();
}

void CESCenterlinesScreen::OnAsrContentToBeClosed()
{
	SaveDataToAsr(DISPLAY_CENTERLINES, DISPLAY_CENTERLINES, display_centerlines ? "1" : "0");
	SaveDataToAsr(DISPLAY_ACTIVE, DISPLAY_ACTIVE, display_active ? "1" : "0");
	delete this;
}

void CESCenterlinesScreen::OnAsrContentToBeSaved(void)
{
	SaveDataToAsr(DISPLAY_CENTERLINES, DISPLAY_CENTERLINES, display_centerlines ? "1" : "0");
	SaveDataToAsr(DISPLAY_ACTIVE, DISPLAY_ACTIVE, display_active ? "1" : "0");
}

bool CESCenterlinesScreen::OnCompileCommand(const char * sCommandLine)
{
	if (!strcmp(sCommandLine, ".cline active"))
	{
		display_active = !display_active;
		RefreshMapContent();
		return true;
	}
	if (!strcmp(sCommandLine, ".cline display"))
	{
		display_centerlines = !display_centerlines;
		RefreshMapContent();
		return true;
	}
	if (!strcmp(sCommandLine, ".cline save"))
	{
		//centerline_settings.Save(runways);
		return true;
	}
	if (!strcmp(sCommandLine, ".cline reload"))
	{
		centerline_settings.Reload();
		LoadRunwayData();
		CreateLines();
		RefreshActiveRunways();
		return true;
	}
	return false;
}

void CESCenterlinesScreen::OnRefresh(HDC hDC, int Phase)
{
	if (Phase == EuroScopePlugIn::REFRESH_PHASE_BACK_BITMAP)
		DrawExtendedCenterlines(hDC);
	if (IsDataUpdated())
		RefreshActiveRunways();
}

void CESCenterlinesScreen::CreateLines()
{
	lines.clear();
	for (auto& r : runways)
	{
		auto cl = centerline_settings.GetExtendedCenterline(r->GetId());
		auto fap = GetFixCoordinate(cl->GetFinalApproachFix(), r->GetThresholdPosition());
		auto color = cl->GetColor();
		geographic.CalculateExtendedCenterline(*r, cl, fap.get(), color, lines);
	}
}

void CESCenterlinesScreen::DrawExtendedCenterlines(HDC & hDC)
{
	if (!display_centerlines)
		return;
	auto color = RGB(200, 200, 200);
	auto pen = CreatePen(BS_SOLID, 1, color);
	auto old_pen = SelectObject(hDC, pen);
	DrawLines(hDC);
	SelectObject(hDC, old_pen);
	DeleteObject(pen);
}

void CESCenterlinesScreen::DrawLines(HDC & hDC)
{
	for (auto & line : lines)
	{
		if (display_active && !IsRunwayActive(line->GetId()))
			continue;
		if (line->DependsOn() && (!display_active || IsRunwayActive(*line->DependsOn())))
			continue;
		auto color = line->GetColor();
		auto pen = CreatePen(BS_SOLID, 1, color);
		auto oldPen = SelectObject(hDC, pen);
		auto pp1 = ConvertCoordFromPositionToPixel(line->C1());
		auto pp2 = ConvertCoordFromPositionToPixel(line->C2());
		MoveToEx(hDC, pp1.x, pp1.y, nullptr);
		LineTo(hDC, pp2.x, pp2.y);
		SelectObject(hDC, oldPen);
		DeleteObject(pen);
		
	}
}

std::unique_ptr<CCoordinate> CESCenterlinesScreen::GetFixCoordinate(const std::string& name, const CCoordinate& threshold)
{
	if (!name.length())
		return nullptr;
	for (auto se = GetPlugIn()->SectorFileElementSelectFirst(EuroScopePlugIn::SECTOR_ELEMENT_FIX);
		 se.IsValid();
		 se = GetPlugIn()->SectorFileElementSelectNext(se, EuroScopePlugIn::SECTOR_ELEMENT_FIX))
	{
		if (se.GetName() == name)
		{
			CCoordinate c;
			se.GetPosition(&c, 0);
			if (c.DistanceTo(threshold) <= 30)
				return std::make_unique<CCoordinate>(c);
		}
	}
	return nullptr;
}

void CESCenterlinesScreen::InitAsrSettings()
{
	if (!(GetDataFromAsr(DISPLAY_CENTERLINES)))
		SaveDataToAsr(DISPLAY_CENTERLINES, DISPLAY_CENTERLINES, "1");
	if (!strcmp(GetDataFromAsr(DISPLAY_CENTERLINES), "0"))
		display_centerlines = false;
	if (!(GetDataFromAsr(DISPLAY_ACTIVE)))
		SaveDataToAsr(DISPLAY_ACTIVE, DISPLAY_ACTIVE, "1");
	if (!strcmp(GetDataFromAsr(DISPLAY_ACTIVE), "0"))
		display_active = false;
}

bool CESCenterlinesScreen::IsRunwayActive(const Identifier& identifier) const
{
	for (auto & id : active_runways)
	{
		if (id == identifier)
			return true;
	}
	return false;
}

void CESCenterlinesScreen::LoadRunwayData()
{
	runways.clear();
	for (auto se = GetPlugIn()->SectorFileElementSelectFirst(EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY);
		 se.IsValid();
		 se = GetPlugIn()->SectorFileElementSelectNext(se, EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY))
	{
		for (auto i = 0; i < 2; ++i)
		{
			Identifier id(se.GetAirportName(), se.GetRunwayName(i));
			runways.push_back(std::make_unique<CRunway>(id, se, i));
		}
	}
}

void CESCenterlinesScreen::RefreshActiveRunways()
{
	ActiveRunwaysLastUpdateTime = ActiveRunwaysUpdateTime; // Set timestamp to last update
	active_runways.clear();
	for (auto se = GetPlugIn()->SectorFileElementSelectFirst(EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY);
		 se.IsValid();
		 se = GetPlugIn()->SectorFileElementSelectNext(se, EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY))
	{
		for (auto i = 0; i < 2; ++i)
			if (se.IsElementActive(false, i))
				active_runways.push_back(Identifier(se.GetAirportName(), se.GetRunwayName(i)));
	}
	RefreshMapContent();
}

inline bool CESCenterlinesScreen::IsDataUpdated() const
{
	return ActiveRunwaysUpdateTime.dwLowDateTime != ActiveRunwaysLastUpdateTime.dwLowDateTime
		|| ActiveRunwaysUpdateTime.dwHighDateTime != ActiveRunwaysLastUpdateTime.dwHighDateTime;
}

inline void CESCenterlinesScreen::DisplayMessage(std::string message)
{
	GetPlugIn()->DisplayUserMessage("Message", "ES Centerlines", message.c_str(), true, true, false, false, false);
}
