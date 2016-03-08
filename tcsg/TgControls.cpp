#include "StdAfx.h"
#include "TgControls.h"
using namespace ShinyNova;

double GetSpeed(int x1, int y1, int x2, int y2, DWORD dwTimes)
{
	if (dwTimes>0)
	{
		double unit=sqrt(pow(x1-x2, 2) + pow(y1-y2, 2)) / dwTimes;  
		return unit*30;
	}
	
	return 30;
}

CTgPlaneObject::CTgPlaneObject()
{

}

CTgPlaneObject::~CTgPlaneObject()
{

}

void CTgPlaneObject::DrawFlowPic(SNG_PIC& pic,int x/*=0*/,int y/*=0*/,int width/*=0*/,int height/*=0*/,double angle/*=0*/,double alpha/*=1.0*/)
{
	if (!pic.hbit) return;
	
	if (width==0)
		width=GetPlaneWidth();
	else if (width<0)
		width=pic.width;
	
	if (height==0)
		height=GetPlaneHeight();
	else if (height<0)
		height=pic.height;
	
	
	COLORDATA hdata={m_PlaneData,GetPlaneWidth(),GetPlaneHeight(),COLOR_BIT_32,GetTransColor()};
//	SNG_AREA harea={angle,x+(width+1)/2,y+(height+1)/2,0,0,0,0,width,height,Radii(width,height),0,0,0,0,0};
	SNG_AREA harea={angle,x+(width+1)/2,y+(height+1)/2,0,0,0,0,width,height,Radii(width,height),AREA_PNG_SOLID,0,0,0,0};
	if (pic.hbit->nPicPerRow>1)
	{
		SNGPICRES singepic=CreateSingleFrame(pic.hbit,pic.nFrame);
		DrawStream(hdata,harea,*(HBITRES*)&singepic,pic.x,pic.y,pic.width,pic.height,alpha,pic.BackColor);
		delete []singepic.Bits;
	}
	else
	{
		DrawStream(hdata,harea,*(HBITRES*)pic.hbit,pic.x,pic.y+pic.nFrame*pic.hbit->height,pic.width,pic.height,alpha,pic.BackColor);//m_obPtr->GetBackColor());
	}
	
	CheckPlane();
}

CTgLightFlow::CTgLightFlow()
{
	memset(&m_picBack,0,sizeof(SNG_PIC));
	memset(&m_picFlow,0,sizeof(SNG_PIC));
	m_bStartFlow=FALSE;
	m_dwBeginTick=0;
	m_dwLastTick=0;
	m_dwMode=LIGHTFLOW_MODE_NORMAL;
	m_hUpdate=0;
}

CTgLightFlow::~CTgLightFlow()
{
	
}

PlaneObject*    SNGAPI CTgLightFlow::CreatePlane(int nWidth,int nHeight,UINT backcolor/*=COLOR_TRANSPARENT*/)
{
	ObjectModule* pOM=GetObjectModule();

	CTgPlaneObject* new_object=new CTgPlaneObject;		
	new_object->CreatePlane(nWidth,nHeight,backcolor,NULL);//&(pOM->m_DefaultFont));
	
	pOM->RegistNewObject(new_object,this);

	return (PlaneObject*)new_object;
}

void CTgLightFlow::CreateGUI(int width, int height, SNG_PIC picBk, SNG_PIC picFlow, DWORD freq, double speed, DWORD dwMode)
{			
	m_plBackGround=(CTgPlaneObject*)CreatePlane(width,height);
	SetFrame(m_plBackGround);

	m_picBack=picBk;
	m_picFlow=picFlow;
	
	if (freq<=GetMinApplySpace())
		freq=GetMinApplySpace();
	m_dwFreq=freq;

	if (speed>=GetWidth())
		speed=5;
	m_speed=speed;

	m_dwMode=dwMode;
}

void CTgLightFlow::SetBackPic(SNG_PIC& pic)
{
	m_picBack=pic;
}

void CTgLightFlow::SetFLowPic(SNG_PIC& pic)
{
	m_picFlow=pic;
}

void CTgLightFlow::ShowPic()
{
	if (!m_plBackGround)
		return;

	m_plBackGround->Erasure();

	if (m_picBack.hbit)
	{
		m_plBackGround->DrawPic(m_picBack,0,0,-1,-1);
	}

	if (m_bStartFlow)
	{
		if (m_picFlow.hbit)
		{
			if (GetTickCount()-m_dwLastTick<m_dwFreq)
				return;//Ê±¼äÎ´µ½			

			int x=-m_picFlow.width;  			
			int xDiff = ((GetTickCount()-m_dwBeginTick)/m_dwFreq) * m_speed;
			if (xDiff<0)
			{
				xDiff=0;
			}
			else
			{
				int width=GetWidth();
  				xDiff %= width+m_picFlow.width;
			}
			x+=xDiff;

			if (m_dwMode==LIGHTFLOW_MODE_NOTALPHA)
				m_plBackGround->DrawFlowPic(m_picFlow,x,0,-1,-1);
			else
				m_plBackGround->DrawPic(m_picFlow,x,0,-1,-1);

			m_dwLastTick = GetTickCount();
		}
	}
}

void CTgLightFlow::StopFlow()
{
	StopApply(m_hUpdate);
	m_hUpdate=0;

	m_bStartFlow=FALSE;
//	m_dwLastTick=0;

	ShowPic();
}

void CTgLightFlow::StartFlow()
{
	StopFlow();

	m_bStartFlow=TRUE;
	m_dwBeginTick=GetTickCount();

	UpdateFlow();
}

void CTgLightFlow::UpdateFlow()
{
	ShowPic();

	StopApply(m_hUpdate);
	m_hUpdate=APPLY_WaitFewTimes(m_dwFreq,&(SNG_MSG)CreateMsg(GetHandle(),MSG_LIGHTFLOW_UPDATE));	
}

BOOL CTgLightFlow::OnMsg(SNG_MSG& Msg)
{
	switch (Msg.msgID)
	{
	case MSG_LIGHTFLOW_UPDATE:
		{
			UpdateFlow();
		}
		break;
	default:
		return CShinyNovaGui::OnMsg(Msg);
	}

	return TRUE;
}

CTgLightFlowEx::CTgLightFlowEx()
{
	memset(&m_picBack,0,sizeof(SNG_PIC));
	m_plBack=NULL;
	m_pFlow=NULL;
}

CTgLightFlowEx::~CTgLightFlowEx()
{

}

void CTgLightFlowEx::CreateGUI(int width, int height, SNG_PIC picBK, SNG_PIC picFlowBK, SNG_PIC picFlow,
							   DWORD freq, double speed, DWORD dwMode)
{			
	m_picBack=picBK;
	m_plBack=CreatePlane(width,height);
	SetFrame(m_plBack);
	m_plBack->DrawPic(m_picBack,0,0,-1,-1);

	PGL_MSG_CLASS(m_pFlow, CTgLightFlow);
	m_pFlow->CreateGUI(width,height,picFlowBK,picFlow,freq,speed,dwMode);
	AddChild(m_pFlow,OBJECT_LOCK_ALL);
}

void CTgLightFlowEx::ShowPic()
{
	if (!m_plBack)
		return;

	m_plBack->Erasure();
}

void CTgLightFlowEx::SetBackPic(SNG_PIC& picBack, SNG_PIC& picFlowBk)
{
	m_picBack=picBack;
	m_plBack->Erasure();
	m_plBack->DrawPic(m_picBack,0,0,-1,-1);
	
	m_pFlow->SetBackPic(picFlowBk);
}

void CTgLightFlowEx::StopFlow()
{
	m_pFlow->StopFlow();	
}

void CTgLightFlowEx::StartFlow()
{
	m_pFlow->StartFlow();
}

