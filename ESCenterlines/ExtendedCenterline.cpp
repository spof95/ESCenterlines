#include "ExtendedCenterline.h"



CExtendedCenterline::CExtendedCenterline()
{
	range_ticks.push_back(CRangeTick(4, 0.5, 1));
	range_ticks.push_back(CRangeTick(10, 0.5, 1));
	range_ticks.push_back(CRangeTick(20, 0.5, 1));
	centerline_elements.push_back(CCenterlineElement(1, 1, 20, true));
}


CExtendedCenterline::~CExtendedCenterline()
{
}
