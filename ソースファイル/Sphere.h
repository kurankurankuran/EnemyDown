// ‹… [Sphere.h]
#pragma once
#include "Mesh.h"

class CSphere : public CMesh
{
public:
	CSphere();
	virtual ~CSphere();

	HRESULT Init(int nSlice = 16, int nStack = 8, float fRadius = 0.5f);
};
