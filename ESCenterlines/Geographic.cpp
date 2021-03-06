#include "Geographic.h"

CGeographic::CGeographic()
	: geodesic(GeographicLib::Geodesic::WGS84())
{
}


CGeographic::~CGeographic()
{
}

void CGeographic::CalculateExtendedCenterline(const CRunway& runway, CExtendedCenterline* cl, const CCoordinate* coordinate, const DWORD color, std::vector<std::unique_ptr<CLine>>& l)
{
	auto course { 0.0 };
	auto threshold = runway.GetThresholdPosition();
	
	if (coordinate)
		geodesic_line = geodesic.InverseLine(threshold.m_Latitude, threshold.m_Longitude, coordinate->m_Latitude, coordinate->m_Longitude);
	else
	{
		if (!(course = cl->GetCourse()))
			course = GetAzimuth(runway.GetThresholdPosition(), runway.GetStopEndPosition());
		geodesic_line = geodesic.Line(threshold.m_Latitude, threshold.m_Longitude, course + 180);
	}
	CalculateCenterline(runway, cl, color, l);
	CalculateRangeTicks(runway, cl, color, l);
}

double CGeographic::GetAzimuth(const CCoordinate & c1, const CCoordinate & c2)
{
	double azi, azi2;
	geodesic.Inverse(c1.m_Latitude, c1.m_Longitude, c2.m_Latitude, c2.m_Longitude, azi, azi2);
	return azi;
}

CCoordinate CGeographic::GetCoordinate(CCoordinate c1, double azimuth, double distance)
{
	double lat2, lon2;
	geodesic.Direct(c1.m_Latitude, c1.m_Longitude, azimuth, distance, lat2, lon2);
	return CCoordinate(lat2, lon2);
}

CCoordinate CGeographic::GetCoordinate(double distance)
{
	CCoordinate coord {};
	geodesic_line.Position(distance, coord.m_Latitude, coord.m_Longitude);
	return coord;
}

double CGeographic::GetDistance(const CCoordinate& c1, const CCoordinate & c2)
{
	return 0.0;
}

void CGeographic::CalculateCenterline(const CRunway& runway, const CExtendedCenterline* centerline, const DWORD color, std::vector<std::unique_ptr<CLine>>& l)
{
	auto pos = 0.0;
	for (auto & cl : centerline->GetElements())
	{
		auto pattern_length = cl.dash_length + cl.gap_length;
		for (auto i = 0; i < cl.number; ++i)
		{
			auto line_start = i * pattern_length + pos;
			if (cl.starts_gap)
				line_start += cl.gap_length;
			auto line_end = line_start + cl.dash_length;
			auto c1 = GetCoordinate(line_start * GeographicLib::Constants::nauticalmile());
			auto c2 = GetCoordinate(line_end * GeographicLib::Constants::nauticalmile());
			l.push_back(std::make_unique<CLine>(runway.GetId(), c1, c2, color));
		}
		pos += pattern_length * cl.number;
	}
}

void CGeographic::CalculateRangeTicks(const CRunway& runway, const CExtendedCenterline* centerline, const DWORD color, std::vector<std::unique_ptr<CLine>>& l)
{
	for (auto & rt : centerline->GetMarkers())
	{
		auto tick_azimuth_left = geodesic_line.Azimuth() + rt.angle;
		auto tick_azimuth_right = geodesic_line.Azimuth() - rt.angle;
		auto c_base = GetCoordinate(rt.dist_thr * GeographicLib::Constants::nauticalmile());
		if (rt.direction == Direction::left || rt.direction == Direction::both)
		{
			auto c1_left = GetCoordinate(c_base, tick_azimuth_left, rt.dist_cl * GeographicLib::Constants::nauticalmile());
			auto c2_left = GetCoordinate(c1_left, tick_azimuth_left, rt.length * GeographicLib::Constants::nauticalmile());
			if (rt.depends_on)
				l.push_back(std::make_unique<CLine>(runway.GetId(), c1_left, c2_left, color, *rt.depends_on));
			else
				l.push_back(std::make_unique<CLine>(runway.GetId(), c1_left, c2_left, color));
		}
		if (rt.direction == Direction::right || rt.direction == Direction::both)
		{
			auto c1_right = GetCoordinate(c_base, tick_azimuth_right, rt.dist_cl * GeographicLib::Constants::nauticalmile());
			auto c2_right = GetCoordinate(c1_right, tick_azimuth_right, rt.length * GeographicLib::Constants::nauticalmile());
			if (rt.depends_on)
				l.push_back(std::make_unique<CLine>(runway.GetId(), c1_right, c2_right, color, *rt.depends_on));
			else
				l.push_back(std::make_unique<CLine>(runway.GetId(), c1_right, c2_right, color));
		}
	}
}

