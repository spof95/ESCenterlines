#include "Line.h"

CLine::CLine(const Identifier& id, const CCoordinate & coordinate1, const CCoordinate & coordinate2, const DWORD& color)
	: id(id), c1(coordinate1), c2(coordinate2), color(color)
{
}

CLine::CLine(const Identifier& id, const CCoordinate & coordinate1, const CCoordinate & coordinate2, const DWORD& color, Identifier depends)
	: id(id), c1(coordinate1), c2(coordinate2), depends_on(std::make_unique<Identifier>(depends)), color(color)
{	
}

CLine::~CLine()
{
}

const CCoordinate& CLine::C1() const
{
	return c1;
}

const CCoordinate& CLine::C2() const
{
	return c2;
}

const Identifier* CLine::DependsOn()
{
	return depends_on.get();
}

const Identifier& CLine::GetId() const
{
	return id;
}

const DWORD& CLine::GetColor() const
{
	return color;
}