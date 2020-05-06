#pragma once
#include <memory>
#include "Coordinate.h"
#include "Identifier.h"

class CLine
{
public:
	CLine(const Identifier& id, const CCoordinate & coordinate1, const CCoordinate & coordinate2, const DWORD& color);
	CLine(const Identifier& id, const CCoordinate & coordinate1, const CCoordinate & coordinate2, const DWORD& color, Identifier depends_on);
	virtual ~CLine();

	const CCoordinate& C1() const;
	const CCoordinate& C2() const;
	const Identifier* DependsOn();
	const Identifier& GetId() const;
	const DWORD& GetColor() const;

private:
	CCoordinate c1;
	CCoordinate c2;
	Identifier id;
	std::unique_ptr<Identifier> depends_on;
	DWORD color;

};
